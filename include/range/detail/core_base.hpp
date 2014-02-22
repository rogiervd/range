/*
Copyright 2011, 2012, 2013 Rogier van Dalen.

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
    // Forward declaration.
    template <class RangeTag, class Enable = void> struct default_direction;

    /**
    Base class for marking an operation as not implemented.
    Operations should be marked as unimplemented when their return type cannot
    be computed, and in other cases where it can be decided at compile-time
    that an operation is not be implemented.
    */
    struct unimplemented : ::callable_traits::unimplemented {
        typedef unimplemented type;

        // Make false_type depend on Arguments so the static assertion is
        // triggered only if operator() is actually called.
        template <class ... Arguments> struct false_type : rime::false_type {};

        template <class ... Arguments>
            int operator() (Arguments const & ...) const
        {
            static_assert (false_type <Arguments ...>::value,
                "Range operation not implemented");
            return 0;
        }
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
        result of the function applied to the last argument.
        */
        template <class SkipArguments, class Function> struct call_with_last;

        template <class ... SkipArguments, class Function>
            struct call_with_last <meta::vector <SkipArguments ...>, Function>
        {
            // Arguments are SkipArguments ..., LastArgument.
            // We would like to return the last argument.
            // CLang 3.0 confuses types when unpacking SkipArguments ... with
            // LastArgument && last_argument.
            // Workaround:
#if (BOOST_CLANG && __clang_major__ == 3 && __clang_minor__ == 0)
            template <class ... Arguments> struct result {
                typedef decltype (Function() (std::declval <
                    typename meta::first <meta::back,
                        meta::vector <Arguments ...>>::type>())) type;
            };

            template <class OnlyArgument>
                static typename result <OnlyArgument>::type
                last (OnlyArgument && only_argument)
            { return Function() (std::forward <OnlyArgument> (only_argument)); }

            template <class FirstArgument, class ... Arguments>
                static typename result <FirstArgument, Arguments ...>::type
                last (FirstArgument &&, Arguments && ... arguments)
            { return last (std::forward <Arguments> (arguments) ...); }

            template <class ... Arguments>
                typename result <Arguments ...>::type
                operator() (Arguments && ... arguments) const
            { return last (std::forward <Arguments> (arguments) ...); }
#else
            // Normal code.
            template <class LastArgument> auto
                operator() (SkipArguments const & ... skip_arguments,
                    LastArgument && last_argument) const
            RETURNS (Function() (std::forward <LastArgument> (last_argument)))
#endif
        };

    } // namespace helper

    namespace range_detail {

        /**
        Return whether the operation is implemented for the forward direction
        of Direction.
        */
        template <template <class, class, class> class Apply,
            class RangeTag, class Direction>
        struct is_implemented_forward
        : is_implemented <Apply <RangeTag, typename result_of <
            direction::callable::make_forward (Direction)>::type, void>> {};

        /**
        Perform operation Apply that takes a direction and a range with the
        reverse direction.
        \pre make_forward <Direction> is defined.
        */
        template <template <class, class, class> class Apply,
            class RangeTag, class Direction>
        struct forward_operation {
            Apply <RangeTag, typename
                result_of <direction::callable::make_forward (Direction)>::type,
                void> implementation;

            // Workaround for GCC 4.6.
            template <class Range> struct result {
                typedef decltype (implementation (
                    ::direction::make_forward (std::declval <
                        Direction const &>()), std::declval <Range>())) type;
            };

            template <class Range> typename result <Range>::type
                operator() (Direction const & direction, Range && range) const
            {
                return implementation (::direction::make_forward (direction),
                    std::forward <Range> (range));
            }
        };

    } // namespace range_detail

    /**
    Return the default direction for a range.

    If its argument is a range, this returns "front()" by default.
    However, it can be overridden.
    */
    template <class RangeTag, class Enable> struct default_direction {
        template <class Range> direction::front operator() (Range &&) const
        { return direction::front(); }
    };

    template<> struct default_direction <not_a_range_tag> : unimplemented {};

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

    template <class ... Arguments> struct default_direction;

    template <class Range> struct default_direction <Range>
    : operation::default_direction <typename tag_of <Range>::type> {};

} // namespace apply

/**** Functions that can actually be called. *****/

namespace callable {

    using ::callable_traits::generic;

    struct default_direction : generic <apply::default_direction> {};

} // namespace callable

/*
Function objects.
*/

static const auto default_direction = callable::default_direction();

static const direction::front front = {};
static const direction::back back = {};

namespace apply {
    /*
    For most operations, there is some automatic filling in of arguments
    to be done.
    They take zero or one (or sometimes more) directions, some variable
    parameters, and one (or sometimes more) ranges.
    If no direction is given, default_direction (range) is usually used.
    The variable parameters can sometimes be filled in automatically too.
    Below code deals with that.
    */

