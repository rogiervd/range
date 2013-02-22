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

#ifndef RANGE_CORE_HPP_INCLUDED
#define RANGE_CORE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/placeholders.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/all.hpp"
#include "meta/transform.hpp"

#include "rime/core.hpp"

#include "direction.hpp"

#include "utility/returns.hpp"
#include "detail/meta_split.hpp"

namespace range {

struct not_a_range_tag;

/**
Ranges use tag dispatching for operations.
Tags should contain enough information to decide whether an operation can
proceed, generally to the point where the return type can be computed.
(For example, the return type of "drop" on a known-empty range does not exist.)

Normally, ranges should specialise tag_of_bare to specify a tag.
However, this is just a convenience mechanism for tag_of.
If it depends on, for example, const specification, whether operations compile,
then specialise tag_of.
*/
template <class Range, class Enable = void>
    struct tag_of_bare { typedef not_a_range_tag type; };

template <class Range> struct tag_of
: tag_of_bare <typename std::decay <Range>::type> {};

template <class Range> struct is_range
: boost::mpl::not_ <std::is_same <
    typename tag_of <Range>::type, not_a_range_tag>> {};

/**** Range operations *****/

/**
Implementation of operations on ranges.
Types in this namespace must be specialised to implement range operations.
This is (hopefully) as simple as possible.
*/
namespace operation {
    // Forward declarations
    template <class RangeTag> struct default_direction;

    template <class RangeTag, class Directions, class Enable = void>
        struct make_view;

    template <class RangeTag, class Directions, class Enable = void>
        struct view;

    template <class RangeTag, class Direction, class Enable = void>
        struct empty;

    template <class RangeTag, class Direction, class Enable = void>
        struct size;

    template <class RangeTag, class Direction, class Enable = void>
        struct first;

    template <class RangeTag, class Direction, class Increment,
        class Enable = void>
    struct drop;

    template <class RangeTag, class Direction, class Function, class State,
        class Enable = void>
    struct fold;

    /**
    Base class for marking an operation as not implemented.
    Operations should be marked as unimplemented when their return type cannot
    be computed, and in other cases where it can be decided at compile-time
    that an operation is not be implemented.
    */
    struct unimplemented {
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

    template <class Operation> struct is_implemented
    : boost::mpl::not_ <std::is_base_of <unimplemented, Operation> > {};

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

    /**
    Return the default direction for a range.

    If its argument is a range, this returns "front()".
    */
    template <class RangeTag> struct default_direction {
        template <class Range> direction::front operator() (Range &&) const
        { return direction::front(); }
    };

    template<> struct default_direction <not_a_range_tag> : unimplemented {};

    /**
    Produce a lightweight range, a "view", on a heavyweight range such as a
    container.

    Directions is a meta::vector<> of at least one direction.
    The resulting type must be assignable without changing any underlying
    container.
    If possible, the resulting type should be homogeneous.
    make_view must not be defined for the resulting type.
    If the range is already a lightweight range, then make_view should not be
    defined for it.
    view (directions..., range) will then return range unchanged.

    Directions is a meta::vector<> of decayed types.
    The range is forwarded as is.
    */
    template <class RangeTag, class Directions, class Enable>
        struct make_view : unimplemented {};

    namespace detail {
        /**
        Default implementation for "view".
        If the directions are all valid for the range (the operationalisation of
        which is: if empty (direction, range) is defined for all directions),
        then the range is returned unchanged.
        */
        template <class RangeTag, class Directions, class Enable = void>
            struct passthrough_view : unimplemented {};

        template <class RangeTag, class Directions>
            struct passthrough_view <RangeTag, Directions,
                typename boost::enable_if <
                    meta::all <typename meta::transform <boost::mpl::and_ <
                        is_direction <boost::mpl::_1>,
                        is_implemented <operation::empty <
                            RangeTag, boost::mpl::_1>>>,
                    Directions>::type>>::type>
        : helper::call_with_last <Directions, helper::return_rvalue_reference>
        {};
    } // namespace detail

