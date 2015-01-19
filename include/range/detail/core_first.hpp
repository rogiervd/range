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

#ifndef RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED
#define RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {


    /// Implement "first" by calling "chop".
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct first_by_chop
    : unimplemented {};

    template <class RangeTag, class Direction, class Range>
        struct first_by_chop <RangeTag, Direction, Range, typename
            boost::enable_if <
                is_implemented <chop <RangeTag, Direction, Range>>>::type>
    {
        auto operator() (Direction const & direction, Range && range) const
        RETURNS (chop <RangeTag, Direction, Range>() (
            direction, std::forward <Range> (range)).forward_first());
    };

    /** \brief
    Return the first element in the range.

    The standard implementation forwards to the <c>.first (Direction)</c>
    member function, if that is available.

    This normally should be implemented (by providing the member function or by
    specialising this) for any range.

    If this is not implemented, but operation::chop is implemented, then this is
    automatically implemented in terms of \c chop.
    This only works for the qualification that \c chop is implemented for.
    This happens in \c first_automatic.
    If for some reason this needs to be switched off, then \c first_automatic
    can be implemented as deriving from \c unimplemented.

    \tparam RangeTag The range tag.
    \tparam Direction The decayed direction type.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Direction, class Range, class Enable>
        struct first
    : member_access::first <Direction, Range>
    {/*
        ... operator() (Direction const & direction, Range && range) const;
    */};

    /** \brief
    Return the first element in the range, using \c chop if necessary and
    possible.

    The only reason to specialise this would be disable this and derive it from
    \c unimplemented even when \c chop is implemented.
    */
    template <class RangeTag, class Direction, class Range>
        struct first_automatic
    : try_all <first <RangeTag, Direction, Range>,
        first_by_chop <RangeTag, Direction, Range>> {};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct first;
} // namespace apply

namespace callable {
    struct first : generic <apply::first> {};
} // namespace callable

static const auto first = callable::first();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct first : operation::unimplemented {};

        template <class Direction, class Range>
            struct first <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::first_automatic <typename range::tag_of <Range>::type,
            typename std::decay <Direction>::type, Range &&> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct first
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::first, meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED
