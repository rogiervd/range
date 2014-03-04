/*
Copyright 2013 Rogier van Dalen.

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

#ifndef RANGE_FIND_HPP_INCLUDED
#define RANGE_FIND_HPP_INCLUDED

#include <utility>
#include <type_traits>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include "utility/returns.hpp"
#include "rime/call_if.hpp"

#include "core.hpp"

namespace range {

/* Interface. */

namespace operation {

    namespace find_detail {
        struct identity {
            template <typename Range> Range operator() (Range range) const
            { return std::move (range); }
        };
    } // namespace find_detail

    /**
    All arguments are qualified.
    */
    template <class Predicate, class NonEmptyActor, class EmptyActor>
        struct find;

} // namespace operation

namespace apply {

    namespace automatic_arguments {

        // find.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct find : operation::unimplemented {};

        // Two actors passed in.
        template <class Direction, class Predicate,
            class NonEmptyActor, class EmptyActor, class Range>
        struct find <meta::vector <Direction>,
            meta::vector <Predicate, NonEmptyActor, EmptyActor>,
            meta::vector <Range>>
        : operation::find <Predicate, NonEmptyActor, EmptyActor> {};

        // One actor passed in: use it for both actors.
        template <class Direction, class Predicate, class Actor, class Range>
        struct find <meta::vector <Direction>,
            meta::vector <Predicate, Actor>, meta::vector <Range>>
        {
            find <meta::vector <Direction>,
                meta::vector <Predicate, Actor, Actor>,
                meta::vector <Range>> implementation;

            auto operator() (Direction const & direction,
                Predicate && predicate, Actor && actor, Range && range) const
            RETURNS (implementation (direction,
                std::forward <Predicate> (predicate),
                std::forward <Actor> (actor), std::forward <Actor> (actor),
                std::forward <Range> (range)))
        };

        // No actor passed in: use find_detail::identity.
        template <class Direction, class Predicate, class Range>
        struct find <meta::vector <Direction>,
            meta::vector <Predicate>, meta::vector <Range>>
        {
            find <meta::vector <Direction>,
                meta::vector <Predicate,
                    operation::find_detail::identity const &,
                    operation::find_detail::identity const &>,
                meta::vector <Range>> implementation;

            auto operator() (Direction const & direction,
                Predicate && predicate, Range && range) const
            RETURNS (implementation (direction,
                std::forward <Predicate> (predicate),
                operation::find_detail::identity(),
                operation::find_detail::identity(),
                std::forward <Range> (range)))
        };

    } // namespace automatic_arguments

    /** find */
    template <class ... Arguments> struct find
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view <automatic_arguments::find>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

namespace callable {
    struct find : generic <apply::find> {};
} // namespace callable

/**
Find the first element in a range that for which a predicate is true, or the
empty range if the predicate is not true for any element.
    find (predicate, range)
is roughly defined as
    if (empty (range) || predicate (first (range)))
        return range;
    else
        return find (predicate, drop (range));

The complete parameter list is
    find ([direction,] predicate, [actor_non_empty, [actor_empty,]], range).

The additional first parameter is the direction.
The actors are functions that are called with the range starting with the first
element for which the predicate is true.
If no actor is given, the range is returned.
If one actor is given, it is called on the range and its return value called.
If two actors are given, the first one is called if the range is non-empty and
the second if the range is empty.
The function then implements
    if (empty (direction, range))
        return actor_empty (range);
    else if (predicate (first (direction, range)))
        return actor_non_empty (range);
    else
        return find (direction, predicate, actor_non_empty, actor_empty,
            drop (direction, range));

The recursion is implemented as an iteration if the range is homogeneous, to
prevent stack overflows.

The first evaluation of the predicate to return true is the last to be
evaluated.

The first evaluation of the predicate to return a true compile-time constant is
the last to be instantiated.
*/
static const auto find = callable::find();

/* Implementation */

namespace operation {

    namespace find_detail {

        template <class Predicate, class NonEmptyActor, class EmptyActor>
            struct finder;
        template <class Predicate, class NonEmptyActor, class EmptyActor>
            struct when_homogeneous;
        template <class Predicate, class NonEmptyActor, class EmptyActor>
            struct when_heterogeneous;

        /**
        Store the predicate and the actors so they can be re-used during the
        recursion.
        This must be mutable so the predicate and actors, rvalue references to
        which are stored, are mutable.
        */
        template <class Predicate, class NonEmptyActor, class EmptyActor>
            struct finder
        {
            Predicate && predicate;
            NonEmptyActor && non_empty_actor;
            EmptyActor && empty_actor;

            typedef when_homogeneous <Predicate, NonEmptyActor, EmptyActor>
                when_homogeneous_;
            typedef when_heterogeneous <Predicate, NonEmptyActor, EmptyActor>
                when_heterogeneous_;

        public:
            finder (Predicate && predicate,
                NonEmptyActor && non_empty_actor, EmptyActor && empty_actor)
            : predicate (std::forward <Predicate> (predicate)),
            non_empty_actor (std::forward <NonEmptyActor> (non_empty_actor)),
            empty_actor (std::forward <EmptyActor> (empty_actor)) {}

