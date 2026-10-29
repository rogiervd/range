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
#include "std/array.hpp"
#include "std/deque.hpp"
#include "std/forward_list.hpp"
#include "std/list.hpp"
#include "std/string.hpp"
#include "std/vector.hpp"

// Associative containers.
#include "std/map.hpp"
#include "std/set.hpp"

// Unordered (hashed) associative containers.
#include "std/unordered_map.hpp"
#include "std/unordered_set.hpp"

// tuple.
#include "std/tuple.hpp"

// view_optional.
#include "std/view_optional.hpp"

#endif  // RANGE_STD_HPP_INCLUDED
