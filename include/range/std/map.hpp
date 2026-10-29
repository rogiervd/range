// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::map into a range.
This includes \c \<map>.
*/

#ifndef RANGE_STD_MAP_HPP_INCLUDED
#define RANGE_STD_MAP_HPP_INCLUDED

#include <map>

#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Key, class Value, class Compare, class Allocator>
struct tag_of_qualified<std::map<Key, Value, Compare, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};

template <class Key, class Value, class Compare, class Allocator>
struct tag_of_qualified<std::multimap<Key, Value, Compare, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_STD_MAP_HPP_INCLUDED
