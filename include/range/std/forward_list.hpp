// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::forward_list into a range.
This includes \c \<forward_list>.
*/

#ifndef RANGE_FORWARD_LIST_HPP_INCLUDED
#define RANGE_FORWARD_LIST_HPP_INCLUDED

#include <forward_list>

#include "range/detail/core_tag.hpp"
#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Type, class Allocator>
struct tag_of_qualified<std::forward_list<Type, Allocator>>
{
    typedef std_container_operation::std_front_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_FORWARD_LIST_HPP_INCLUDED
