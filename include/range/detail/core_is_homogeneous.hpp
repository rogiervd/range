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

#ifndef RANGE_DETAIL_CORE_IS_HOMOGENEOUS_HPP_INCLUDED
#define RANGE_DETAIL_CORE_IS_HOMOGENEOUS_HPP_INCLUDED

#include <type_traits>

#include "utility/enable_if_compiles.hpp"

#include "rime/core.hpp"

#include "../direction.hpp"

namespace range {

namespace is_homogeneous_detail {

    /** \brief
    Return \c true iff Range is homogeneous in Direction.

    This means that \c chop_in_place is implemented, either directly, or through
    \c drop or \c chop.
    */
    template <class Range, class Direction, class Enable = void>
        struct is_homogeneous
    : rime::false_type {};

    template <class Range, class Direction>
        struct is_homogeneous <Range, Direction,
            typename utility::enable_if_compiles <
                decltype (std::declval <callable::chop_in_place>() (
                    std::declval <Range &>(), std::declval <Direction>()))
                >::type>
    : rime::true_type {};

} // namespace is_homogeneous_detail

/** \brief
Metafunction that returns true iff <c>drop (range, direction)</c> returns a
Range, or if <c>chop (range, direction)</c> is implemented.

This means, for example, that a recursive iteration can be written as a loop.
Since applying "drop" with an increment is equivalent to applying "drop" a
number of times, a homogeneous range cannot become heterogeneous.

\param Range
    The range to check for homogeneousness.
    Qualifications are ignored.
\param Direction (optional)
    The direction.
    If left out, the default direction is used.
*/
template <class Range, class Direction = decltype (
        callable::default_direction() (std::declval <Range>()))>
    struct is_homogeneous
: is_homogeneous_detail::is_homogeneous <
    typename std::decay <Range>::type, Direction>
{
    static_assert (is_range <Range>::value, "Range must be a range.");
    static_assert (is_direction <Direction>::value,
        "Direction must be a direction.");
};

} // namespace range

#endif  // RANGE_DETAIL_CORE_IS_HOMOGENEOUS_HPP_INCLUDED