    template <class RangeTag, class Directions, class Enable> struct view
    : boost::mpl::if_ <
        is_implemented <make_view <RangeTag, Directions>>,
        make_view <RangeTag, Directions>,
        detail::passthrough_view <RangeTag, Directions>>::type {};

    namespace range_detail {

        /**
        Return whether the operation is implemented for the forward direction
        of Direction.
        */
        template <template <class, class, class> class Apply,
            class RangeTag, class Direction>
        struct is_implemented_forward
        : is_implemented <Apply <RangeTag, typename
            direction::result_of::make_forward <Direction>::type, void>> {};

        /**
        Perform operation Apply that takes a direction and a range with the
        reverse direction.
        \pre make_forward <Direction> is defined.
        */
        template <template <class, class, class> class Apply,
            class RangeTag, class Direction>
        struct forward_operation {
            Apply <RangeTag,
                typename direction::result_of::make_forward <Direction>::type,
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
    Return true if there are no elements in the range.

    This only needs to be defined for the forward direction, because under the
    following conditions "empty (make_forward (direction), range)" is called:
    \li make_forward (direction) is defined.
    \li empty (make_forward (direction), range) is defined.
    An earlier version used to check whether "first (direction, range)" is
    defined as a proxy for whether "direction" is a sensible direction for
    range.
    However, then compile-time empty ranges do not work.

    Direction is a decayed type.
    The range is forwarded as is.
    */
    template <class RangeTag, class Direction, class Enable>
        struct empty
    // Forward to the forward direction if it is available.
    : boost::mpl::if_ <
        boost::mpl::and_ <
            ::direction::has::make_forward <Direction>,
            range_detail::is_implemented_forward <empty, RangeTag, Direction>
        >,
        range_detail::forward_operation <empty, RangeTag, Direction>,
        unimplemented
    >::type {/*
        template <class Range>
            ... operator() (Direction const & direction, Range && range) const;
    */};

    /**
    Return the number of elements in the range.

    This only needs to be defined for the forward direction, because under the
    following conditions "size (make_forward (direction), range)" is called:
    \li make_forward (direction) is defined.
    \li size (make_forward (direction), range) is defined.

    Direction is a decayed type.
    The range is forwarded as is.
    */
    template <class RangeTag, class Direction, class Enable>
        struct size
    // Forward to the forward direction if it is available.
    : boost::mpl::if_ <
        boost::mpl::and_ <
            ::direction::has::make_forward <Direction>,
            range_detail::is_implemented_forward <size, RangeTag, Direction>
        >,
        range_detail::forward_operation <size, RangeTag, Direction>,
        unimplemented
    >::type {/*
        template <class Range>
            ... operator() (Direction const & direction, Range && range) const;
    */};

    /**
    Return the first element in the range.
    */
    template <class RangeTag, class Direction, class Enable>
    struct first : unimplemented {/*
        template <class Range>
            ... operator() (Direction const & direction, Range && range) const;
    */};

    typedef rime::size_t <1> one_type;

    /**
    Convenience operation.
    Can be specialised if "drop" is only available for an increment of one.
    */
    template <class RangeTag, class Direction, class Enable = void>
    struct drop_one : unimplemented
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Increment const & increment, Range && range) const;
    */};

    /**
    Convenience operation.
    Can be specialised if "drop" is only available for increments known at
    compile time.
    */
    template <class RangeTag, class Direction, class Increment,
        class Enable = void>
    struct drop_constant
    : boost::mpl::if_ <rime::equal_constant <Increment, one_type>,
        drop_one <RangeTag, Direction>, unimplemented
    >::type
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Increment const & increment, Range && range) const;
    */};

