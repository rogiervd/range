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

#ifndef RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED
#define RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "rime/core.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {

    /// Turn Direction into the forward direction and apply "empty".
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct empty_by_forward
    : unimplemented {};

    template <class RangeTag, class Direction, class Range>
        struct empty_by_forward <RangeTag, Direction, Range, typename
            boost::enable_if <boost::mpl::and_ <
                has <direction::callable::make_forward (Direction)>,
                range_detail::is_implemented_forward <empty,
                    RangeTag, Direction, Range>
            >>::type>
    : range_detail::forward_operation <empty, RangeTag, Direction, Range> {};

    /** \brief
    Return \c true if there are no elements in the range.

    The standard implementation forwards to the <c>.empty (Direction)</c>
    member function.
    If that is not available, it will forward to the forward direction, that is,
    <c>empty (make_forward (direction), range)</c>, if that is defined.

    This needs to be implemented (by providing the member function or by
    specialising this) for any range, but only for the forward direction.
    For example, by defining it for \c direction::front, it will automatically
    also be defined for \c direction::back.

    \tparam RangeTag The range tag.
    \tparam Direction The decayed direction type.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Direction, class Range, class Enable>
        struct empty
    : try_all <member_access::empty <Direction, Range>,
        empty_by_forward <RangeTag, Direction, Range>>
    {/*
        ... operator() (Direction const & direction, Range && range) const;
    */};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct empty;
} // namespace apply

namespace callable {
    struct empty : generic <apply::empty> {};
} // namespace callable

static const auto empty = callable::empty();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct empty : operation::unimplemented {};

        template <class Direction, class Range>
            struct empty <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::empty <typename range::tag_of <Range>::type,
            typename std::decay <Direction>::type, Range &&> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct empty
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::empty, meta::vector <Arguments ...>>::type {};

} // namespace apply

/**
Evaluate to \c true iff the range is known at compile time to be empty.
This happens when <c>empty (direction, range)</c> returns a compile-time
constant with value true.

If this evaluates to \c false, it is still possible for the range to be empty at
run time.
*/
template <class Direction, class Range> struct always_empty
: rime::equal_constant <
    typename result_of <callable::empty (Direction, Range)>::type,
    rime::true_type> {};

/**
Evaluate to \c true iff the range is known at compile time to be not empty.
This happens when <c>empty (direction, range)</c> returns a compile-time
constant with value false.

If this evaluates to \c false, it is still possible for the range to be
non-empty at run time.
*/
template <class Direction, class Range> struct never_empty
: rime::equal_constant <
    typename result_of <callable::empty (Direction, Range)>::type,
    rime::false_type> {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED
