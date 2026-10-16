// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Turn \c std::array into a range.
This includes \c \<array>.
*/

#ifndef RANGE_STD_ARRAY_HPP_INCLUDED
#define RANGE_STD_ARRAY_HPP_INCLUDED

#include <array>
#include <cstddef>

#include "range/detail/core_tag.hpp"
#include "range/std/detail/container_tag.hpp"

namespace range {

/// \cond DONT_DOCUMENT
template <class Type, std::size_t Number>
struct tag_of_qualified<std::array<Type, Number>>
{
    typedef std_container_operation::std_front_back_container_tag type;
};
/// \endcond

}  // namespace range

#endif  // RANGE_STD_ARRAY_HPP_INCLUDED
