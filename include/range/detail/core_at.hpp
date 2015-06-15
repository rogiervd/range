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

namespace helper {

    /** \brief
    Return the element at a specific position in a range.

    This should be equivalent to
    <c>first (drop (range, position, direction), direction)</c>.

    It is not usually necessary to implement this explicitly if first() and
    drop() are implemented.
    However, a more optimised implementation could be provided by member
    functions \c at and \c at_constant, and free functions implement_at()
    and implement_at_constant.

    If multiple of these are defined, then at_constant() is preferred over
    at(), and free functions over member functions.

    \param tag The range tag.
    \param range The range itself.
    \param position The position of the element to return.
    \param direction The direction to count the element from.
    */
    void implement_at (unusable);

    /** \brief
    Return the element at a specific position in a range, where \a index is a
    compile-time constant.

    \param tag The range tag.
    \param range The range itself.
    \param position
        The position of the element to return.
        This will be a compile-time constant.
    \param direction The direction to count the element from.
    */
    void implement_at_constant (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_at;
        using helper::implement_at_constant;

        struct at_dispatch {
            /* If position is a compile-time constant. */
            // Use free function.
            template <class Range, class Position, class Direction,
                class Enable = typename std::enable_if <
                    rime::is_constant <Position>::value>::type>
            auto operator() (Range && range, Position const & position,
                Direction const & direction, overload_order <1> *) const
            RETURNS (implement_at_constant (typename tag_of <Range>::type(),
                std::forward <Range> (range), position, direction));

            // Use member function.
            template <class Range, class Position, class Direction,
                class Enable = typename std::enable_if <
                    rime::is_constant <Position>::value>::type>
            auto operator() (Range && range, Position const & position,
                Direction const & direction, overload_order <2> *) const
            RETURNS (helper::member_access::at_constant (
                std::forward <Range> (range), position, direction));

            /* If position is anything. */
            // Use free function.
            template <class Range, class Position, class Direction>
            auto operator() (Range && range, Position const & position,
                Direction const & direction, overload_order <1> *) const
            RETURNS (implement_at (typename tag_of <Range>::type(),
                std::forward <Range> (range), position, direction));

            // Use member function.
            template <class Range, class Position, class Direction>
            auto operator() (Range && range, Position const & position,
                Direction const & direction, overload_order <2> *) const
            RETURNS (helper::member_access::at (
                std::forward <Range> (range), position, direction));

            /* Use drop and first. */
            template <class Range, class Position, class Direction>
            auto operator() (Range && range, Position const & position,
                Direction const & direction, overload_order <3> *) const
            RETURNS (first_direct() (
                drop_direct() (std::forward <Range> (range),
                    position, direction, pick_overload()),
                direction, pick_overload()));
        };

        struct at {
            // With direction.
            template <class Range, class Position, class Direction,
                class Enable = typename
                    std::enable_if <is_direction <Direction>::value>::type>
            auto operator() (Range && range, Position const & position,
                Direction const & direction)
                const
            RETURNS (at_dispatch() (std::forward <Range> (range), position,
                direction, pick_overload()));

            // Without direction: use default direction.
            template <class Range, class Position, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range && range, Position const & position) const
            RETURNS (at_dispatch() (std::forward <Range> (range), position,
                range::default_direction (range), pick_overload()));
        };

        template <std::size_t Position> struct at_c {
            // With direction.
            template <class Range, class Direction, class Enable = typename
                std::enable_if <is_direction <Direction>::value>::type>
            auto operator() (Range && range, Direction const & direction) const
            RETURNS (at_dispatch() (std::forward <Range> (range),
                rime::size_t <Position>(), direction, pick_overload()));

            // Without direction: use default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range && range) const
            RETURNS (at_dispatch() (std::forward <Range> (range),
                rime::size_t <Position>(), range::default_direction (range),
                pick_overload()));
        };

    } // namespace implementation

    using implementation::at;
    using implementation::at_c;

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


/** \brief
Return the element at a specific position in a range.

This is roughly equivalent to
<c>first (drop (range, position))</c>,
and is only available if that is available.
\param range The range within which the element is sought.
\param position The position of the desired element.
\param direction (optional) The direction of traversal.
\sa at_c
*/
static const auto at = callable::at();

/** \brief
Return the element at a specific index in a range.

The first template parameter (the index) must be given explicitly.
This is roughly equivalent to <c>at (range, rime::size_t<Index>())</c>.

\tparam Position The position of the desired element.

\param range The range within which the element is sought.
\param direction (optional) The direction of traversal.
*/
template <std::size_t Position, class Range, class Direction>
    inline auto at_c (Range && range, Direction const & direction)
RETURNS (callable::at_c <Position>() (std::forward <Range> (range), direction));

/// \cond DONT_DOCUMENT
template <std::size_t Position, class Range>
    inline auto at_c (Range && range)
RETURNS (callable::at_c <Position>() (std::forward <Range> (range)));
/// \endcond

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

} // namespace range

#endif  // RANGE_DETAIL_CORE_AT_HPP_INCLUDED
