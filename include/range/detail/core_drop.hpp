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

#ifndef RANGE_DETAIL_CORE_DROP_HPP_INCLUDED
#define RANGE_DETAIL_CORE_DROP_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "rime/core.hpp"

#include "utility/returns.hpp"

namespace range {

namespace operation {

    template <class RangeTag, class Direction, class Increment,
        class Enable = void>
    struct drop;

    template <class RangeTag, class Direction, class Increment,
        class Enable = void>
    struct drop_constant;

    template <class RangeTag, class Direction, class Enable = void>
        struct drop_one;

    typedef rime::size_t <1> one_type;

    /**
    Convenience operation.
    Can be specialised if "drop" is only available for an increment of one.
    */
    template <class RangeTag, class Direction, class Enable>
    struct drop_one : unimplemented
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Increment const & increment, Range && range) const;
    */};

    /**
    Convenience operation.
    Can be specialised if "drop" is only available for increments known at
    compile time.
    */
    template <class RangeTag, class Direction, class Increment, class Enable>
    struct drop_constant
    : boost::mpl::if_ <rime::equal_constant <Increment, one_type>,
        drop_one <RangeTag, Direction>, unimplemented
    >::type
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Increment const & increment, Range && range) const;
    */};

    /**
    Return the range without the first "Increment" elements from the direction
    "DirectionTag".
    The return type and value should be the same as the return type and value
    of "Increment" calls to drop <DirectionTag, one_type>.
    */
    template <class RangeTag, class Direction, class Increment, class Enable>
    struct drop
    : boost::mpl::if_ <rime::is_constant <Increment>,
        drop_constant <RangeTag, Direction, Increment>, unimplemented
    >::type
    {/*
        template <class Range>
            ... operator() (Direction const & direction,
                Increment const & increment, Range && range) const;
    */};

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

        // All arguments filled in: forward to operation::drop.
        template <class Direction, class Increment, class Range>
            struct drop <meta::vector <Direction>,
                meta::vector <Increment>, meta::vector <Range>>
        : operation::drop <typename tag_of <Range>::type,
            Direction, typename std::decay <Increment>::type> {};

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

