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
Define the "at" operation, which returns the element at a specific position in a
range.
*/

#ifndef RANGE_DETAIL_CORE_AT_HPP_INCLUDED
#define RANGE_DETAIL_CORE_AT_HPP_INCLUDED

#include <utility>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include "meta/vector.hpp"

namespace range {

/*
The operation "at" is not defined in namespace "operation", because this would
be impossible to do.
(This may be a flaw in the interface for ranges.)
Given just a range tag and not the type of the actual range, it is not possible
to find the return type of drop().
That return type may have a different range tag (indeed, for heterogeneous
ranges this is expected).
It is therefore impossible to know whether first() is implemented for the range.
The operation can be defined in namespace "apply", because then the range type
is known.
*/

namespace apply {
    template <class ... Arguments> struct at;
} // namespace apply

namespace callable {
    struct at : generic <apply::at> {};
} // namespace callable

/**
Return the element at a specific index in a range.
This is equivalent to first (direction, drop (direction, index, range)).
\param direction (optional) The direction of traversal.
\param index The index of the desired element.
\param range The range within which the element is sought.
\sa at_c
*/
static const auto at = callable::at();

/**
Return the element at a specific index in a range.
The first template parameter (the index) must be given explicitly.
This is equivalent to at (direction, rime::size_t<Index>(), range).
*/
template <size_t Index, class Direction, class Range>
    inline auto at_c (Direction const & direction, Range && range)
RETURNS (at (
    direction, rime::size_t <Index>(), std::forward <Range> (range)));

/**
Return the element at a specific index in a range.
The first template parameter (the index) must be given explicitly.
This is equivalent to at (rime::size_t<Index>(), range).
*/
template <size_t Index, class Range>
    inline auto at_c (Range && range)
RETURNS (at (rime::size_t <Index>(), std::forward <Range> (range)));

namespace apply {

    namespace detail {

        template <class Direction, class Index, class Range>
            struct at_implementation
        {
            auto operator() (Direction const & direction,
                Index const & index, Range && range) const
            RETURNS (range::first (direction, range::drop (
                direction, index, std::forward <Range> (range))));
        };

        template <class Direction, class Index, class Range>
            struct at
        : boost::mpl::if_ <range::has <callable::first (Direction,
                range::callable::drop (Direction, Index, Range))>,
            at_implementation <Direction, Index, Range>,
            operation::unimplemented
        >::type {};

    } // namespace detail

    namespace automatic_arguments {

        template <class Directions, class Indexes, class Ranges,
            class Enable = void>
        struct at : operation::unimplemented {};

        template <class Direction, class Index, class Range>
            struct at <meta::vector <Direction>, meta::vector <Index>,
                meta::vector <Range>>
        : apply::detail::at <Direction, Index, Range> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct at
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::at, meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_AT_HPP_INCLUDED

