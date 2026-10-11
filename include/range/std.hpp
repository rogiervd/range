/*
Copyright 2013, 2017 Rogier van Dalen.

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

/** \file
Turn heterogeneous and homogeneous containers in std:: into ranges.

This file includes all the required system headers.
For compile speed, prefer to include the headers from \c ./std/.
*/

#ifndef RANGE_STD_HPP_INCLUDED
#define RANGE_STD_HPP_INCLUDED

// Sequence containers.
#include "std/array.hpp"         // IWYU pragma: export
#include "std/deque.hpp"         // IWYU pragma: export
#include "std/forward_list.hpp"  // IWYU pragma: export
#include "std/list.hpp"          // IWYU pragma: export
#include "std/string.hpp"        // IWYU pragma: export
#include "std/vector.hpp"        // IWYU pragma: export

// Associative containers.
#include "std/map.hpp"  // IWYU pragma: export
#include "std/set.hpp"  // IWYU pragma: export

// Unordered (hashed) associative containers.
#include "std/unordered_map.hpp"  // IWYU pragma: export
#include "std/unordered_set.hpp"  // IWYU pragma: export

// tuple.
#include "std/tuple.hpp"  // IWYU pragma: export

// view_optional.
#include "std/view_optional.hpp"  // IWYU pragma: export

#endif  // RANGE_STD_HPP_INCLUDED
