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

/** \file
Declare the "fold" operation, but give no implementation.
The reason for this is that a range might provide a fast implementation for
fold, but the generic implementation is given by "fold.hpp".
Neither of these files should rely on each other.
*/

#ifndef RANGE_DETAIL_CORE_FOLD_DECLARATION_HPP_INCLUDED
#define RANGE_DETAIL_CORE_FOLD_DECLARATION_HPP_INCLUDED

namespace range {
namespace operation {

    /**
    Return the result of a "fold" operation, a general form of iteration over
    the range.
    Generic "fold" functionality is based on drop and first, and  implemented in
    "fold.hpp".
    This is a hook that can be specialised for a particular type of range,
    which can be more efficient.
    */
    template <class RangeTag, class Direction, class Function, class State,
        class Enable = void>
    struct fold;
    /*{
        template <class Range>
            ... operator() (Direction const &,
                Function &&, State && state, Range && range) const;
    };*/

} // namespace operation
} // namespace range

#endif  // RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED

