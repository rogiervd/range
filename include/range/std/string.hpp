/*
Copyright 2013, 2015, 2017 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
