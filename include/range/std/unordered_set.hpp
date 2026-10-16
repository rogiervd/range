// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::unordered_set into a range.
This includes \c \<unordered_set>.
*/

#ifndef RANGE_STD_UNORDERED_SET_HPP_INCLUDED
#define RANGE_STD_UNORDERED_SET_HPP_INCLUDED

#include <unordered_set>

#include "range/detail/core_tag.hpp"
#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Key, class Hash, class KeyEqual, class Allocator>
struct tag_of_qualified<std::unordered_set<Key, Hash, KeyEqual, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};

template <class Key, class Hash, class KeyEqual, class Allocator>
struct tag_of_qualified<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_STD_UNORDERED_SET_HPP_INCLUDED