            template <class Direction, class Range>
                auto operator() (Direction const & direction, Range && range)
            -> decltype (rime::call_if (range::is_homogeneous <Range>(),
                std::declval <when_homogeneous_>(),
                std::declval <when_heterogeneous_>(),
                std::declval <finder &>(),
                direction, std::forward <Range> (range)))
            {
                return rime::call_if (range::is_homogeneous <Range>(),
                    when_homogeneous_(), when_heterogeneous_(),
                    *this, direction, std::forward <Range> (range));
            }
        };

        /**
        Iterative implementation of "find" for homogeneous ranges.
        */
        template <class Predicate, class NonEmptyActor, class EmptyActor>
            struct when_homogeneous
        {
            typedef finder <Predicate, NonEmptyActor, EmptyActor> finder_;

            /**
            \return whether the predicate returns compile-time false.
            If this is true, the resulting range will always be empty.
            */
            template <class Direction, class Range> struct always_empty {
                typedef typename result_of <Predicate (
                        callable::first (Direction, Range))>::type
                    predicate_result;

                static const bool value = boost::mpl::and_ <
                        rime::is_constant <predicate_result>,
                        boost::mpl::not_ <predicate_result>
                    >::value;
            };

            // Normal implementation.
            template <class Direction, class Range>
                typename boost::lazy_disable_if <
                    always_empty <Direction, Range>,
                    typename rime::make_variant_over <meta::vector <
                        typename std::result_of <NonEmptyActor (Range)>::type,
                        typename std::result_of <EmptyActor (Range)>::type
                    >>>::type
            operator() (finder_ & f, Direction const & direction, Range range)
                const
            {
                // "range" is used as the local variable.
                while (!range::empty (direction, range)) {
                    if (f.predicate (range::first (direction, range)))
                        return f.non_empty_actor (std::move (range));
                    range = range::drop (direction, std::move (range));
                }
                return f.empty_actor (std::move (range));
            }

            // Implementation for compile-time true predicates.
            // This makes a difference for the return type.
            template <class Direction, class Range>
                typename boost::lazy_enable_if <
                    always_empty <Direction, Range>,
                    std::result_of <EmptyActor (Range)>
                >::type
            operator() (finder_ & f, Direction const & direction, Range range)
                const
            {
                // "range" is used as the local variable.
                while (!range::empty (direction, range)) {
                    // Even though it will return compile-time false, for
                    // consistency, it is called.
                    f.predicate (range::first (direction, range));
                    range = range::drop (direction, std::move (range));
                }
                return f.empty_actor (std::move (range));
            }
        };

        /**
        Recursive implementation of "find" for heterogeneous ranges.
        */
        template <class Predicate, class NonEmptyActor, class EmptyActor>
            struct when_heterogeneous
        {
            typedef finder <Predicate, NonEmptyActor, EmptyActor> finder_;

            // \pre empty (range).
            struct when_empty {
                template <class Direction, class Range>
                    auto operator() (finder_ & f,
                        Direction const &, Range && range) const
                RETURNS (f.empty_actor (std::forward <Range> (range)))
            };

            // \pre !empty (range) && predicate (first (range)).
            struct when_found {
                template <class Direction, class Range>
                    auto operator() (finder_ & f,
                        Direction const &, Range && range) const
                RETURNS (f.non_empty_actor (std::forward <Range> (range)))
            };

            // \pre !empty (range) || !predicate (first (range)).
            struct when_not_done {
                template <class Direction, class Range>
                    auto operator() (finder_ & f,
                        Direction const & direction, Range && range) const
                -> decltype (std::declval <finder_>() (direction,
                    range::drop (direction, std::declval <Range>())))
                {
                    // Recurse.
                    return f (direction,
                        range::drop (direction, std::forward <Range> (range)));
                }
            };

            // \pre !empty (range).
            struct when_not_empty {
                template <class Direction, class Range>
                    auto operator() (finder_ & f,
                        Direction const & direction, Range && range) const
                RETURNS (rime::call_if (
                    f.predicate (range::first (direction, range)),
                    when_found(), when_not_done(),
                    f, direction, std::forward <Range> (range)))
            };

            // Entry point.
            template <class Direction, class Range>
                auto operator() (finder_ & f, Direction const & direction,
                    Range && range) const
            RETURNS (rime::call_if (range::empty (direction, range),
                when_empty(), when_not_empty(),
                f, direction, std::forward <Range> (range)))
        };

    } // namespace find_detail

    /**
    Main implementation.
    Forward to a newly constructed mutable find_detail::finder <...>.
    */
    template <class Predicate, class NonEmptyActor, class EmptyActor>
        struct find
    {
        typedef find_detail::finder <Predicate, NonEmptyActor, EmptyActor>
            finder_type;

        template <class Direction, class Range> struct result
        : result_of <finder_type (Direction const &, Range)>
        {};

        template <class Direction, class Range>
            typename result <Direction, Range>::type
            operator() (Direction const & direction,
                Predicate && predicate, NonEmptyActor && non_empty_actor,
                EmptyActor && empty_actor, Range && range) const
        {
            // If no actor is given, it must be "identity".
            finder_type finder (std::forward <Predicate> (predicate),
                std::forward <NonEmptyActor> (non_empty_actor),
                std::forward <EmptyActor> (empty_actor));
            return finder (direction, std::forward <Range> (range));
        }
    };

} // namespace operation

} // namespace range

#endif // RANGE_FIND_HPP_INCLUDED

