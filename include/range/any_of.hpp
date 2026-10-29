// SPDX-FileCopyrightText: Copyright 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_ANY_OF_HPP_INCLUDED
#define RANGE_ANY_OF_HPP_INCLUDED

// The implementation of all and any is basically the same, and defined here:
#include "detail/find_bool.hpp"

namespace range {

namespace callable {
    struct any_of : find_bool_detail::find_bool<false> {};
}  // namespace callable

/**
Return whether any elements of the range evaluate to true.

Written tersely,
    any_of (r)
returns
    fold (||, false, r).

If the result is known at compile time, a compile time constant is returned.

Short-circuiting is important.
If any element is known at compile time to be true, then the following elements
are not instantiated.
If any element is found at run time to true, then the following elements are
not evaluated.
*/
static auto const any_of = callable::any_of();

}  // namespace range

#endif  // RANGE_ANY_OF_HPP_INCLUDED
