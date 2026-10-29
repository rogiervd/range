// SPDX-FileCopyrightText: Copyright 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_ALL_OF_HPP_INCLUDED
#define RANGE_ALL_OF_HPP_INCLUDED

// The implementation of all and any is basically the same, and defined here:
#include "detail/find_bool.hpp"

namespace range {

namespace callable {
    struct all_of : find_bool_detail::find_bool<true> {};
}  // namespace callable

/**
Return whether all elements of the range evaluate to true.

Written tersely,
    all_of (r)
returns
    fold (r, &&, true).

If the result is known at compile time, a compile time constant is returned.

Short-circuiting is important.
If any element is known at compile time to be false, then the following elements
are not instantiated.
If any element is found at run time to false, then the following elements are
not evaluated.
*/
static auto const all_of = callable::all_of();

}  // namespace range

#endif  // RANGE_ALL_OF_HPP_INCLUDED
