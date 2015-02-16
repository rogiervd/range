/*
Copyright 2013, 2015 Rogier van Dalen.

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

#ifndef RANGE_ALL_OF_HPP_INCLUDED
#define RANGE_ALL_OF_HPP_INCLUDED

// The implementation of all and any is basically the same, and defined here:
#include "detail/find_bool.hpp"

namespace range {

namespace callable {
    struct all_of : find_bool_detail::find_bool <true> {};
} // namespace callable

/**
Return whether all elements of the range evaluate to true.

Written tersely,
    all_of (r)
returns
    fold (&&, true, r).

If the result is known at compile time, a compile time constant is returned.

Short-circuiting is important.
If any element is known at compile time to be false, then the following elements
are not instantiated.
If any element is found at run time to false, then the following elements are
not evaluated.
*/
static const auto all_of = callable::all_of();

} // namespace range

#endif // RANGE_ALL_OF_HPP_INCLUDED

