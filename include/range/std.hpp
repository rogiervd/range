// SPDX-FileCopyrightText: Copyright 2013, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

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
