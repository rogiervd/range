/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_CHOP_IN_PLACE_HPP_INCLUDED
#define RANGE_DETAIL_CORE_CHOP_IN_PLACE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include "meta/vector.hpp"

#include "utility/storage.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {

    /* chop_in_place. */

    /*
    Two strategies for synthesising the implementation can be used:
    1. Use first() and drop().
    2. Use chop().
    Both are quite straightforward.
    */

    // Strategy 1: synthesise an implementation with first() and drop().

    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct chop_in_place_by_first_drop
    : unimplemented {};

    namespace chop_detail {

        template <class Type, class Expression> struct result_is_same
        : std::is_same <Type, typename result_of <Expression>::type> {};

        template <class Type, class Expression> struct rest_type_is_same
        : std::is_same <Type, typename result_of <Expression>::type::rest_type>
        {};

    } // namespace chop_detail

    template <class RangeTag, class Direction, class Range>
        class chop_in_place_by_first_drop <RangeTag, Direction, Range &,
            typename boost::enable_if <boost::mpl::and_ <
                is_implemented <first <RangeTag, Direction, Range &>>,
                is_implemented <drop <RangeTag, Direction, one_type, Range &&>>,
                chop_detail::result_is_same <Range,
                    callable::drop (Direction, Range)>
            >>::type>
    {
    public:
        typename result_of <range::callable::first (
            Direction const &, Range &)>::type
        operator() (Direction const & direction, Range & range) const
        {
            auto && first = range::first (direction, range);
            range = range::drop (direction, std::move (range));
            return static_cast <decltype (first)> (first);
        }
    };

    // Strategy 2: synthesise an implementation with chop().

    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct chop_in_place_by_chop
    : unimplemented {};

    template <class RangeTag, class Direction, class Range>
        struct chop_in_place_by_chop <RangeTag, Direction, Range &, typename
        boost::enable_if <boost::mpl::and_ <
            is_implemented <chop <RangeTag, Direction, Range &&>>,
            chop_detail::rest_type_is_same <Range,
                callable::chop (Direction, Range)>
        >>::type>
    {
        typename result_of <callable::chop (Direction, Range)>::type::first_type
        operator() (Direction const & direction, Range & range) const {
            auto result = range::chop (direction, std::move (range));
            range = result.move_rest();
            return result.forward_first();
        }
    };

    /** \brief
    Return the first element of the range, and replace the range in place by
    the range without the first element.

    The range must be homogeneous.

    The standard implementation forwards to the
    <c>.chop_in_place (Direction)</c> member function, if that is available.

    If this is not implemented, a default implementation is provided if either
    \a first and \a drop are implemented, or if \a chop is implemented.
    This happens in \c chop_in_place_automatic.
    If for some reason this needs to be switched off, then
    \c chop_in_place_automatic can be implemented as deriving from
    \c unimplemented.

    Usually, it is necessary to make sure that only non-const reference
    ranges are picked up.
    When specialising this struct, the range tag must then differentiate between
    non-const references and other qualificatiers.

    \tparam RangeTag The range tag.
    \tparam Direction The decayed direction type.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Direction, class Range, class Enable>
        struct chop_in_place
    : member_access::chop_in_place <Direction, Range> {/*
        ... operator() (Direction const & direction, Range && range) const;
    */};

    /** \brief
    Return the first element of the range, and replace the range in place by
    the range without the first element, using \c first and \c drop if necessary
    and possible.

    The only reason to specialise this would be disable this and derive it from
    \c unimplemented even when \c chop is implemented.
    */
    template <class RangeTag, class Direction, class Range>
        struct chop_in_place_automatic
    : try_all <chop_in_place <RangeTag, Direction, Range>,
        chop_in_place_by_first_drop <RangeTag, Direction, Range>,
        chop_in_place_by_chop <RangeTag, Direction, Range>> {};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct chop_in_place;
} // namespace apply

namespace callable {
    struct chop_in_place : generic <apply::chop_in_place> {};
} // namespace callable

/**
Return the first element of the range, and remove this element from the range
itself.

\param direction
    (optional) The direction from which the first element is taken.
\param range
    The range to operate on.
    It must be homogeneous, so that without the first element it has the same
    type.
    This is normally required to be a reference.
*/
static const auto chop_in_place = callable::chop_in_place();

namespace apply {

    namespace automatic_arguments {

        /* chop_in_place. */

        template <class Directions, class Others, class Ranges,
            class Enable = void>
        struct chop_in_place : operation::unimplemented {};

        template <class Direction, class Range>
            struct chop_in_place <
                meta::vector <Direction>, meta::vector<>, meta::vector <Range>>
        : operation::chop_in_place_automatic <typename tag_of <Range>::type,
            typename std::decay <Direction>::type, Range &&> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct chop_in_place
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::chop_in_place, meta::vector <Arguments ...>>::type
    {};

}} // namespace range::apply

#endif  // RANGE_DETAIL_CORE_CHOP_IN_PLACE_HPP_INCLUDED
