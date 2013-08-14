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

#ifndef RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED
#define RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

namespace range {

namespace operation {

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
    template <class RangeTag, class Direction, class Enable = void>
        struct empty    // Forward to the forward direction if it is available.
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

} // namespace operation

namespace apply {
    template <class ... Arguments> struct empty;
} // namespace apply

namespace has {
    template <class ... Arguments> struct empty
    : operation::is_implemented <apply::empty <Arguments ...>> {};
} // namespace has

namespace result_of {
    template <class ... Arguments> struct empty
    : detail::compute_result <false, apply::empty, meta::vector <Arguments ...>>
    {};
} // namespace result_of

namespace callable {
    struct empty : detail::generic <apply::empty> {};
} // namespace callable

static const auto empty = callable::empty();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct empty : operation::unimplemented {};

        template <class Direction, class Range>
            struct empty <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::empty <typename range::tag_of <Range>::type, Direction> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct empty
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::empty, meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED

