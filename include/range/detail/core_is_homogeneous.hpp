/*
Copyright 2011, 2012, 2013 Rogier van Dalen.

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

#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/all_of_c.hpp"

#include "rime/core.hpp"

#include "../direction.hpp"

namespace range {

namespace detail {

    /**
    Return true iff Range is homogeneous in Direction.
    This means that either:
    \li <c>drop \<Direction, Range></c> exists and returns a range of type
        \a Range modulo qualifications.
    \li <c>chop \<Direction, Range></c> exists and the range that it returns
        is of type \a Range modulo qualifications.
    */
    template <class Direction, class Range, class Enable = void>
        struct is_homogeneous_in
    : boost::mpl::false_ {};

    template <class Direction, class Range>
    struct is_homogeneous_in <Direction, Range, typename
        boost::enable_if <range::has <callable::drop (Direction, Range)>>::type>
    : boost::is_same <
        typename std::decay <Range>::type,
        typename decayed_result_of <callable::drop (Direction, Range)>::type
    > {};

    template <class Direction, class Range>
    struct is_homogeneous_in <Direction, Range, typename
        boost::enable_if_c <
            !range::has <callable::drop (Direction, Range)>::value &&
            range::has <callable::chop (Direction, Range)>::value
        >::type>
    : boost::is_same <
        typename std::decay <Range>::type,
        typename decayed_result_of <callable::chop (Direction, Range)>::type
            ::rest_type
    > {};

    /**
    Metafunction that returns true iff the range, the last argument, is
    homogeneous in all directions.
    */
    template <class Directions, class Range> struct is_homogeneous;

    template <class ... Directions, class Range>
        struct is_homogeneous <meta::vector <Directions ...>, Range>
    : meta::all_of_c <is_homogeneous_in <Directions, Range>::value ...> {};

} // namespace detail

/**
Metafunction that returns true iff drop (Direction, Range) returns a Range
for all Directions.
This means, for example, that a recursive iteration can be written as a loop.
Since applying "drop" with an increment is equivalent to applying "drop" a
number of times, a homogeneous range cannot become heterogeneous.

The last argument is Range.
The arguments before it form Directions.
If only one argument is given, it is Range, and its default direction is used.
Range can also be a callable expression.
*/
template <class ... Arguments> struct is_homogeneous;

template <class ... Arguments> struct is_homogeneous
: rime::as_rime_constant <
    detail::is_homogeneous <
        typename meta::drop <meta::back, meta::vector <Arguments ...>>::type,
        typename meta::first <meta::back, meta::vector <Arguments ...>>::type>
>::type {};

template <class Range> struct is_homogeneous <Range>
: is_homogeneous <
    typename result_of <callable::default_direction (Range)>::type, Range>
{};

} // namespace range

#endif  // RANGE_DETAIL_CORE_IS_HOMOGENEOUS_HPP_INCLUDED