    /**
    Return the range without the first "Increment" elements from the direction
    "DirectionTag".
    The return type and value should be the same as the return type and value
    of "Increment" calls to drop <DirectionTag, one_type>.
    */
    template <class RangeTag, class Direction, class Increment,
        class Enable>
    struct drop
    : boost::mpl::if_ <rime::is_constant <Increment>,
        drop_constant <RangeTag, Direction, Increment>, unimplemented
    >::type
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Increment const & increment, Range && range) const;
    */};

    /**
    Return the result of a "fold" operation, a general form of iteration over
    the range.
    Generic "fold" functionality is based on drop and first, and  implemented in
    "fold.hpp".
    This is a hook that can be specialised for a particular type of range,
    which can be more efficient.
    */
    template <class RangeTag, class Direction, class Function, class State,
        class Enable>
    struct fold;
    /*{
        template <class Range>
            ... operator() (Direction const &,
                Function &&, State && state, Range && range) const;
    };*/

} // namespace operation

/**
Namespace with structures that normalise the parameters for the structures
in the namespace "operation".
They take as class template parameters the arguments that the functors (in
callable) take, and remove cv-qualifications from  directions and increments.

The classes are defined for all template parameters, and are implemented
(i.e. do not derive from operation::unimplemented, and have an operator()) only
for the relevant ones.
*/
namespace apply {
    /*
    The implementations are given below; they need the types in has:: and
    result_of:: to be declared.
    To allow the classes in callable:: to be implemented easily, with
    template template parameters, the number of template parameters is 4,
    even if it could be less.
    */

    template <class ... Arguments> struct default_direction;
    template <class ... Arguments> struct view;
    template <class ... Arguments> struct empty;
    template <class ... Arguments> struct size;
    template <class ... Arguments> struct first;
    template <class ... Arguments> struct drop;

} // namespace apply

/*
Namespace with structures indicating whether a type of range supports an
operation.
For completeness, they are also implemented for silly parameters, for example,
when no parameter is a range.
*/
namespace has {
    // default_direction
    template <class ... Arguments> struct default_direction
    : operation::is_implemented <apply::default_direction <Arguments ...>> {};

    // view
    template <class ... Arguments> struct view
    : operation::is_implemented <apply::view <Arguments ...>> {};

    // empty
    template <class ... Arguments> struct empty
    : operation::is_implemented <apply::empty <Arguments ...>> {};

    // size
    template <class ... Arguments> struct size
    : operation::is_implemented <apply::size <Arguments ...>> {};

    // first
    template <class ... Arguments> struct first
    : operation::is_implemented <apply::first <Arguments ...>> {};

    // drop
    template <class ... Arguments> struct drop
    : operation::is_implemented <apply::drop <Arguments ...>> {};

} // namespace has

namespace result_of {
    namespace detail {

        template <bool decay, template <class ...> class Apply, class Arguments>
            struct compute_result_exists;

        // Compute and then decay.
        template <template <class ...> class Apply, class Arguments>
        struct compute_result_exists <true, Apply, Arguments>
        : std::decay <typename compute_result_exists <false,
            Apply, Arguments>::type> {};

        // Compute.
        template <template <class ...> class Apply, class ... Arguments>
        struct compute_result_exists <
            false, Apply, meta::vector <Arguments ...>>
        {
            typedef decltype (Apply <Arguments ...>() (
                std::declval <Arguments>()...)) type;
        };

        template <bool decay, template <class ...> class Apply, class Arguments,
            class Enable = void>
        struct compute_result;

        template <bool decay, template <class ...> class Apply,
            class ... Arguments>
        struct compute_result <
            decay, Apply, meta::vector <Arguments...>,
            typename boost::enable_if <
                operation::is_implemented <Apply <Arguments...>>>::type>
        : compute_result_exists <
            decay, Apply, meta::vector <Arguments ...>> {};

    } // namespace detail

    /**
    \return The decayed return type from default_direction.
    */
    template <class ... Arguments> struct default_direction
    : detail::compute_result <
        true, apply::default_direction, meta::vector <Arguments ...>> {};

    template <class ... Arguments> struct view
    : detail::compute_result <false, apply::view, meta::vector <Arguments...>>
    {};

