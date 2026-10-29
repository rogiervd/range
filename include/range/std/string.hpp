// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::basic_string into a range.
This includes \c \<string>.
*/

#ifndef RANGE_STD_BASIC_STRING_HPP_INCLUDED
#define RANGE_STD_BASIC_STRING_HPP_INCLUDED

#include <string>

#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Type, class Traits, class Allocator>
struct tag_of_qualified<std::basic_string<Type, Traits, Allocator>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_STD_BASIC_STRING_HPP_INCLUDED
