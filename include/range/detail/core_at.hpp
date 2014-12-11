/*
Copyright 2013 Rogier van Dalen.

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
Define the "at" operation, which returns the element at a specific position in a
range.
*/

#ifndef RANGE_DETAIL_CORE_AT_HPP_INCLUDED
#define RANGE_DETAIL_CORE_AT_HPP_INCLUDED

#include <utility>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include "meta/vector.hpp"

namespace range {

namespace operation {

    template <class RangeTag, class Direction, class Index, class Enable = void>
        struct at_constant;

    /**
    Convenience operation.
    Can be specialised if "at" is only available for indices known at compile
    time.
    */
    template <class RangeTag, class Direction, class Index, class Enable>
        struct at_constant
    : unimplemented
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Index const & index, Range && range) const;
    */};

    /**
    Return the element at \a Index in the range, starting from \a Direction.

    There is normally no need to specialise this: an implementation will
    automatically be synthesised using drop() and first().
    Sometimes, however, a better implementation can be provided; in that case,
    this should be specialised.

    To specialise this only for constant indices, specialise at_constant.

    \internal
    The automatic implementation is provided in namespace apply, because it
    is impossible to give it here.
    Given just a range tag and not the type of the actual range, it is not
    possible to find the return type of drop().
    That return type may have a different range tag (indeed, for heterogeneous
    ranges this is expected).
    It is therefore impossible to know whether first() is implemented for the
    range.
    The operation can be defined in namespace "apply", because there the range
    type is known.
    */
    template <class RangeTag, class Direction, class Index, class Enable>
        struct at
    : boost::mpl::if_ <rime::is_constant <Index>,
        at_constant <RangeTag, Direction, Index>, unimplemented
    >::type
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Index const & index, Range && range) const;
    */};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct at;
    template <std::size_t Index, class ... Arguments> struct at_c;
} // namespace apply

namespace callable {

    struct at : generic <apply::at> {};

    // It is not possible to use "generic <apply::at_c <Index, ?>>".
    template <std::size_t Index> struct at_c {
        template <class ... Arguments> struct apply
        : ::range::apply::at_c <Index, Arguments ...> {};

        template <class ... Arguments>
            auto operator() (Arguments && ... arguments) const
        RETURNS (apply <Arguments ...>() (
            std::forward <Arguments> (arguments) ...));
    };

    // Convenience definitions.
    struct second : at_c <1> {};
    struct third : at_c <2> {};
    struct fourth : at_c <3> {};
    struct fifth : at_c <4> {};
    struct sixth : at_c <5> {};
    struct seventh : at_c <6> {};
    struct eighth : at_c <7> {};
    struct ninth : at_c <8> {};
    struct tenth : at_c <9> {};

} // namespace callable

/**
Return the element at a specific index in a range.
This is equivalent to <c>first (direction, drop (direction, index, range))</c>,
and is only available if that is available.
\param direction (optional) The direction of traversal.
\param index The index of the desired element.
\param range The range within which the element is sought.
\sa at_c
*/
static const auto at = callable::at();

/**
Return the element at a specific index in a range.
The first template parameter (the index) must be given explicitly.
This is equivalent to at (direction, rime::size_t<Index>(), range).
*/
template <std::size_t Index, class Direction, class Range>
    inline auto at_c (Direction const & direction, Range && range)
RETURNS (callable::at_c <Index>() (direction, std::forward <Range> (range)));

/**
Return the element at a specific index in a range.
The first template parameter (the index) must be given explicitly.
This is equivalent to at (rime::size_t<Index>(), range).
*/
template <std::size_t Index, class Range>
    inline auto at_c (Range && range)
RETURNS (callable::at_c <Index>() (std::forward <Range> (range)));


