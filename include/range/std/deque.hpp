// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::deque into a range.
This includes \c \<deque>.
*/

#ifndef RANGE_STD_DEQUE_HPP_INCLUDED
#define RANGE_STD_DEQUE_HPP_INCLUDED

#include <deque>

#include "range/detail/core_tag.hpp"
#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Type, class Allocator>
struct tag_of_qualified<std::deque<Type, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_STD_DEQUE_HPP_INCLUDED
