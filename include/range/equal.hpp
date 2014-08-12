/*
Copyright 2014 Rogier van Dalen.

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

#ifndef RANGE_EQUAL_HPP_INCLUDED
#define RANGE_EQUAL_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>

#include "rime/call_if.hpp"

#include "core.hpp"

namespace range {

namespace operation {

    template <class Range1Tag, class Range2Tag, class Direction,
        class Predicate, class Enable = void>
    struct equal
    {
    private:
        // Homogeneous implementation.
        struct when_homogeneous {
            template <class Range1, class Range2> bool operator() (
                Direction const & direction, Predicate && predicate,
                Range1 range1, Range2 range2)
            {
                while (!range::empty (direction, range1)
                    && !range::empty (direction, range2))
                {
                    if (!predicate (range::first (direction, range1),
                            range::first (direction, range2)))
                        return false;
                    range1 = range::drop (range1);
                    range2 = range::drop (range2);
                }
                return range::empty (direction, range1)
                    == range::empty (direction, range2);
            }
        };

        // The dummy type parameters enable freedom in the order of these.
        template <class Dummy> struct when_empty;
        template <class Dummy> struct when_not_empty;
        template <class Dummy> struct next;

        // Heterogeneous implementation: recursive.
        struct when_heterogeneous {
            template <class Range1, class Range2> auto operator() (
                Direction const & direction, Predicate && predicate,
                Range1 && range1, Range2 && range2) const
            // This is the run-time recursive implementation:
            /*{
                if (range::empty (direction, range1)
                    || range::empty (direction, range2))
                    return range::empty (direction, range1)
                        == range::empty (direction, range2);
                else {
                    if (!predicate (range::first (direction, range1),
                            range::first (direction, range2)))
                        return false;
                    else {
                        return (*this) (direction,
                            range::drop (direction, range1),
                            range::drop (direction, range2));
                    }
                }
            }*/
            RETURNS (rime::call_if (rime::or_ (range::empty (direction, range1),
                range::empty (direction, range2)),
                when_empty <Range1>(), when_not_empty <Range1>(),
                direction, predicate,
                std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };

        template <class Dummy> struct when_empty {
            template <class Range1, class Range2> auto operator() (
                Direction const & direction, Predicate && predicate,
                Range1 && range1, Range2 && range2) const
            RETURNS (range::empty (direction, range1)
                == range::empty (direction, range2));
        };

        struct return_false {
            template <class ... Arguments>
                rime::false_type operator() (Arguments const & ...) const
            { return rime::false_; }
        };

        template <class Dummy> struct when_not_empty {
            template <class Range1, class Range2> auto operator() (
                Direction const & direction, Predicate && predicate,
                Range1 && range1, Range2 && range2) const
            RETURNS (rime::call_if (
                !predicate (range::first (direction, range1),
                    range::first (direction, range2)),
                return_false(), next <Range1>(), direction, predicate,
                std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };

        template <class Dummy> struct next {
            template <class Range1, class Range2> auto operator() (
                Direction const & direction, Predicate && predicate,
                Range1 && range1, Range2 && range2) const
            RETURNS (equal() (direction, predicate,
                range::drop (direction, std::forward <Range1> (range1)),
                range::drop (direction, std::forward <Range2> (range2))));
        };

    public:
        template <class Range1, class Range2>
        auto operator() (Direction const & direction, Predicate && predicate,
            Range1 && range1, Range2 && range2) const
        RETURNS (rime::call_if (rime::and_ (
                is_homogeneous <Direction, Range1>(),
                is_homogeneous <Direction, Range2>()),
            when_homogeneous(), when_heterogeneous(), direction, predicate,
            std::forward <Range1> (range1),
            std::forward <Range2> (range2)));
    };

} // namespace operation

namespace apply {
    template <class ... Arguments> struct equal;
} // namespace apply

namespace callable {
    struct equal : generic <apply::equal> {};
} // namespace callable

/**
Compare two ranges for equality.

\return \c true iff the two arguments have equal length and all elements
compare equal.

\param direction
    (optional) Direction that should be used to traverse the ranges.
    If not given, the default direction of the first range is used.
\param predicate
    (optional) Predicate to use to compare individual elements.
    If not given, then \c operator== is used.
\param range1
    The first range to compare.
\param range2
    The second range to compare.
*/
static const auto equal = callable::equal();

namespace apply {

    namespace automatic_arguments {

        namespace equal_detail {

            struct element_equal {
                template <class Left, class Right>
                auto operator() (Left && left, Right && right) const
                RETURNS (std::forward <Left> (left)
                    == std::forward <Right> (right));
            };

        } // namespace equal_detail

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct equal : operation::unimplemented {};

        template <class Direction, class Predicate, class Range1, class Range2>
            struct equal <meta::vector <Direction>, meta::vector <Predicate>,
                meta::vector <Range1, Range2>>
        : operation::equal <typename range::tag_of <Range1>::type,
            typename range::tag_of <Range2>::type, Direction, Predicate const &>
        {};

        template <class Direction, class Range1, class Range2>
            struct equal <meta::vector <Direction>,
                meta::vector<>, meta::vector <Range1, Range2>>
        {
            typedef operation::equal <
                typename range::tag_of <Range1>::type,
                typename range::tag_of <Range2>::type, Direction,
                equal_detail::element_equal const &>
                implementation;

            auto operator() (Direction const & direction,
                Range1 && range1, Range2 && range2) const
            RETURNS (implementation() (direction,
                equal_detail::element_equal(),
                std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };
    } // namespace automatic_arguments

    template <class ... Arguments> struct equal
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view <automatic_arguments::equal>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif // RANGE_EQUAL_HPP_INCLUDED
