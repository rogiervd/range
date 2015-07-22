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

#ifndef RANGE_EQUAL_HPP_INCLUDED
#define RANGE_EQUAL_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>

#include "rime/call_if.hpp"
#include "rime/always.hpp"

#include "core.hpp"

namespace range {

namespace equal_detail {

    template <class Direction, class Predicate> struct equal_default {
    private:
        // Homogeneous implementation.
        struct when_homogeneous {
            template <class Range1, class Range2> bool operator() (
                Range1 range1, Range2 range2,
                Direction const & direction, Predicate && predicate)
            {
                while (!range::empty (range1, direction)
                    && !range::empty (range2, direction))
                {
                    if (!predicate (range::first (range1, direction),
                            range::first (range2, direction)))
                        return false;
                    range1 = range::drop (range1);
                    range2 = range::drop (range2);
                }
                return range::empty (range1, direction)
                    == range::empty (range2, direction);
            }
        };

        struct when_empty;
        struct when_not_empty;
        // The dummy type parameter allows it to call this class itself.
        template <class Dummy> struct next;

        // Heterogeneous implementation: recursive.
        struct when_heterogeneous {
            template <class Range1, class Range2> auto operator() (
                Range1 && range1, Range2 && range2,
                Direction const & direction, Predicate && predicate) const
            // This is the run-time recursive implementation:
            /*{
                if (range::empty (range1, direction)
                    || range::empty (range2, direction))
                    return range::empty (range1, direction)
                        == range::empty (range2, direction);
                else {
                    if (!predicate (range::first (range1, direction),
                            range::first (range2, direction)))
                        return false;
                    else {
                        return (*this) (range::drop (range1, direction),
                            range::drop (range2, direction), direction);
                    }
                }
            }*/
            RETURNS (rime::call_if (rime::or_ (range::empty (range1, direction),
                range::empty (range2, direction)),
                when_empty(), when_not_empty(),
                std::forward <Range1> (range1),
                std::forward <Range2> (range2), direction,
                std::forward <Predicate> (predicate)));
        };

        struct when_empty {
            template <class Range1, class Range2> auto operator() (
                Range1 && range1, Range2 && range2,
                Direction const & direction, Predicate &&) const
            RETURNS (range::empty (range1, direction)
                == range::empty (range2, direction));
        };

        struct when_not_empty {
            template <class Range1, class Range2> auto operator() (
                Range1 && range1, Range2 && range2,
                Direction const & direction, Predicate && predicate) const
            RETURNS (rime::call_if (
                !predicate (range::first (range1, direction),
                    range::first (range2, direction)),
                rime::callable::always_default <rime::false_type>(),
                next <Range1>(),
                std::forward <Range1> (range1),
                std::forward <Range2> (range2),
                direction, std::forward <Predicate> (predicate)));
        };

        template <class Dummy> struct next {
            template <class Range1, class Range2> auto operator() (
                Range1 && range1, Range2 && range2,
                Direction const & direction, Predicate && predicate) const
            RETURNS (equal_default() (
                range::drop (std::forward <Range1> (range1), direction),
                range::drop (std::forward <Range2> (range2), direction),
                direction, std::forward <Predicate> (predicate)));
        };

    public:
        template <class Range1, class Range2>
        auto operator() (Range1 && range1, Range2 && range2,
            Direction const & direction, Predicate && predicate) const
        RETURNS (rime::call_if (rime::and_ (
                is_homogeneous <Range1, Direction>(),
                is_homogeneous <Range2, Direction>()),
            when_homogeneous(), when_heterogeneous(),
            std::forward <Range1> (range1),
            std::forward <Range2> (range2),
            direction, std::forward <Predicate> (predicate)));
    };

    struct element_equal {
        template <class Left, class Right>
        auto operator() (Left && left, Right && right) const
        RETURNS (std::forward <Left> (left) == std::forward <Right> (right));
    };

} // namespace equal_detail

namespace helper {

    /** \brief
    Compare two ranges for equality.

    \a implement_equal can be implemented to provide an implementation for a
    range type.
    This does not \em need to be implemented, because the generic implementation
    is fine, but it might be an optimisation.

    \param tag1 The tag of \a range1.
    \param tag2 The tag of \a range2.
    \param range1 The left-hand side range.
    \param range2 The right-hand side range.
    \param direction The direction.
    \param predicate The predicate used to compare the elements.
    */
    void implement_equal (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_equal;

        class equal {
            struct dispatch {
                template <class Range1, class Range2,
                    class Direction, class Predicate>
                auto operator() (Range1 && range1, Range2 && range2,
                    Direction const & direction, Predicate && predicate,
                    overload_order <1> *) const
                RETURNS (implement_equal (typename tag_of <Range1>::type(),
                    typename tag_of <Range2>::type(),
                    std::forward <Range1> (range1),
                    std::forward <Range2> (range2), direction,
                    std::forward <Predicate> (predicate)));

                template <class Range1, class Range2,
                    class Direction, class Predicate>
                auto operator() (Range1 && range1, Range2 && range2,
                    Direction const & direction, Predicate && predicate,
                    overload_order <2> *) const
                RETURNS (equal_detail::equal_default <Direction, Predicate>() (
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
            // Use element_equal.
            template <class Range1, class Range2, class Direction,
                class Enable = typename std::enable_if <
                    is_direction <Direction>::value>::type>
            auto operator() (Range1 && range1, Range2 && range2,
                Direction const & direction) const
            RETURNS (dispatch() (
                range::view (std::forward <Range1> (range1), direction),
                range::view (std::forward <Range2> (range2), direction),
                direction, equal_detail::element_equal(), pick_overload()));


            // Without direction, and without predicate.
            // Use the default direction of the first range and element_equal.
            template <class Range1, class Range2>
                auto operator() (Range1 && range1, Range2 && range2) const
            RETURNS (dispatch() (
                range::view (std::forward <Range1> (range1),
                    range::default_direction (range1)),
                range::view (std::forward <Range2> (range2),
                    range::default_direction (range1)),
                range::default_direction (range1),
                equal_detail::element_equal(), pick_overload()));
        };

    } // namespace implementation

    using implementation::equal;

} // namespace callable

/**
Compare two ranges for equality.

\return \c true iff the two arguments have equal length and all elements
compare equal.

\param range1
    The first range to compare.
\param range2
    The second range to compare.
\param direction
    (optional) Direction that should be used to traverse the ranges.
    If not given, the default direction of the first range is used.
\param predicate
    (optional) Predicate to use to compare individual elements.
    If not given, then \c operator== is used.
*/
static const auto equal = callable::equal();

} // namespace range

#endif // RANGE_EQUAL_HPP_INCLUDED
