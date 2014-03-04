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

#ifndef RANGE_DETAIL_CORE_IS_HOMOGENEOUS_HPP_INCLUDED
#define RANGE_DETAIL_CORE_IS_HOMOGENEOUS_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/all.hpp"

#include "rime/core.hpp"

#include "../direction.hpp"

namespace range {

namespace detail {

    /**
    Return true iff Range is homogeneous in Direction.
    \pre bump <Direction, Range> exists.
    */
    template <class Direction, class Range> struct is_homogeneous_in
    : boost::is_same <
        typename std::decay <typename result_of <Range>::type>::type,
        typename std::decay <typename result_of_or <
            callable::drop (Direction, Range), void>::type>::type
    > {};

    /**
    Metafunction that returns true iff the range, the last argument, is
    homogeneous in all directions.
    */
    template <class Directions, class Range> struct is_homogeneous;

    template <class ... Directions, class Range>
        struct is_homogeneous <meta::vector <Directions ...>, Range>
    : meta::all <meta::vector <is_homogeneous_in <Directions, Range>...>> {};

} // namespace detail

/**
Metafunction that returns true iff drop (Direction, Range) returns a Range
for all Directions.
This means, for example, that a recursive iteration can be written as a loop.
Since applying "drop" with an increment is equivalent to applying "drop" a
number of times, a homogeneous range cannot become heterogeneous.

The last argument is Range.
The arguments before it form Directions.
If only one argument is given, it is Range, and its default direction is used.
Range can also be a callable expression.
*/
template <class ... Arguments> struct is_homogeneous;

template <class ... Arguments> struct is_homogeneous
: rime::as_rime_constant <
    detail::is_homogeneous <
        typename meta::drop <meta::back, meta::vector <Arguments ...>>::type,
        typename meta::first <meta::back, meta::vector <Arguments ...>>::type>
>::type {};

template <class Range> struct is_homogeneous <Range>
: is_homogeneous <
    typename result_of <callable::default_direction (Range)>::type, Range>
{};

} // namespace range

#endif  // RANGE_DETAIL_CORE_IS_HOMOGENEOUS_HPP_INCLUDED

