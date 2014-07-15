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

#ifndef RANGE_LESS_LEXICOGRAPHICAL_HPP_INCLUDED
#define RANGE_LESS_LEXICOGRAPHICAL_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>

#include "rime/call_if.hpp"

#include "core.hpp"

namespace range {

namespace operation {

    template <class Range1Tag, class Range2Tag, class Direction, class Less,
        class Enable = void>
    struct less_lexicographical
    {
    private:
        // Homogeneous implementation.
        struct when_homogeneous {
            template <class Range1, class Range2>
            bool operator() (Direction const & direction, Less && less,
                Range1 range1, Range2 range2)
            {
                while (!range::empty (direction, range1)
                    && !range::empty (direction, range2))
                {
                    if (less (range::first (direction, range1),
                            range::first (direction, range2)))
                        return true;
                    if (less (range::first (direction, range2),
                            range::first (direction, range1)))
                        return false;
                    // Else continue to the next element.
                    range1 = range::drop (direction, std::move (range1));
                    range2 = range::drop (direction, std::move (range2));
                }
                return !range::empty (direction, range2);
            }
        };

        // The dummy type parameters enable freedom in the order of these.
        template <bool value, class Dummy> struct return_;
        template <class Dummy> struct when_range2_not_empty;
        template <class Dummy> struct when_neither_empty;
        template <class Dummy> struct when_range1_not_less;
        template <class Dummy> struct next;

        // Heterogeneous implementation: recursive.
        struct when_heterogeneous {
            template <class Range1, class Range2>
            auto operator() (Direction const & direction, Less && less,
                Range1 && range1, Range2 && range2) const
            // This is the run-time recursive implementation:
            /*{
                if (range::empty (direction, range2))
                    // Also if both are empty.
                    return false;
                else if (range::empty (direction, range1))
                    return true;
                else if (range::first (direction, range1)
                        < range::first (direction, range2))
                    return true;
                else if (range::first (direction, range2)
                        < range::first (direction, range1))
                    return false;
                else {
                    return (*this) (direction,
                        range::drop (direction, range1),
                        range::drop (direction, range2));
                }
            }*/
            RETURNS (rime::call_if (range::empty (direction, range2),
                return_ <false, Range1>(), when_range2_not_empty <Range1>(),
                direction, less, std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };


        template <bool value, class Dummy> struct return_ {
            template <class ... Arguments>
                rime::bool_ <value> operator() (Arguments const & ...) const
            { return rime::bool_ <value>(); }
        };

        template <class Dummy> struct when_range2_not_empty {
            template <class Range1, class Range2>
                auto operator() (Direction const & direction, Less && less,
                    Range1 && range1, Range2 && range2) const
            RETURNS (rime::call_if (range::empty (direction, range1),
                return_ <true, Dummy>(), when_neither_empty <Dummy>(),
                direction, less, std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };

        template <class Dummy> struct when_neither_empty {
            template <class Range1, class Range2>
                auto operator() (Direction const & direction, Less && less,
                    Range1 && range1, Range2 && range2) const
            RETURNS (rime::call_if (
                less (range::first (direction, range1),
                    range::first (direction, range2)),
                return_ <true, Dummy>(), when_range1_not_less <Dummy>(),
                direction, less, std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };

        template <class Dummy> struct when_range1_not_less {
            template <class Range1, class Range2>
                auto operator() (Direction const & direction, Less && less,
                    Range1 && range1, Range2 && range2) const
            RETURNS (rime::call_if (
                less (range::first (direction, range2),
                    range::first (direction, range1)),
                return_ <false, Dummy>(), next <Dummy>(),
                direction, less, std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };

        template <class Dummy> struct next {
            template <class Range1, class Range2>
                auto operator() (Direction const & direction, Less && less,
                    Range1 && range1, Range2 && range2) const
            RETURNS (less_lexicographical() (direction, less,
                range::drop (direction, std::forward <Range1> (range1)),
                range::drop (direction, std::forward <Range2> (range2))));
        };

    public:
        template <class Range1, class Range2>
        auto operator() (Direction const & direction, Less && less,
            Range1 && range1, Range2 && range2) const
        RETURNS (rime::call_if (rime::and_ (
                is_homogeneous <Direction, Range1>(),
                is_homogeneous <Direction, Range2>()),
            when_homogeneous(), when_heterogeneous(), direction, less,
            std::forward <Range1> (range1),
            std::forward <Range2> (range2)));
    };

} // namespace callable

namespace apply {
    template <class ... Arguments> struct less_lexicographical;
} // namespace apply

namespace callable {
    struct less_lexicographical : generic <apply::less_lexicographical> {};
} // namespace callable

/**
Compare two ranges lexicographically.

\return \c true iff the left-hand side is ordered before the right-hand side in
lexicographical ordering.

\param direction
    (optional) Direction that should be used to traverse the ranges.
\param less
    (optional) Predicate used for comparing individual elements.
\param range1
    The first range to compare.
\param range2
    The second range to compare.
*/
static const auto less_lexicographical = callable::less_lexicographical();

namespace apply {

    namespace automatic_arguments {

        namespace less_lexicographical_detail {

            struct less {
                template <class Left, class Right>
                auto operator() (Left && left, Right && right) const
                RETURNS (
                    std::forward <Left> (left) < std::forward <Right> (right));
            };

        } // namespace less_lexicographical_detail

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct less_lexicographical : operation::unimplemented {};

        template <class Direction, class Less, class Range1, class Range2>
            struct less_lexicographical <meta::vector <Direction>,
                meta::vector <Less>, meta::vector <Range1, Range2>>
        : operation::less_lexicographical <
            typename range::tag_of <Range1>::type,
            typename range::tag_of <Range2>::type, Direction, Less const &> {};

        template <class Direction, class Range1, class Range2>
            struct less_lexicographical <meta::vector <Direction>,
                meta::vector<>, meta::vector <Range1, Range2>>
        {
            typedef operation::less_lexicographical <
                typename range::tag_of <Range1>::type,
                typename range::tag_of <Range2>::type, Direction,
                less_lexicographical_detail::less const &>
                implementation;

            auto operator() (Direction const & direction,
                Range1 && range1, Range2 && range2) const
            RETURNS (implementation() (direction,
                less_lexicographical_detail::less(),
                std::forward <Range1> (range1),
                std::forward <Range2> (range2)));
        };

    } // namespace automatic_arguments

    template <class ... Arguments> struct less_lexicographical
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view <
            automatic_arguments::less_lexicographical>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif // RANGE_LESS_LEXICOGRAPHICAL_HPP_INCLUDED
