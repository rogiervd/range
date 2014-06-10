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

/** \file
Helpers for all() and any().
*/

#ifndef RANGE_DETAIL_FIND_BOOL_HPP_INCLUDED
#define RANGE_DETAIL_FIND_BOOL_HPP_INCLUDED

#include "rime/core.hpp"

#include "../find.hpp"

namespace range { namespace callable { namespace find_bool_detail {

/**
Check the truth value either at compile time or run time.
*/
template <bool value> struct equals;

template <> struct equals <false> {
    template <class Type> auto operator() (Type const & v) const
    RETURNS (rime::not_ (v));
};

template <> struct equals <true> {
    template <class Type> auto operator() (Type const & v) const
    RETURNS (!rime::not_ (v));
};

/**
Generic implementation of all() and any().
all() finds the first element that evaluates to false and returns false iff it
finds one.
any() finds the first element that evaluates to true and returns true iff it
finds one.
*/
template <bool Value> struct find_bool {
    equals <!Value> equals_value;
    range::operation::helper::return_default_constructed <rime::bool_<!Value>>
        when_not_empty;
    range::operation::helper::return_default_constructed <rime::bool_<Value>>
        when_empty;

    template <class Direction, class Range>
        auto operator() (Direction const & direction, Range && range) const
    RETURNS (range::find (direction,
        equals_value, when_not_empty, when_empty, range));

    template <class Range>
        auto operator() (Range && range) const
    RETURNS (range::find (equals_value, when_not_empty, when_empty, range));
};

}}} // namespace range::callable::find_bool_detail

#endif  // RANGE_DETAIL_FIND_BOOL_HPP_INCLUDED

