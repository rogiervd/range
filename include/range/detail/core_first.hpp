/*
Copyright 2011-2014 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED
#define RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {

    namespace first_detail {

        template <class RangeTag, class Direction> struct forward_to_chop {
            template <class Range> auto
                operator() (Direction const & direction, Range && range) const
            RETURNS (chop <RangeTag, Direction>() (
                direction, std::forward <Range> (range)).forward_first());
        };

    } // namespace first_detail

    /**
    Return the first element in the range.

    If operation::chop is implemented, this is automatically implemented in
    terms of it.
    */
    template <class RangeTag, class Direction, class Enable> struct first
    : boost::mpl::if_ <is_implemented <chop <RangeTag, Direction>>,
        first_detail::forward_to_chop <RangeTag, Direction>, unimplemented
    >::type {/*
        template <class Range>
            ... operator() (Direction const & direction, Range && range) const;
    */};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct first;
} // namespace apply

namespace callable {
    struct first : generic <apply::first> {};
} // namespace callable

static const auto first = callable::first();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct first : operation::unimplemented {};

        template <class Direction, class Range>
            struct first <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::first <typename range::tag_of <Range>::type, Direction> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct first
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::first, meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED

