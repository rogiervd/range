/*
Copyright 2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_MEMBER_ACCESS_HPP_INCLUDED
#define RANGE_DETAIL_CORE_MEMBER_ACCESS_HPP_INCLUDED

#include <type_traits>

#include "callable_traits.hpp"

#include "core_tag.hpp"
#include "core_base.hpp"

namespace range { namespace operation {

    /**
    Class through which access to members is channeled.
    Ranges which define operations through member functions can make them
    private and make friends with this class.

    All member structs derived from \c unimplemented unless the appropriate
    member function can be called.
    */
    class member_access {
    public:
        /**
        Implement "default_direction" by calling the <c>.default_direction()</c>
        member function.
        */
        template <class Range, class Enable = void> struct default_direction
        : unimplemented {};

        template <class Range>
            struct default_direction <Range, typename enable_if_member <
                decltype (std::declval <Range>().default_direction())>::type>
        {
            auto operator() (Range && range) const
            RETURNS (std::forward <Range> (range).default_direction());
        };

        /**
        Implement "empty" by calling the <c>.empty (Direction)</c> member
        function.
        */
        template <class Direction, class Range, class Enable = void>
            struct empty
        : unimplemented {};

        template <class Direction, class Range>
            struct empty <Direction, Range, typename enable_if_member <
                decltype (std::declval <Range>().empty (
                    std::declval <Direction const &>()))>::type>
        {
            auto operator() (Direction const & direction, Range && range) const
            RETURNS (std::forward <Range> (range).empty (direction));
        };

        /**
        Implement "first" by calling the <c>.first (Direction)</c> member
        function.
        */
        template <class Direction, class Range, class Enable = void>
            struct first
        : unimplemented {};

        template <class Direction, class Range>
            struct first <Direction, Range, typename enable_if_member <
                decltype (std::declval <Range>().first (
                    std::declval <Direction const &>()))>::type>
        {
            auto operator() (Direction const & direction, Range && range) const
            RETURNS (std::forward <Range> (range).first (direction));
        };

        /**
        Implement "size" by calling the <c>.size (Direction)</c> member
        function.
        */
        template <class Direction, class Range, class Enable = void>
            struct size
        : unimplemented {};

        template <class Direction, class Range>
            struct size <Direction, Range, typename enable_if_member <
                decltype (std::declval <Range>().size (
                    std::declval <Direction const &>()))>::type>
        {
            auto operator() (Direction const & direction, Range && range) const
            RETURNS (std::forward <Range> (range).size (direction));
        };

        /**
        Implement "drop_one" by calling the <c>.drop_one (Direction)</c> member
        function.
        */
        template <class Direction, class Range, class Enable = void>
            struct drop_one
        : unimplemented {};

        template <class Direction, class Range>
            struct drop_one <Direction, Range, typename enable_if_member <
                decltype (std::declval <Range>().drop_one (
                    std::declval <Direction const &>()))>::type>
        {
            template <class One>
                auto operator() (Direction const & direction, One const &,
                    Range && range) const
            RETURNS (std::forward <Range> (range).drop_one (direction));
        };

        /**
        Implement "drop_constant" by calling the
        <c>.drop_constant (Direction, Increment)</c> member function.
        */
        template <class Direction, class Increment, class Range,
                class Enable = void>
            struct drop_constant
        : unimplemented {};

        template <class Direction, class Increment, class Range>
            struct drop_constant <Direction, Increment, Range,
                typename enable_if_member <decltype (std::declval <Range>()
                    .drop_constant (std::declval <Direction const &>(),
                        std::declval <Increment const &>()))>::type>
        {
            auto operator() (Direction const & direction,
                Increment const & increment, Range && range) const
            RETURNS (std::forward <Range> (range)
                .drop_constant (direction, increment));
        };

        /**
        Implement "drop" by calling the <c>.drop (Direction, Increment)</c>
        member function.
        */
        template <class Direction, class Increment, class Range,
                class Enable = void>
            struct drop
        : unimplemented {};

        template <class Direction, class Increment, class Range>
            struct drop <Direction, Increment, Range,
                typename enable_if_member <decltype (std::declval <Range>()
                    .drop (std::declval <Direction const &>(),
                        std::declval <Increment const &>()))>::type>
        {
            auto operator() (Direction const & direction,
                Increment const & increment, Range && range) const
            RETURNS (std::forward <Range> (range).drop (direction, increment));
        };

        /**
        Implement "chop" by calling the <c>.chop (Direction)</c> member
        function.
        */
        template <class Direction, class Range, class Enable = void> struct chop
        : unimplemented {};

        template <class Direction, class Range>
            struct chop <Direction, Range, typename enable_if_member <
                decltype (std::declval <Range>().chop (
                    std::declval <Direction const &>()))>::type>
        {
            template <class One> auto operator() (
                Direction const & direction, One const &, Range && range) const
            RETURNS (std::forward <Range> (range).chop (direction));
        };

        /**
        Implement "chop" by calling the <c>.chop (Direction)</c> member
        function.
        */
        template <class Direction, class Range, class Enable = void>
            struct chop_in_place
        : unimplemented {};

        // Only for lvalue reference ranges.
        // (They can be const references, though.)
        template <class Direction, class Range>
            struct chop_in_place <Direction, Range &, typename
                enable_if_member <decltype (std::declval <Range>()
                    .chop_in_place (std::declval <Direction const &>()))>::type>
        {
            auto operator() (Direction const & direction, Range & range) const
            RETURNS (range.chop_in_place (direction));
        };
    };

}} // namespace range::operation

#endif  // RANGE_DETAIL_CORE_MEMBER_ACCESS_HPP_INCLUDED