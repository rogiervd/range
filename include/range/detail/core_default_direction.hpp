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
