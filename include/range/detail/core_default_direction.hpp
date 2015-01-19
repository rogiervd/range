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

#ifndef RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED
#define RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {

    /**
    The default implementation of \c default_direction.
    Return \c range::front
    */
    struct default_direction_default {
        template <class Range> direction::front operator() (Range const &) const
        { return range::front; }
    };

    /** \brief
    Return the default direction for a range.

    The standard implementation forwards to the <c>.default_direction()</c>
    member function.
    If that is not available, it returns \c range::front.

    Implement this by specialising this class or providing the member function
    for a range that has another default direction than \c range::front.

    \tparam RangeTag The range tag.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Range, class Enable /*= void*/>
        struct default_direction
    : try_all <member_access::default_direction <Range>,
        default_direction_default> {};

    template <class Range> struct default_direction <not_a_range_tag, Range>
    : unimplemented {};

} // namespace operation

namespace apply {

    template <class ... Arguments> struct default_direction;

    template <class Range> struct default_direction <Range>
    : operation::default_direction <typename tag_of <Range>::type, Range &&> {};

} // namespace apply

namespace callable {
    struct default_direction : generic <apply::default_direction> {};
} // namespace callable

static const auto default_direction = callable::default_direction();

} // namespace range

#endif  // RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED
