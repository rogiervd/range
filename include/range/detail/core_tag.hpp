/*
Copyright 2011-2015 Rogier van Dalen.

This file is part of Rogier van Dalen's Range library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
