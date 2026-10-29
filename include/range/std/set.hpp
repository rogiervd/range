// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::set into a range.
This includes \c \<set>.
*/

#ifndef RANGE_STD_SET_HPP_INCLUDED
#define RANGE_STD_SET_HPP_INCLUDED

#include <set>

#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Key, class Compare, class Allocator>
struct tag_of_qualified<std::set<Key, Compare, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};

template <class Key, class Compare, class Allocator>
struct tag_of_qualified<std::multiset<Key, Compare, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_STD_SET_HPP_INCLUDED