    namespace automatic_arguments {

        /**
        Categorise arguments into directions, other, and ranges.
        "type" is set to Apply <meta::vector <Directions ...>,
        meta::vector <Other...>, meta::vector <Ranges ...>, void>.
        The types in Directions are decayed.
        */
        template <class Arguments> struct categorise_arguments {
            typedef typename detail::split <
                    meta::front, is_direction <boost::mpl::_>, Arguments>::type
                directions_rest;

            typedef typename meta::as_vector <meta::transform <
                std::decay <boost::mpl::_>,
                typename directions_rest::first>>::type directions;

            typedef typename detail::split <
                    meta::back, is_range <boost::mpl::_>,
                    typename directions_rest::second
                >::type rest_ranges;

            // Here first and second are counted from the back!
            typedef typename rest_ranges::second other;
            typedef typename rest_ranges::first ranges;

            typedef meta::vector <directions, other, ranges> type;
        };

        /**
        Categorise Arguments into Directions, Other, and Ranges.
        Return Apply <Directions, Other, Ranges>.
        Except when Directions is empty.
        Then it takes the default direction of the first range and passes
        that as the first argument to
        Apply <meta::vector <direction>, Other, Ranges>.
        */
        template <template <class, class, class, class> class Apply,
            class Arguments, class Categorised =
                typename categorise_arguments <Arguments>::type>
        struct categorise_arguments_default_direction;

        // At least one direction.
        template <template <class, class, class, class> class Apply,
            class Arguments, class ... Directions, class Other, class Ranges>
        struct categorise_arguments_default_direction <Apply, Arguments,
            meta::vector <meta::vector <Directions ...>, Other, Ranges>>
        {
            typedef Apply <meta::vector <Directions ...>, Other, Ranges, void>
                type;
        };

        /**
        Actually call the implementation with the default direction.
        */
        template <class Implementation, class Other, class Ranges>
            struct prepend_default_direction;

        // No direction.
        template <template <class, class, class, class> class Apply,
            class Arguments, class Other, class Ranges>
        struct categorise_arguments_default_direction <Apply, Arguments,
            meta::vector <meta::vector<>, Other, Ranges>>
        {
            typedef typename meta::first <Ranges>::type range_type;

            typedef typename result_of_or <
                callable::default_direction (range_type), void>::type direction;
            typedef Apply <meta::vector <direction>, Other, Ranges, void>
                implementation;
            typedef typename boost::mpl::if_ <boost::mpl::and_ <
                        has <callable::default_direction (range_type)>,
                        operation::is_implemented <implementation>>,
                    prepend_default_direction <implementation, Other, Ranges>,
                    operation::unimplemented
                >::type type;
        };

        // No direction but no range either: a bit silly.
        // Needs disabling explicitly.
        template <template <class, class, class, class> class Apply,
            class Arguments, class Other>
        struct categorise_arguments_default_direction <Apply, Arguments,
            meta::vector <meta::vector<>, Other, meta::vector <>>>
        : operation::unimplemented {};

        /**
        \return The default direction of the first range.
        */
        template <class Range, class ... MoreRanges> inline auto
            first_default_direction (
                Range const & range, MoreRanges const & ...)
        RETURNS (::range::default_direction (range))

        // Explicit specialisations for different numbers of "Other" to keep
        // CLang 3.0 happy.
        template <class Implementation, class ... Ranges>
            struct prepend_default_direction <Implementation,
                meta::vector<>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Ranges> (ranges) ...))
        };

        template <class Implementation, class Other, class ... Ranges>
            struct prepend_default_direction <Implementation,
                meta::vector <Other>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Other && other, Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Other> (other),
                std::forward <Ranges> (ranges) ...))
        };

        template <class Implementation, class Other1, class Other2,
                class ... Ranges>
            struct prepend_default_direction <Implementation,
                meta::vector <Other1, Other2>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Other1 && other_1, Other2 && other_2,
                Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Other1> (other_1),
                std::forward <Other2> (other_2),
                std::forward <Ranges> (ranges) ...))
        };

        template <class Implementation, class Other1, class Other2,
            class Other3, class ... Ranges>
        struct prepend_default_direction <Implementation,
            meta::vector <Other1, Other2, Other3>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Other1 && other_1, Other2 && other_2,
                Other3 && other_3, Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Other1> (other_1),
                std::forward <Other2> (other_2),
                std::forward <Other3> (other_3),
                std::forward <Ranges> (ranges) ...))
        };

    } // namespace automatic_arguments

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_BASE_HPP_INCLUDED

