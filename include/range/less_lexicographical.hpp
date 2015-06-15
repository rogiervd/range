/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_LESS_LEXICOGRAPHICAL_HPP_INCLUDED
#define RANGE_LESS_LEXICOGRAPHICAL_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>

#include "rime/always.hpp"
#include "rime/core.hpp"
#include "rime/call_if.hpp"

#include "core.hpp"

namespace range {

namespace less_lexicographical_detail {

    template <class Direction, class Less>
        struct less_lexicographical_default
    {
    private:
        // Homogeneous implementation.
        struct when_homogeneous {
            template <class Range1, class Range2>
            bool operator() (Range1 range1, Range2 range2,
                Direction const & direction, Less && less)
            {
                while (!range::empty (range1, direction)
                    && !range::empty (range2, direction))
                {
                    if (less (range::first (range1, direction),
                            range::first (range2, direction)))
                        return true;
                    if (less (range::first (range2, direction),
                            range::first (range1, direction)))
                        return false;
                    // Else continue to the next element.
                    range1 = range::drop (std::move (range1), direction);
                    range2 = range::drop (std::move (range2), direction);
                }
                return !range::empty (range2, direction);
            }
        };

        struct when_range2_not_empty;
        struct when_neither_empty;
        struct when_range1_not_less;
        // The dummy type parameter allows it to call this class itself.
        template <class Dummy> struct next;

        // Heterogeneous implementation: recursive.
        struct when_heterogeneous {
            template <class Range1, class Range2>
            auto operator() (Range1 && range1, Range2 && range2,
                Direction const & direction, Less && less) const
            // This is the run-time recursive implementation:
            /*{
                if (range::empty (range2, direction))
                    // Also if both are empty.
                    return false;
                else if (range::empty (range1, direction))
                    return true;
                else if (range::first (range1, direction)
                        < range::first (range2, direction))
                    return true;
                else if (range::first (range2, direction)
                        < range::first (range1, direction))
                    return false;
                else {
                    return (*this) (range::drop (range1, direction),
                        range::drop (range2, direction), direction);
                }
            }*/
            RETURNS (rime::call_if (range::empty (range2, direction),
                rime::always <rime::false_type>(),
                when_range2_not_empty(),
                std::forward <Range1> (range1), std::forward <Range2> (range2),
                direction, std::forward <Less> (less)));
        };

        struct when_range2_not_empty {
            template <class Range1, class Range2> auto operator() (
                Range1 && range1, Range2 && range2,
                Direction const & direction, Less && less) const
            RETURNS (rime::call_if (range::empty (range1, direction),
                rime::always <rime::true_type>(), when_neither_empty(),
                std::forward <Range1> (range1), std::forward <Range2> (range2),
                direction, std::forward <Less> (less)));
        };

        struct when_neither_empty {
            template <class Range1, class Range2>
                auto operator() (Range1 && range1, Range2 && range2,
                    Direction const & direction, Less && less) const
            RETURNS (rime::call_if (
                less (range::first (range1, direction),
                    range::first (range2, direction)),
                rime::always <rime::true_type>(), when_range1_not_less(),
                std::forward <Range1> (range1), std::forward <Range2> (range2),
                direction, std::forward <Less> (less)));
        };

        struct when_range1_not_less {
            template <class Range1, class Range2>
                auto operator() (Range1 && range1, Range2 && range2,
                    Direction const & direction, Less && less) const
            RETURNS (rime::call_if (
                less (range::first (range2, direction),
                    range::first (range1, direction)),
                rime::always <rime::false_type>(), next <Range1>(),
                std::forward <Range1> (range1), std::forward <Range2> (range2),
                direction, std::forward <Less> (less)));
        };

        template <class Dummy> struct next {
            template <class Range1, class Range2>
                auto operator() (Range1 && range1, Range2 && range2,
                    Direction const & direction, Less && less) const
            RETURNS (less_lexicographical_default() (
                range::drop (std::forward <Range1> (range1), direction),
                range::drop (std::forward <Range2> (range2), direction),
                direction, std::forward <Less> (less)));
        };

    public:
        template <class Range1, class Range2>
        auto operator() (Range1 && range1, Range2 && range2,
            Direction const & direction, Less && less) const
        RETURNS (rime::call_if (rime::and_ (
                is_homogeneous <Range1, Direction>(),
                is_homogeneous <Range2, Direction>()),
            when_homogeneous(), when_heterogeneous(),
            std::forward <Range1> (range1), std::forward <Range2> (range2),
            direction, std::forward <Less> (less)));
    };

