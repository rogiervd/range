/*
Copyright 2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_MEMBER_ACCESS_HPP_INCLUDED
#define RANGE_DETAIL_CORE_MEMBER_ACCESS_HPP_INCLUDED

namespace range { namespace helper {

    class member_access {
    public:

#if defined (__clang__) || !defined (__GNUC__) || __GNUC__ > 4 || \
    (__GNUC__ == 4 && __GNUC_MINOR__ > 6)

        // default_direction.
        template <class Range>
            static auto default_direction (Range && range)
        RETURNS (std::forward <Range> (range).default_direction());

        // empty.
        template <class Range, class Direction>
            static auto empty (Range && range, Direction const & direction)
        RETURNS (std::forward <Range> (range).empty (direction));

        // size.
        template <class Range, class Direction>
            static auto size (Range && range, Direction const & direction)
        RETURNS (std::forward <Range> (range).size (direction));

        // first.
        template <class Range, class Direction>
            static auto first (Range && range, Direction const & direction)
        RETURNS (std::forward <Range> (range).first (direction));

        // at.
        template <class Range, class Position, class Direction>
            static auto at_constant (Range && range,
                Position const & position, Direction const & direction)
        RETURNS (std::forward <Range> (range)
            .at_constant (position, direction));

        template <class Range, class Position, class Direction>
            static auto at (Range && range,
                Position const & position, Direction const & direction)
        RETURNS (std::forward <Range> (range).at (position, direction));

        // drop.
        template <class Range, class Direction>
            static auto drop_one (Range && range, Direction const & direction)
        RETURNS (std::forward <Range> (range).drop_one (direction));

        template <class Range, class Increment, class Direction>
            static auto drop_constant (Range && range,
                Increment const & increment, Direction const & direction)
        RETURNS (std::forward <Range> (range)
            .drop_constant (increment, direction));

        template <class Range, class Increment, class Direction>
            static auto drop (Range && range,
                Increment const & increment, Direction const & direction)
        RETURNS (std::forward <Range> (range)
            .drop (increment, direction));

        // chop.
        template <class Range, class Direction>
            static auto chop (Range && range, Direction const & direction)
        RETURNS (std::forward <Range> (range).chop (direction));

        // chop_in_place.
        template <class Range, class Direction>
            static auto chop_in_place (
                Range && range, Direction const & direction)
        RETURNS (std::forward <Range> (range).chop_in_place (direction));

        // fold.
        template <class State, class Range, class Direction, class Function>
            static auto fold (State && state, Range && range,
                Direction const & direction, Function && function)
        RETURNS (std::forward <Range> (range).fold (
            std::forward <State> (state), direction,
            std::forward <Function> (function)));

        // for_each.
        template <class Range, class Direction, class Function>
            static auto for_each (Range && range,
                Direction const & direction, Function && function)
        RETURNS (std::forward <Range> (range).for_each (
            direction, std::forward <Function> (function)));
    };

#else

        // Implementation for GCC 4.6, which refuses to mangle the above
        // static member functions.

        struct callable_default_direction {
            template <class Range> auto operator() (Range && range) const
            RETURNS (std::forward <Range> (range).default_direction());
        };

        static callable_default_direction const default_direction;

        struct callable_empty {
            template <class Range, class Direction>
                auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (std::forward <Range> (range).empty (direction));
        };

        static callable_empty const empty;

        struct callable_size {
            template <class Range, class Direction>
                auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (std::forward <Range> (range).size (direction));
        };

        static callable_size const size;

        struct callable_first {
            template <class Range, class Direction>
                auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (std::forward <Range> (range).first (direction));
        };

        static callable_first const first;

        // at.
        struct callable_at_constant {
            template <class Range, class Position, class Direction>
                auto operator() (Range && range, Position const & position,
                    Direction const & direction) const
            RETURNS (std::forward <Range> (range)
                .at_constant (position, direction));
        };

        static callable_at_constant const at_constant;

        struct callable_at {
            template <class Range, class Position, class Direction>
                auto operator() (Range && range, Position const & position,
                    Direction const & direction) const
            RETURNS (std::forward <Range> (range).at (position, direction));
        };

        static callable_at const at;

        // drop.
        struct callable_drop_one {
            template <class Range, class Direction>
                auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (std::forward <Range> (range).drop_one (direction));
        };

        static callable_drop_one const drop_one;

        struct callable_drop_constant {
            template <class Range, class Increment, class Direction>
                auto operator() (Range && range, Increment const & increment,
                    Direction const & direction) const
            RETURNS (std::forward <Range> (range)
                .drop_constant (increment, direction));
        };

        static callable_drop_constant const drop_constant;

        struct callable_drop {
            template <class Range, class Increment, class Direction>
                auto operator() (Range && range, Increment const & increment,
                    Direction const & direction) const
            RETURNS (std::forward <Range> (range).drop (increment, direction));
        };

        static callable_drop const drop;

        // chop.
        struct callable_chop {
            template <class Range, class Direction>
                auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (std::forward <Range> (range).chop (direction));
        };

        static callable_chop const chop;

        // chop_in_place
        struct callable_chop_in_place {
            template <class Range, class Direction>
                auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (std::forward <Range> (range).chop_in_place (direction));
        };

        static callable_chop_in_place const chop_in_place;

        // fold.
        struct callable_fold {
            template <class State, class Range, class Direction, class Function>
                auto operator() (State && state,
                    Range && range, Direction const & direction,
                    Function && function) const
            RETURNS (std::forward <Range> (range).fold (
                std::forward <State> (state), direction,
                std::forward <Function> (function)));
        };

        static callable_fold const fold;

        // for_each.
        struct callable_for_each {
            template <class Range, class Direction, class Function>
                auto operator() (Range && range, Direction const & direction,
                    Function && function) const
            RETURNS (std::forward <Range> (range).for_each (
                direction, std::forward <Function> (function)));
        };

        static callable_for_each const for_each;
    };

    member_access::callable_default_direction const
        member_access::default_direction
        = member_access::callable_default_direction();

    member_access::callable_empty const member_access::empty
        = member_access::callable_empty();
    member_access::callable_size const member_access::size
        = member_access::callable_size();
    member_access::callable_first const member_access::first
        = member_access::callable_first();
    member_access::callable_at const member_access::at_constant
        = member_access::callable_at_constant();
    member_access::callable_at const member_access::at
        = member_access::callable_at();
    member_access::callable_drop_one const member_access::drop_one
        = member_access::callable_drop_one();
    member_access::callable_drop_constant const member_access::drop_constant
        = member_access::callable_drop_constant();
    member_access::callable_drop const member_access::drop
        = member_access::callable_drop();
    member_access::callable_chop const member_access::chop
        = member_access::callable_chop();
    member_access::callable_chop_in_place const member_access::chop_in_place
        = member_access::callable_chop_in_place();
    member_access::callable_fold const member_access::fold
        = member_access::callable_fold();
    member_access::callable_for_each const member_access::for_each
        = member_access::callable_for_each();

#endif

}} // namespace range::helper

#endif // RANGE_DETAIL_CORE_MEMBER_ACCESS_HPP_INCLUDED
