// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::unordered_map into a range.
This includes \c \<unordered_map>.
*/

#ifndef RANGE_STD_UNORDERED_MAP_HPP_INCLUDED
#define RANGE_STD_UNORDERED_MAP_HPP_INCLUDED

#include <unordered_map>

#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
struct tag_of_qualified<
    std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};

template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
struct tag_of_qualified<
    std::unordered_multimap<Key, Value, Hash, KeyEqual, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_STD_UNORDERED_MAP_HPP_INCLUDED
