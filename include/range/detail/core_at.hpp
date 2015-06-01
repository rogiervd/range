/*
Copyright 2013-2015 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

    template <class RangeTag, class Direction, class Index, class Range,
            class Enable = void>
        struct at_constant;

    /**
    Convenience operation.
    Can be specialised if "at" is only available for indices known at compile
    time.
    */
    template <class RangeTag, class Direction, class Index, class Range,
            class Enable>
        struct at_constant
    : unimplemented
    {/*
        ... operator() (Direction const & direction,
            Index const & index, Range && range) const;
    */};

    template <class RangeTag, class Direction, class Index, class Range,
            class Enable = void>
        struct at_by_at_constant
    : unimplemented {};

    template <class RangeTag, class Direction, class Index, class Range>
        struct at_by_at_constant <RangeTag, Direction, Index, Range,
            typename boost::enable_if <rime::is_constant <Index>>::type>
    : at_constant <RangeTag, Direction, Index, Range> {};

    /**
    Synthesise an implementation for "at" that uses \c drop and \c first.
    If those are not available, then derive from operation::unimplemented.
    */
    template <class RangeTag, class Direction, class Index, class Range,
        class Enable = void>
    struct at_by_drop_first
    : operation::unimplemented {};

    template <class RangeTag, class Direction, class Index, class Range>
        struct at_by_drop_first <RangeTag, Direction, Index, Range, typename
            boost::enable_if <range::has <callable::first (Direction,
                range::callable::drop (Direction, Index, Range))>>::type>
    {
        auto operator() (Direction const & direction,
            Index const & index, Range && range) const
        RETURNS (range::first (direction, range::drop (
            direction, index, std::forward <Range> (range))));
    };

    /**
    Return the element at \a Index in the range, starting from \a Direction.

    There is normally no need to specialise this: an implementation will
    automatically be synthesised using drop() and first().
    This happens in \c at_automatic.
    If for some reason this needs to be switched off, then \c at_automatic
    can be implemented as deriving from \c unimplemented.

    Sometimes, however, a better implementation than the automatic one can be
    provided; in that case, this should be specialised.

    To specialise this only for constant indices, specialise at_constant.

    \tparam RangeTag The range tag.
    \tparam Direction The decayed direction type.
    \tparam Index The decayed type for the index.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Direction, class Index, class Range,
            class Enable>
        struct at
    : at_by_at_constant <RangeTag, Direction, Index, Range>
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Index const & index, Range && range) const;
    */};

    template <class RangeTag, class Direction, class Index, class Range>
        struct at_automatic
    : try_all <at <RangeTag, Direction, Index, Range>,
        at_by_drop_first <RangeTag, Direction, Index, Range>> {};

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
This is equivalent to
<c>first (direction, drop (direction, index, view_once (range)))</c>,
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
This is equivalent to <c>first (drop (direction, view_once (range)))</c>.
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

    namespace automatic_arguments {

        template <class Directions, class Indexes, class Ranges,
            class Enable = void>
        struct at : operation::unimplemented {};

        template <class Direction, class Index, class Range>
            struct at <meta::vector <Direction>, meta::vector <Index>,
                meta::vector <Range>>
        : operation::at_automatic <typename tag_of <Range>::type,
            typename std::decay <Direction>::type,
            typename std::decay <Index>::type, Range &&> {};

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
