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

#ifndef RANGE_DETAIL_WITH_DIRECTION_HPP_INCLUDED
#define RANGE_DETAIL_WITH_DIRECTION_HPP_INCLUDED

#include "rime/assert.hpp"

namespace range { namespace detail {

/**
Ranges often have to hold directions; but they should not take up unnecessary
space.
This class compresses empty \a Direction objects by deriving from the
\a Direction class.
*/
template <class Direction> class with_direction
: Direction {
public:
    with_direction (Direction const & direction)
    : Direction (direction) {}

    Direction const & direction() const { return *this; }

    Direction const & direction_must_be_equal (Direction const & that_direction)
        const
    {
        rime::assert_ (this->direction() == that_direction);
        return that_direction;
    }
};

} // namespace detail

} // namespace range

#endif // RANGE_DETAIL_WITH_DIRECTION_HPP_INCLUDED
