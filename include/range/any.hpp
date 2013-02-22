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

#ifndef RANGE_ANY_HPP_INCLUDED
#define RANGE_ANY_HPP_INCLUDED

// The implementation of all and any is basically the same, and defined here:
#include "detail/fold_bool.hpp"

namespace range {

namespace callable { struct any : fold_bool_detail::fold_bool <false> {}; }

/**
Return whether any elements of the range evaluate to true.

Written tersely,
    any (r)
returns
    fold (||, false, r).

If the result is known at compile time, a compile time constant is returned.

Short-circuiting is important.
If any element is known at compile time to be true, then the following elements
are not instantiated, and none are evaluated.
If any element is found at run time to true, then the following elements are
not evaluated.

Note that for consistency, even if the result is known at compile time, the
elements up to the elements that decides this will be evaluated.
*/
static const auto any = callable::any();

} // namespace range

#endif // RANGE_ANY_HPP_INCLUDED

