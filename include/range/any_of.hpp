/*
Copyright 2013, 2015 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef RANGE_ANY_OF_HPP_INCLUDED
#define RANGE_ANY_OF_HPP_INCLUDED

// The implementation of all and any is basically the same, and defined here:
#include "detail/find_bool.hpp"

namespace range {

namespace callable {
    struct any_of : find_bool_detail::find_bool <false> {};
} // namespace callable

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
static const auto any_of = callable::any_of();

} // namespace range

#endif // RANGE_ANY_OF_HPP_INCLUDED

