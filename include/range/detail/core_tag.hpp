/*
Copyright 2011-2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_TAG_HPP_INCLUDED
#define RANGE_DETAIL_CORE_TAG_HPP_INCLUDED

#include <type_traits>
#include <boost/mpl/not.hpp>

namespace range {

struct not_a_range_tag;

/**
Helper for tag_of.
Specialise this for an unqualified range to assign it a tag.
*/
template <class Range> struct tag_of_qualified
{ typedef not_a_range_tag type; };

/**
Evaluate the range tag of the type Range.
The tag should not depend on the qualifier.

tag_of should be used to retrieve the range tag for a type.
To assign tags to ranges, it is easiest to specialise tag_of_qualified.
*/
template <class Range> struct tag_of
: tag_of_qualified <typename std::decay <Range>::type> {};

/**
Evaluate to true if Range is a range type.
*/
template <class Range> struct is_range
: boost::mpl::not_ <std::is_same <
    typename tag_of <Range>::type, not_a_range_tag>> {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_TAG_HPP_INCLUDED
