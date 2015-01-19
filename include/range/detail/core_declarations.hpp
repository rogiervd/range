/*
Copyright 2011-2015 Rogier van Dalen.

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
    ("drop" and "chop" do not have to be defined, e.g. if the range is always
    empty.)
    Specialise this for a particular type of range if that makes it more
    efficient.
    */
    template <class RangeTag, class Direction, class Function, class State,
        class Range, class Enable = void>
    struct fold;
    /*{
        ... operator() (Direction const &,
            Function &&, State && state, Range && range) const;
    };*/

    /**
    Call a function on each element of a range.
    This is normally implemented through "fold", which is by default implemented
    where possible.
    Specialise this for a particular type of range if that makes it more
    efficient.
    In particular, for_each always returns void and it does not have to compute
    any return types.
    Therefore, the number of template instantiations can be lower.
    */
    template <class RangeTag, class Direction, class Function, class Range,
        class Enable = void>
    struct for_each;
    /*{
        void operator() (Direction const &, Function &&, Range &&) const;
    };*/

    /**
    Return a lazy "prefix sum", i.e. all the intermediate step of an
    accumulation.
    This is implemented for general ranges in scan.hpp.
    For some ranges, it might be more efficient to implement it specifically.
    */
    template <class RangeTag, class Direction, class Function, class State,
            class Range, class Enable = void>
        struct scan;

    /**
    Compare two ranges lexicographically.
    This is implemented for general ranges in less_lexicographical.hpp.
    For some combination of ranges, it might be more efficient to specialise
    this to implement it.
    */
    template <class Range1Tag, class Range2Tag, class Direction, class Less,
        class Range1, class Range2, class Enable = void>
    struct less_lexicographical;

    /**
    Compare two ranges for equality.
    This means that the number of elements is equal, and the elements compare
    equal.
    This is implemented for general ranges in equal.hpp.
    For some combination of ranges, it might be more efficient to specialise
    this to implement it.
    */
    template <class Range1Tag, class Range2Tag, class Direction,
        class Predicate, class Range1, class Range2, class Enable = void>
    struct equal;

}} // namespace range::operation

#endif // RANGE_DETAIL_DECLARATIONS_HPP_INCLUDED
