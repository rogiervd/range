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

#ifndef RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED
#define RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {

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
            has <direction::callable::make_forward (Direction)>,
            range_detail::is_implemented_forward <size, RangeTag, Direction>
        >,
        range_detail::forward_operation <size, RangeTag, Direction>,
        unimplemented
    >::type {/*
        template <class Range>
            ... operator() (Direction const & direction, Range && range) const;
    */};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct size;
} // namespace apply

namespace callable {
    struct size : generic <apply::size> {};
} // namespace callable

static const auto size = callable::size();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct size : operation::unimplemented {};

        template <class Direction, class Range>
            struct size <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::size <typename range::tag_of <Range>::type, Direction> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct size
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::size, meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED

