/*
Copyright 2011-2014 Rogier van Dalen.

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
Relevant categories of qualification of ranges.
*/
enum qualification { any_qualification, temporary, reference, const_reference };

/**
Helper for tag_of.
Specialise this for a range type, and optionally a qualification, to assign
tags to the range with various const and reference qualification.
By default, this forwards to itself with the same \a Range and
\c any_qualification, and then to itself with \c void and \c any_qualification,
resulting in not_a_range_tag.

Therefore, if it does not depend on const or reference qualification whether
operations are defined, specialise tag_of_qualified giving only the Range.
If this does make a difference, then additionally provide a specialisation with
a second template argument (probably \c temporary).
*/
template <class Range, qualification qualifier = any_qualification>
    struct tag_of_qualified
: std::conditional <(qualifier == any_qualification),
    tag_of_qualified <void>,
    tag_of_qualified <Range>>::type {};

template <> struct tag_of_qualified <void>
{ typedef not_a_range_tag type; };

/**
Evaluate the range tag of the type Range.
The tag usually does not depend on the qualifier, but it may.
By default, this forwards to tag_of_qualified.

tag_of should usually be used to retrieve the range tag for a type.
To assign tags to ranges, it is usually easiest to specialise tag_of_qualified.
*/
template <class Range> struct tag_of
: tag_of_qualified <Range, temporary> {};

template <class Range> struct tag_of <Range &>
: tag_of_qualified <Range, reference> {};

template <class Range> struct tag_of <Range &&>
: tag_of_qualified <Range, temporary> {};

template <class Range> struct tag_of <Range const>
: tag_of_qualified <Range, const_reference> {};

template <class Range> struct tag_of <Range const &>
: tag_of_qualified <Range, const_reference> {};

template <class Range> struct tag_of <Range const &&>
: tag_of_qualified <Range, const_reference> {};

/**
Evaluate to true if Range is a range type.
*/
template <class Range> struct is_range
: boost::mpl::not_ <std::is_same <
    typename tag_of <Range>::type, not_a_range_tag>> {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_TAG_HPP_INCLUDED