/**
Return the element at the ... position in a range.
This is equivalent to <c>first (drop (direction, range))</c>.
It is also available for ranges that implement \c drop only with an increment of
one.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto second = callable::second();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<2> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto third = callable::third();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<3> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto fourth = callable::fourth();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<4> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto fifth = callable::fifth();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<5> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto sixth = callable::sixth();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<6> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto seventh = callable::seventh();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<7> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto eighth = callable::eighth();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<8> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto ninth = callable::ninth();

/**
Return the element at the ... position in a range.
This is equivalent to <c>at_c<9> (...)</c>.
\param direction (optional)
    The direction from which to count.
\param range
    The range from which the element is taken.
*/
static const auto tenth = callable::tenth();

namespace apply {

    /* at. */

    namespace detail {

        /**
        Forward to operation::at.
        */
        template <class Direction, class Index, class Range>
            struct at_specialised
        : operation::at <typename tag_of <Range>::type,
            Direction, typename std::decay <Index>::type> {};

        /**
        Synthesise an implementation for "at" that uses \c drop and \c first.
        If those are not available, then derive from operation::unimplemented.
        */
        template <class Direction, class Index, class Range,
            class Enable = void>
        struct at_synthesise
        : operation::unimplemented {};

        template <class Direction, class Index, class Range>
            struct at_synthesise <Direction, Index, Range, typename
                boost::enable_if <range::has <callable::first (Direction,
                    range::callable::drop (Direction, Index, Range))>>::type>
        {
            auto operator() (Direction const & direction,
                Index const & index, Range && range) const
            RETURNS (range::first (direction, range::drop (
                direction, index, std::forward <Range> (range))));
        };

        template <class Direction, class Index, class Range>
            struct at
        : boost::mpl::if_ <operation::is_implemented <
                at_specialised <Direction, Index, Range>>,
            at_specialised <Direction, Index, Range>,
            at_synthesise <Direction, Index, Range>
        >::type {};

    } // namespace detail

    namespace automatic_arguments {

        template <class Directions, class Indexes, class Ranges,
            class Enable = void>
        struct at : operation::unimplemented {};

        template <class Direction, class Index, class Range>
            struct at <meta::vector <Direction>, meta::vector <Index>,
                meta::vector <Range>>
        : apply::detail::at <Direction, Index, Range> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct at
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::at, meta::vector <Arguments ...>>::type {};

    /* at_c. */
    // This forwards to "at".
    // Since it has a template argument, the Direction argument must be handled
    // explicitly.

    namespace at_c_detail {

        // With direction.
        template <std::size_t Index, class Direction, class Range,
            class Underlying =
                apply::at <Direction, rime::size_t <Index>, Range>,
            class Enable = void>
        struct with_direction : operation::unimplemented {};

        template <std::size_t Index, class Direction, class Range,
            class Underlying>
        struct with_direction <Index, Direction, Range, Underlying, typename
            boost::enable_if <operation::is_implemented <Underlying>>::type>
        {
            auto operator() (Direction const & direction, Range && range) const
            RETURNS (Underlying() (direction, rime::size_t <Index>(),
                std::forward <Range> (range)));
        };

        // Without direction.
        template <std::size_t Index, class Range, class Underlying =
                apply::at <rime::size_t <Index>, Range>,
            class Enable = void>
        struct without_direction : operation::unimplemented {};

        template <std::size_t Index, class Range, class Underlying>
        struct without_direction <Index, Range, Underlying, typename
            boost::enable_if <operation::is_implemented <Underlying>>::type>
        {
            auto operator() (Range && range) const
            RETURNS (Underlying() (
                rime::size_t <Index>(), std::forward <Range> (range)));
        };

    } // namespace at_c_detail

    template <std::size_t Index, class ... Arguments> struct at_c
    : operation::unimplemented {};

    template <std::size_t Index, class Range> struct at_c <Index, Range>
    : at_c_detail::without_direction <Index, Range> {};

    template <std::size_t Index, class Direction, class Range>
        struct at_c <Index, Direction, Range>
    : at_c_detail::with_direction <Index, Direction, Range> {};

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_AT_HPP_INCLUDED
