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

/** \file
Declare the operations, but give no implementations.
Fold is one such operation.
A range might provide a fast implementation for fold, but the generic
implementation is given by "fold.hpp" (which a user must explicitly include).
The definition of ranges and the definition of "fold" should not depend on each
other.
This file therefore declares the points of contact.
*/

#ifndef RANGE_DETAIL_DECLARATIONS_HPP_INCLUDED
#define RANGE_DETAIL_DECLARATIONS_HPP_INCLUDED

namespace range { namespace operation {

    /**
    Return the result of a "fold" operation, a general form of iteration over
    the range.
    Generic "fold" functionality is based on drop and first, and implemented in
    "fold.hpp".
    This is automatically marked as implemented if the range has "empty"
    defined in the correct direction.
    ("drop" and "chop" doe not have to be defined, e.g. if the range is always
    empty.)
    Specialise this for a particular type of range if that makes it more
    efficient.
    */
    template <class RangeTag, class Direction, class Function, class State,
        class Enable = void>
    struct fold;
    /*{
        template <class Range>
            ... operator() (Direction const &,
                Function &&, State && state, Range && range) const;
    };*/

}} // namespace range::operation

#endif // RANGE_DETAIL_DECLARATIONS_HPP_INCLUDED