    template <class ... Arguments> struct empty
    : detail::compute_result <false, apply::empty, meta::vector <Arguments ...>>
    {};

    template <class ... Arguments> struct size
    : detail::compute_result <false, apply::size, meta::vector <Arguments ...>>
    {};

    template <class ... Arguments> struct first
    : detail::compute_result <false, apply::first, meta::vector <Arguments ...>>
    {};

    template <class ... Arguments> struct drop
    : detail::compute_result <false, apply::drop, meta::vector <Arguments ...>>
    {};

} // namespace result_of

/**** Functions that can actually be called. *****/

namespace callable {

    namespace detail {
        template <template <class ...> class Apply>
            struct generic
        {
            template <class ... Arguments>
                auto operator() (Arguments && ... arguments) const
            RETURNS (Apply <Arguments ...>() (
                std::forward <Arguments> (arguments) ...))
        };
    } // namespace detail

    struct default_direction : detail::generic <apply::default_direction> {};
    struct empty : detail::generic <apply::empty> {};
    struct size : detail::generic <apply::size> {};
    struct first : detail::generic <apply::first> {};
    struct drop : detail::generic <apply::drop> {};
    struct view : detail::generic <apply::view> {};

} // namespace callable

/*
Function objects.
*/

static const auto default_direction = callable::default_direction();
static const auto view = callable::view();
static const auto empty = callable::empty();
static const auto size = callable::size();
static const auto first = callable::first();
static const auto drop = callable::drop();

static const direction::front front = {};
static const direction::back back = {};

namespace apply {

    /** default_direction */
    template <class Range> struct default_direction <Range>
    : operation::default_direction <typename tag_of <Range>::type> {};

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

            typedef typename meta::transform <std::decay <boost::mpl::_>,
                typename directions_rest::first>::type directions;

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
        Return result_of::default_direction.
        If default_direction does not exist, this returns void, instead of a
        compiler error.
        */
        template <class Range> struct result_of_default_direction
        : boost::mpl::eval_if <has::default_direction <Range>,
            result_of::default_direction <Range>,
            boost::mpl::identity <void>> {};

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

            typedef typename result_of_default_direction <range_type>::type
                direction;
            typedef Apply <meta::vector <direction>, Other, Ranges, void>
                implementation;
            typedef typename boost::mpl::if_ <boost::mpl::and_ <
                        has::default_direction <range_type>,
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

        // Explicit specialisations to keep compilers happy.
        template <class Implementation, class Range>
            struct prepend_default_direction <Implementation,
                meta::vector<>, meta::vector <Range>>
        {
            Implementation implementation;

            auto operator() (Range && range) const
            RETURNS (implementation (range::default_direction (range),
                std::forward <Range> (range)))
        };

