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

#ifndef RANGE_DETAIL_CORE_DROP_HPP_INCLUDED
#define RANGE_DETAIL_CORE_DROP_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "rime/core.hpp"

#include "utility/returns.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {

    template <class RangeTag, class Direction, class Increment, class Range,
            class Enable>
        struct drop;

    template <class RangeTag, class Direction, class Increment, class Range,
        class Enable = void>
    struct drop_constant;

    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct drop_one;

    typedef rime::size_t <1> one_type;

    template <class RangeTag, class Direction, class Increment, class Range,
            class Enable = void>
        struct drop_by_chop
    : unimplemented {};

    template <class RangeTag, class Direction, class Increment, class Range>
        struct drop_by_chop <RangeTag, Direction, Increment, Range, typename
            boost::enable_if <boost::mpl::and_ <
                rime::equal_constant <Increment, one_type>,
                is_implemented <chop <RangeTag, Direction, Range>>
        >>::type>
    {
        template <class One> auto
            operator() (Direction const & direction, One const &,
                Range && range) const
        RETURNS (chop <RangeTag, Direction, Range>() (
            direction, std::forward <Range> (range)).forward_rest());
    };

    /**
    Convenience operation.
    Can be specialised if \c drop is only available for an increment of one.

    If operation::chop is implemented, this is automatically implemented in
    terms of it.
    */
    template <class RangeTag, class Direction, class Range, class Enable>
    struct drop_one
    : member_access::drop_one <Direction, Range>
    {/*
        template <class One>
            ... operator() (Direction const & direction,
                One const &, Range && range) const;
    */};

    template <class RangeTag, class Direction, class Increment, class Range,
        class Enable = void>
    struct drop_by_drop_one
    : unimplemented {};

    template <class RangeTag, class Direction, class Increment, class Range>
    struct drop_by_drop_one <RangeTag, Direction, Increment, Range,
        typename boost::enable_if <
            rime::equal_constant <Increment, one_type>>::type>
    : drop_one <RangeTag, Direction, Range> {};

    /**
    Convenience operation.
    Can be specialised if \c drop is only available for increments known at
    compile time.
    */
    template <class RangeTag, class Direction, class Increment, class Range,
        class Enable>
    struct drop_constant
    : member_access::drop_constant <Direction, Increment, Range>
    {/*
        ... operator() (Direction const & direction,
            Increment const & increment, Range && range) const;
    */};

    template <class RangeTag, class Direction, class Increment, class Range,
            class Enable = void>
        struct drop_by_drop_constant
    : unimplemented {};

    template <class RangeTag, class Direction, class Increment, class Range>
        struct drop_by_drop_constant <RangeTag, Direction, Increment, Range,
            typename boost::enable_if <rime::is_constant <Increment>>::type>
    : drop_constant <RangeTag, Direction, Increment, Range> {};

    /** \brief
    Return the range without the first \c Increment elements from the direction
    \c Direction.
    The return type and value should be the same as the return type and value
    of \c Increment calls to drop <DirectionTag, one_type>.

    The default behaviour of this is as follows:
    \li If \a Increment equals 1, then try forwarding to \c drop_one.
    \li If this is not specialised, it will try calling
        <c>.drop_one (direction)</c>.
    \li If \a Increment is a constant, then try forwarding to \c drop_constant.
    \li If this is not specialised, it will try calling
        <c>.drop_constant (direction, increment)</c>.
    \li Try calling <c>.drop (direction, increment)</c>.

    To implement this, implement one of the member functions \c drop_one,
    \c drop_constant, or \c drop, or specialise one of the template classes
    \c drop_one, \c drop_constant, or \c drop.

    If this is not implemented, but operation::chop is implemented, then drop
    of one element is automatically implemented in terms of \c chop.
    This only works for the qualification that \c chop is implemented for.
    This happens in \c drop_automatic.
    If for some reason this needs to be switched off, then \c drop_automatic
    can be implemented as deriving from \c unimplemented.

    \tparam RangeTag The range tag.
    \tparam Direction The decayed direction type.
    \tparam Increment The decayed type for the increment.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Direction, class Increment, class Range,
        class Enable>
    struct drop
    : try_all <
        drop_by_drop_one <RangeTag, Direction, Increment, Range>,
        drop_by_drop_constant <RangeTag, Direction, Increment, Range>,
        member_access::drop <Direction, Increment, Range>
    > {/*
        ... operator() (Direction const & direction,
            Increment const & increment, Range && range) const;
    */};

    /** \brief
    Return the range without the first \c Increment elements from the direction
    \c Direction, using \c chop if necessary and possible.

    The only reason to specialise this would be disable this and derive it from
    \c unimplemented even when \c chop is implemented.
    */
    template <class RangeTag, class Direction, class Increment, class Range>
    struct drop_automatic
    : try_all <
        drop <RangeTag, Direction, Increment, Range>,
        drop_by_chop <RangeTag, Direction, Increment, Range>> {};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct drop;
} // namespace apply

namespace callable {
    struct drop : generic <apply::drop> {};
} // namespace callable

static const auto drop = callable::drop();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Increments, class Ranges,
            class Enable = void>
        struct drop : operation::unimplemented {};

        // All arguments filled in: forward to operation::drop_automatic.
        template <class Direction, class Increment, class Range>
            struct drop <meta::vector <Direction>,
                meta::vector <Increment>, meta::vector <Range>>
        : operation::drop_automatic <typename tag_of <Range>::type,
            typename std::decay <Direction>::type,
            typename std::decay <Increment>::type, Range &&> {};

        // No Increment: insert one_type().
        using operation::one_type;

        template <class Direction, class Range>
            struct drop <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>, typename boost::enable_if <
                    operation::is_implemented <drop <meta::vector <Direction>,
                        meta::vector <one_type>, meta::vector <Range>>>
                >::type>
        {
            drop <meta::vector <Direction>, meta::vector <one_type>,
                meta::vector <Range>> implementation;

            auto operator() (Direction const & direction, Range && range) const
            RETURNS (implementation (
                direction, one_type(), std::forward <Range> (range)));
        };

    } // namespace automatic_arguments

    template <class ... Arguments> struct drop
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::drop, meta::vector <Arguments ...>>::type {};

} // namespace apply

} // namespace range

#endif  // RANGE_DETAIL_CORE_DROP_HPP_INCLUDED
