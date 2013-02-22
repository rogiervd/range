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

#ifndef RANGE_WALK_SIZE_HPP_INCLUDED
#define RANGE_WALK_SIZE_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include "core.hpp"

namespace range {

namespace callable {

    struct walk_size {
        template <class Direction, class Range>
            typename boost::lazy_enable_if <range::has::size <Direction, Range>,
                range::result_of::size <Direction, Range>>::type
        operator() (Direction const & direction, Range && range) const
        { return range::size (direction, std::forward <Range> (range)); }

        template <class Direction, class Range>
            typename boost::disable_if <range::has::size <Direction, Range>,
                std::size_t>::type
        operator() (Direction const & direction, Range && range) const {
            std::size_t size = 0;
            auto current = range::view (range);
            while (!range::empty (current)) {
                current = range::drop (direction, current);
                ++ size;
            }
            return size;
        }

        // No direction: use default_direction.
        template <class Range> auto operator() (Range && range) const
        -> decltype (std::declval <walk_size>() (
            range::default_direction (range), std::declval <Range>()))
        {
            return (*this) (
                range::default_direction (range), std::forward <Range> (range));
        }
    };

} // namespace callable

/**
walk_size (range) or walk_size (direction, range) computes the number of
elements in any type of range.
If the range has a size() operation, that is used.
If not, then the drop() operation is used until the range is empty, and the
number of steps is counted.
\todo Currently only works on homogeneous ranges; use fold to remedy this.
*/
static const auto walk_size = callable::walk_size();

} // namespace range

#endif // RANGE_WALK_SIZE_HPP_INCLUDED

