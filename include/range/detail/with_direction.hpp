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

/**
Hold a \a Direction object (and optimise space if it is empty).
Additionally, implement default_direction to return that direction.

To implement a range that holds its default direction, simply derive from this
class.
*/
template <class Direction> class with_default_direction
: public with_direction <Direction> {
public:
    with_default_direction (Direction const & direction)
    : with_direction <Direction> (direction) {}

    Direction const & default_direction() const { return this->direction(); }
};

} // namespace detail

} // namespace range

#endif // RANGE_DETAIL_WITH_DIRECTION_HPP_INCLUDED
