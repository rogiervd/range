/*
Copyright 2011-2015 Rogier van Dalen.

This file is part of Rogier van Dalen's Range library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
This file defines a skeleton and helper classes to define range operations.
The other core_*.hpp in this directory give examples of how to define the
innards of such operations.
The operation "default_direction" is needed by this file, so it is defined here
too.
*/

#ifndef RANGE_DETAIL_CORE_BASE_HPP_INCLUDED
#define RANGE_DETAIL_CORE_BASE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/transform.hpp"

#include "rime/core.hpp"

#include "utility/returns.hpp"
#include "utility/nested_callable.hpp"

#include "callable_traits.hpp"
#include "../direction.hpp"

#include "meta_split.hpp"

#include "core_tag.hpp"

namespace range {

using ::callable_traits::has;
using ::callable_traits::result_of;
using ::callable_traits::result_of_or;

template <class Expression> struct decayed_result_of
: std::decay <typename result_of <Expression>::type> {};

/**
Implementation of operations on ranges.
Types in this namespace must be specialised to implement range operations.
This is (hopefully) as simple as possible.
*/
namespace operation {

    // Forward declarations for operations so they can refer to each other.
    template <class RangeTag, class Range, class Enable = void>
        struct default_direction;
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct empty;
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct size;
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct first;
    template <class RangeTag, class Direction, class Increment, class Range,
            class Enable = void>
        struct drop;
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct chop;
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct chop_in_place;
    template <class RangeTag, class Direction, class Index, class Range,
            class Enable = void>
        struct at;

    /**
    Base class for marking an operation as not implemented.
    Operations should be marked as unimplemented when their return type cannot
    be computed, and in other cases where it can be decided at compile-time
    that an operation is not be implemented.
    */
    struct unimplemented : ::callable_traits::unimplemented {
        typedef unimplemented type;
    };

    using ::callable_traits::is_implemented;

    namespace helper {
        /**
        Operation that returns a default-constructed object.
        */
        template <class ResultType> struct return_default_constructed {
            template <class... Arguments>
                ResultType operator() (Arguments &&...) const
            { return ResultType(); }
        };

        struct return_rvalue_reference {
            template <class Argument>
                Argument && operator() (Argument && argument) const
            { return static_cast <Argument &&> (argument); }
        };

        /**
        Operation that skips the arguments that are given and returns only the
        result of the function applied to the last "Number" arguments.
        */
        template <unsigned Number, class SkipArguments, class Function>
            struct call_with_last;

#if !(BOOST_CLANG && __clang_major__ == 3 && __clang_minor__ == 0)
        template <unsigned Number, class ... SkipArguments, class Function>
            struct call_with_last <
                Number, meta::vector <SkipArguments ...>, Function>
        {
            template <class ... LastArguments> typename
                boost::lazy_enable_if_c <sizeof ... (LastArguments) == Number,
                std::result_of <Function (LastArguments ...)>>::type
                operator() (SkipArguments const & ... skip_arguments,
                    LastArguments && ... last_arguments) const
            {
                return Function() (
                    std::forward <LastArguments> (last_arguments) ...);
            }
        };
#else
        // We would like to return the last argument.
        // CLang 3.0 confuses types when unpacking SkipArguments ... with
        // LastArgument && last_argument.
        // Workaround:
        template <unsigned Number, class Function>
            struct call_with_last <Number, meta::vector<>, Function>
        {
            template <class ... LastArguments> typename
                boost::lazy_enable_if_c <sizeof ... (LastArguments) == Number,
                std::result_of <Function (LastArguments ...)>>::type
                operator() (LastArguments && ... last_arguments) const
            {
                return Function() (
                    std::forward <LastArguments> (last_arguments) ...);
            }
        };

        template <unsigned Number, class SkipArguments, class Function>
            struct call_with_last
        {
            typedef call_with_last <Number,
                typename meta::drop <SkipArguments>::type, Function> recursive;

            template <class FirstArgument, class ... OtherArguments>
                auto operator() (FirstArgument const & first_argument,
                    OtherArguments && ... other_arguments) const
            RETURNS (recursive() (
                std::forward <OtherArguments> (other_arguments)...));
        };
#endif

    } // namespace helper

    namespace range_detail {

        /**
        Return whether the operation is implemented for the forward direction
        of Direction.
        */
        template <template <class, class, class, class> class Apply,
            class RangeTag, class Direction, class Range>
        struct is_implemented_forward
        : is_implemented <Apply <RangeTag, typename result_of <
            direction::callable::make_forward (Direction)>::type, Range, void>>
        {};

        /**
        Perform operation Apply that takes a direction and a range with the
        reverse direction.
        \pre make_forward <Direction> is defined.
        */
        template <template <class, class, class, class> class Apply,
            class RangeTag, class Direction, class Range>
        struct forward_operation {
            Apply <RangeTag, typename
                result_of <direction::callable::make_forward (Direction)>::type,
                Range, void> implementation;

            auto operator() (Direction const & direction, Range && range) const
            RETURNS (implementation (::direction::make_forward (direction),
                std::forward <Range> (range)));
        };

    } // namespace range_detail

    /**
    Evaluate to \c void.
    This can be used when \a ResultType may or may not be a valid type, like the
    return type of a call to a member function.
    */
    template <class ResultType> struct enable_if_member { typedef void type; };

    /**
    Derive from the first operation if it is implemented.
    Otherwise, derive from the second operation if it is implemented.
    And so on.
    If none of the operations are implemented, derive from \c unimplemented.
    */
    template <class ... Operations> struct try_all;

    template <> struct try_all <> : unimplemented {};

    template <class Operation, class ... OtherOperations>
        struct try_all <Operation, OtherOperations ...>
    : boost::mpl::if_ <is_implemented <Operation>,
        Operation, try_all <OtherOperations ...>>::type {};

} // namespace operation

/**
Namespace with structures that normalise the parameters for the structures
in the namespace "operation".
They take as class template parameters the arguments that the functors (in
callable) take, and remove cv-qualifications from directions and increments.

The classes are defined for all template parameters, and are implemented
(i.e. do not derive from operation::unimplemented, and have an operator()) only
for the relevant ones.
*/
namespace apply {
    /*
    To allow the classes in callable:: to be implemented easily, with
    template template parameters, the number of template parameters should be
    variadic.
    */
} // namespace apply

/**** Functions that can actually be called. *****/

namespace callable {
    using ::callable_traits::generic;
} // namespace callable

static const direction::front front = {};
static const direction::back back = {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_BASE_HPP_INCLUDED
