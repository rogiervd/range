/*
Copyright 2011, 2012, 2013 Rogier van Dalen.

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
Ranges use tag dispatching for operations.
Tags should contain enough information to decide whether an operation can
proceed, generally to the point where the return type can be computed.
(For example, the return type of "drop" on a known-empty range does not exist.)

Normally, ranges should specialise tag_of_unqualified to specify a tag.
However, this is just a convenience mechanism for tag_of.
If it depends on, for example, const specification, whether operations compile,
then specialise tag_of.
*/
template <class Range, class Enable = void>
    struct tag_of_unqualified { typedef not_a_range_tag type; };

/**
Convenience definition for ranges that assigns a tag only to lvalue references.
\tparam Range The type, which may be cv-qualified.
*/
template <class Range, class Enable = void>
    struct tag_of_lvalue_reference
: tag_of_unqualified <typename std::decay <Range>::type> {};

/**
Convenience definition for ranges that assigns a tag only to rvalues.
\tparam Range The type, which may be cv-qualified.
*/
template <class Range, class Enable = void>
    struct tag_of_rvalue
: tag_of_unqualified <typename std::decay <Range>::type> {};

/**
Evaluate the range tag of the type Range.
The tag usually does not depend on the qualification, but it may.
*/
template <class Range> struct tag_of
: tag_of_rvalue <Range> {};

template <class Range> struct tag_of <Range &>
: tag_of_lvalue_reference <Range> {};

template <class Range> struct tag_of <Range &&>
: tag_of_rvalue <Range> {};

/**
Evaluate to true if Range is a range type.
*/
template <class Range> struct is_range
: boost::mpl::not_ <std::is_same <
    typename tag_of <Range>::type, not_a_range_tag>> {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_TAG_HPP_INCLUDED
