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
    fold (r, &&, true).

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