    struct less {
        template <class Left, class Right>
        auto operator() (Left && left, Right && right) const
        RETURNS (std::forward <Left> (left) < std::forward <Right> (right));
    };

} // namespace less_lexicographical_detail

namespace helper {

    /** \brief
    Compare two ranges lexicographically.

    \a implement_less_lexicographical can be implemented to provide an
    implementation for a range type.
    This does not \em need to be implemented, because the generic implementation
    is fine, but it might be an optimisation.

    \param tag1 The tag of \a range1.
    \param tag2 The tag of \a range2.
    \param range1 The left-hand side range.
    \param range2 The right-hand side range.
    \param direction The direction.
    \param predicate The predicate used to compare the elements.

    \todo Test
    */
    void implement_less_lexicographical (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_less_lexicographical;

        class less_lexicographical {
            struct dispatch {
                template <class Range1, class Range2,
                    class Direction, class Predicate>
                auto operator() (Range1 && range1, Range2 && range2,
                    Direction const & direction, Predicate && predicate,
                    overload_order <1> *) const
                RETURNS (implement_less_lexicographical (
                    typename tag_of <Range1>::type(),
                    typename tag_of <Range2>::type(),
                    std::forward <Range1> (range1),
                    std::forward <Range2> (range2), direction,
                    std::forward <Predicate> (predicate)));

                template <class Range1, class Range2,
                    class Direction, class Predicate>
                auto operator() (Range1 && range1, Range2 && range2,
                    Direction const & direction, Predicate && predicate,
                    overload_order <2> *) const
                RETURNS (
                    less_lexicographical_detail::less_lexicographical_default <
                        Direction, Predicate>() (
                            std::forward <Range1> (range1),
                            std::forward <Range2> (range2), direction,
                            std::forward <Predicate> (predicate)));
            };

        public:
            // With direction; with predicate.
            template <class Range1, class Range2,
                class Direction, class Predicate,
                class Enable = typename std::enable_if <
                    is_direction <Direction>::value>::type>
            auto operator() (Range1 && range1, Range2 && range2,
                Direction const & direction, Predicate && predicate) const
            RETURNS (dispatch() (
                range::view (std::forward <Range1> (range1), direction),
                range::view (std::forward <Range2> (range2), direction),
                direction, std::forward <Predicate> (predicate),
                pick_overload()));

            // Without direction; with predicate.
            // Use the default direction of the first range.
            template <class Range1, class Range2, class Predicate,
                class Enable = typename std::enable_if <
                    !is_direction <Predicate>::value>::type>
            auto operator() (Range1 && range1, Range2 && range2,
                Predicate && predicate) const
            RETURNS (dispatch() (
                range::view (std::forward <Range1> (range1),
                    range::default_direction (range1)),
                range::view (std::forward <Range2> (range2),
                    range::default_direction (range1)),
                range::default_direction (range1),
                std::forward <Predicate> (predicate), pick_overload()));

            // With direction; without predicate.
            // Use less().
            template <class Range1, class Range2, class Direction,
                class Enable = typename std::enable_if <
                    is_direction <Direction>::value>::type>
            auto operator() (Range1 && range1, Range2 && range2,
                Direction const & direction) const
            RETURNS (dispatch() (
                range::view (std::forward <Range1> (range1), direction),
                range::view (std::forward <Range2> (range2), direction),
                direction,
                less_lexicographical_detail::less(),
                pick_overload()));


            // Without direction, and without predicate.
            // Use the default direction of the first range and less.
            template <class Range1, class Range2>
                auto operator() (Range1 && range1, Range2 && range2) const
            RETURNS (dispatch() (
                range::view (std::forward <Range1> (range1),
                    range::default_direction (range1)),
                range::view (std::forward <Range2> (range2),
                    range::default_direction (range1)),
                range::default_direction (range1),
                less_lexicographical_detail::less(),
                pick_overload()));
        };

    } // namespace implementation

    using implementation::less_lexicographical;

} // namespace callable

/**
Compare two ranges lexicographically.

\return \c true iff the left-hand side is ordered before the right-hand side in
lexicographical ordering.

\param range1
    The first range to compare.
\param range2
    The second range to compare.
\param direction
    (optional) Direction that should be used to traverse the ranges.
    If not given, the default direction of the first range is used.
\param less
    (optional) Predicate used for comparing individual elements.
    If not given, then \c operator== is used.
*/
static const auto less_lexicographical = callable::less_lexicographical();

} // namespace range

#endif // RANGE_LESS_LEXICOGRAPHICAL_HPP_INCLUDED