        template <class Implementation, class Other, class Range>
            struct prepend_default_direction <Implementation,
                meta::vector <Other>, meta::vector <Range>>
        {
            Implementation implementation;

            auto operator() (Other && other, Range && range) const
            RETURNS (implementation (range::default_direction (range),
                std::forward <Other> (other), std::forward <Range> (range)))
        };

    } // namespace automatic_arguments

    /* Classes that actually forward to operations::. */
    namespace automatic_arguments {

        // view.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct view : operation::unimplemented {};

        template <class Directions, class Range>
            struct view <Directions, meta::vector<>, meta::vector <Range>>
        : operation::view <typename range::tag_of <Range>::type, Directions> {};

        // empty.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct empty : operation::unimplemented {};

        template <class Direction, class Range>
            struct empty <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::empty <typename range::tag_of <Range>::type, Direction> {};

        // size.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct size : operation::unimplemented {};

        template <class Direction, class Range>
            struct size <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::size <typename range::tag_of <Range>::type, Direction> {};

        // first.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct first : operation::unimplemented {};

        template <class Direction, class Range>
            struct first <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::first <typename range::tag_of <Range>::type, Direction> {};

        // drop.
        template <class Directions, class Increments, class Ranges,
            class Enable = void>
        struct drop : operation::unimplemented {};

        // All arguments filled in: forward to operation::drop.
        template <class Direction, class Increment, class Range>
            struct drop <meta::vector <Direction>,
                meta::vector <Increment>, meta::vector <Range>>
        : operation::drop <typename tag_of <Range>::type,
            Direction, typename std::decay <Increment>::type> {};

        // No Increment: insert one_type().
        using operation::one_type;

        template <class Direction, class Range>
            struct drop <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>, typename boost::enable_if <
                    operation::is_implemented <drop <meta::vector <Direction>,
                        meta::vector <one_type>, meta::vector <Range>>>
                >::type>
        {
            drop <meta::vector <Direction>, meta::vector <one_type>,
                meta::vector <Range>> implementation;

            auto operator() (Direction const & direction, Range && range) const
            RETURNS (implementation (
                direction, one_type(), std::forward <Range> (range)))
        };

    } // namespace automatic_arguments

    /** view */
    template <class ... Arguments> struct view
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::view, meta::vector <Arguments ...>>::type {};

    /** empty */
    template <class ... Arguments> struct empty
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::empty, meta::vector <Arguments ...>>::type {};

    /** size */
    template <class ... Arguments> struct size
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::size, meta::vector <Arguments ...>>::type {};

    /** first */
    template <class ... Arguments> struct first
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::first, meta::vector <Arguments ...>>::type {};

    /** drop */
    template <class ... Arguments> struct drop
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::drop, meta::vector <Arguments ...>>::type {};

} // namespace apply

/* is_view */

/**
Metafunction that returns true iff Range is a view.
That is, view (directions..., range) returns range itself.

The last argument is Range.
The arguments before it form Directions.
If only one argument is given, it is Range, and its default direction is used.
*/
template <class ... Arguments> struct is_view
: rime::as_rime_constant <typename std::is_same <
        typename std::decay <
            // Last argument is the range.
            typename meta::first <meta::back, meta::vector <Arguments...>>::type
            >::type,
        typename std::decay <typename result_of::view <Arguments...>::type
            >::type
    >::type>::type {};

/* is_homogeneous */

namespace detail {

    /**
    Return true iff Range is homogeneous in Direction.
    \pre bump <Direction, Range> exists.
    */
    template <class Direction, class Range> struct is_homogeneous_in_impl
    : boost::is_same <
        typename std::decay <Range>::type,
        typename std::decay <typename result_of::drop <Direction, Range>::type
            >::type
    > {};

    template <class Direction, class Range> struct is_homogeneous_in
    : boost::mpl::and_ <has::drop <Direction, Range>,
        is_homogeneous_in_impl <Direction, Range>> {};

    /**
    Metafunction that returns true iff the range, the last argument, is
    homogeneous in all directions.
    */
    template <class Directions, class Range> struct is_homogeneous;

    template <class ... Directions, class Range>
        struct is_homogeneous <meta::vector <Directions ...>, Range>
    : meta::all <meta::vector <is_homogeneous_in <Directions, Range>...>> {};

} // namespace detail

/**
Metafunction that returns true iff drop (Direction, Range) returns a Range
for all Directions.
This means, for example, that a recursive iteration can be written as a loop.
Since applying "drop" with an increment is equivalent to applying "drop" a
number of times, a homogeneous range cannot become heterogeneous.

The last argument is Range.
The arguments before it form Directions.
If only one argument is given, it is Range, and its default direction is used.
*/
template <class ... Arguments> struct is_homogeneous;

template <class ... Arguments> struct is_homogeneous
: rime::as_rime_constant <
    detail::is_homogeneous <
        typename meta::drop <meta::back, meta::vector <Arguments ...>>::type,
        typename meta::first <meta::back, meta::vector <Arguments ...>>::type>
>::type {};

template <class Range> struct is_homogeneous <Range>
: is_homogeneous <typename result_of::default_direction <Range>::type, Range>
{};

} // namespace range

#endif  // RANGE_CORE_HPP_INCLUDED

