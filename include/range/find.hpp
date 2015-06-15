/*
Copyright 2013, 2015 Rogier van Dalen.

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

namespace find_detail {
    struct identity {
        template <typename Range> Range operator() (Range range) const
        { return std::move (range); }
    };
} // namespace find_detail

/* Implementation */
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

        template <class Range, class Direction>
            auto operator() (Range && range, Direction const & direction)
        -> decltype (rime::call_if (range::is_homogeneous <Range>(),
            std::declval <when_homogeneous_>(),
            std::declval <when_heterogeneous_>(),
            std::declval <finder &>(),
            std::forward <Range> (range), direction))
        {
            return rime::call_if (range::is_homogeneous <Range>(),
                when_homogeneous_(), when_heterogeneous_(),
                *this, std::forward <Range> (range), direction);
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
        template <class Range, class Direction> struct always_empty {
            typedef decltype (std::declval <Predicate>() (
                    range::first (
                        std::declval <Range>(), std::declval <Direction>())))
                predicate_result;

            static const bool value = boost::mpl::and_ <
                    rime::is_constant <predicate_result>,
                    boost::mpl::not_ <predicate_result>
                >::value;
        };

        // Normal implementation.
        template <class Range, class Direction>
            typename boost::lazy_disable_if <
                always_empty <Range, Direction>,
                typename rime::make_variant_over <meta::vector <
                    typename result_of <NonEmptyActor (Range)>::type,
                    typename result_of <EmptyActor (Range)>::type
                >>>::type
        operator() (finder_ & f, Range range, Direction const & direction)
            const
        {
            // "range" is used as the local variable.
            while (!range::empty (range, direction)) {
                if (f.predicate (range::first (range, direction)))
                    return f.non_empty_actor (std::move (range));
                range = range::drop (std::move (range), direction);
            }
            return f.empty_actor (std::move (range));
        }

        // Implementation for compile-time true predicates.
        // This makes a difference for the return type.
        template <class Range, class Direction>
            typename boost::lazy_enable_if <
                always_empty <Range, Direction>, result_of <EmptyActor (Range)>
            >::type
        operator() (finder_ & f, Range range, Direction const & direction)
            const
        {
            // "range" is used as the local variable.
            while (!range::empty (range, direction)) {
                // Even though it will return compile-time false, for
                // consistency, it is called.
                f.predicate (range::first (range, direction));
                range = range::drop (std::move (range), direction);
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
            template <class Range, class Direction>
                auto operator() (finder_ & f, Range && range, Direction const &)
                    const
            RETURNS (f.empty_actor (std::forward <Range> (range)));
        };

        // \pre !empty (range) && predicate (first (range)).
        struct when_found {
            template <class Range, class Direction>
                auto operator() (finder_ & f, Range && range, Direction const &)
                    const
            RETURNS (f.non_empty_actor (std::forward <Range> (range)));
        };

        // \pre !empty (range) || !predicate (first (range)).
        struct when_not_done {
            template <class Range, class Direction>
                auto operator() (finder_ & f,
                    Range && range, Direction const & direction) const
            -> decltype (std::declval <finder_>() (
                range::drop (std::declval <Range>(), direction), direction))
            {
                // Recurse.
                return f (range::drop (std::forward <Range> (range), direction),
                    direction);
            }
        };

        // \pre !empty (range).
        struct when_not_empty {
            template <class Range, class Direction>
                auto operator() (finder_ & f,
                    Range && range, Direction const & direction) const
            RETURNS (rime::call_if (
                f.predicate (range::first (range, direction)),
                when_found(), when_not_done(),
                f, std::forward <Range> (range), direction));
        };

        // Entry point.
        template <class Range, class Direction>
            auto operator() (finder_ & f,
                Range && range, Direction const & direction) const
        RETURNS (rime::call_if (range::empty (range, direction),
            when_empty(), when_not_empty(),
            f, std::forward <Range> (range), direction));
    };

} // namespace find_detail

/**
Main implementation.
Forward to a newly constructed mutable find_detail::finder <...>.
*/
/*template <class Predicate, class NonEmptyActor, class EmptyActor>
    struct find
{
    typedef find_detail::finder <Predicate, NonEmptyActor, EmptyActor>
        finder_type;

    template <class Range, class Direction> struct result
    : result_of <finder_type (Direction const &, Range)>
    {};

    template <class Range, class Direction>
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
};*/

/* Interface. */

namespace callable {

    class find {
    private:
        struct dispatch {
            // All arguments.
            template <class Range, class Direction,
                class Predicate, class NonEmptyActor, class EmptyActor>
            auto operator() (Range && range, Direction const & direction,
                Predicate && predicate, NonEmptyActor && non_empty_actor,
                    EmptyActor && empty_actor) const
            RETURNS (
                find_detail::finder <Predicate, NonEmptyActor, EmptyActor> (
                        std::forward <Predicate> (predicate),
                        std::forward <NonEmptyActor> (non_empty_actor),
                        std::forward <EmptyActor> (empty_actor)
                    ) (
                        std::forward <Range> (range), direction));

            // No empty_actor.
            template <class Range, class Direction,
                class Predicate, class Actor>
            auto operator() (Range && range, Direction const & direction,
                Predicate && predicate, Actor && actor) const
            RETURNS (
                find_detail::finder <Predicate, Actor, Actor> (
                        std::forward <Predicate> (predicate),
                        std::forward <Actor> (actor),
                        std::forward <Actor> (actor)
                    ) (std::forward <Range> (range), direction));

            // No actors.
            template <class Range, class Direction, class Predicate>
            auto operator() (Range && range, Direction const & direction,
                Predicate && predicate) const
            RETURNS (
                find_detail::finder <Predicate,
                    find_detail::identity, find_detail::identity> (
                        std::forward <Predicate> (predicate),
                        find_detail::identity(), find_detail::identity()
                    ) (
                        std::forward <Range> (range), direction));
        };

    public:
        // With direction.
        template <class Range, class Direction, class ... Rest,
            class Enable = typename std::enable_if <
                is_direction <Direction>::value>::type>
        auto operator() (Range && range, Direction const & direction,
            Rest && ... rest) const
        RETURNS (dispatch() (
            range::view (std::forward <Range> (range), direction),
            direction, std::forward <Rest> (rest) ...));

        // No direction: use default_direction.
        template <class Range, class Argument, class ... Rest,
            class Enable = typename std::enable_if <
                !is_direction <Argument>::value>::type>
        auto operator() (Range && range, Argument && argument, Rest && ... rest)
            const
        RETURNS (dispatch() (
            range::view (std::forward <Range> (range)),
            range::default_direction (range),
            std::forward <Argument> (argument),
            std::forward <Rest> (rest) ...));
    };

} // namespace callable

/** \brief
Find the first element in a range that for which a predicate is \c true, or the
empty range if the predicate is not true for any element.

<c>find (predicate, range)</c>
is roughly defined as
    if (empty (range) || predicate (first (range)))
        return range;
    else
        return find (predicate, drop (range));

The complete parameter list is
    find (range, [direction,] predicate, [actor_non_empty, [actor_empty]]).

The additional first parameter is the direction.
The actors are functions that are called with the range starting with the first
element for which the predicate is true.
If no actor is given, the range is returned.
If one actor is given, it is called on the range and its return value called.
If two actors are given, the first one is called if the range is non-empty and
the second if the range is empty.
The function then implements
    if (empty (range, direction))
        return actor_empty (range);
    else if (predicate (first (range, direction)))
        return actor_non_empty (range);
    else
        return find (drop (range, direction), direction,
            predicate, actor_non_empty, actor_empty);

The recursion is implemented as an iteration if the range is homogeneous, to
prevent stack overflows.

The first evaluation of the predicate to return true is the last to be
evaluated.

The first evaluation of the predicate to return a true compile-time constant is
the last to be instantiated.

\param range
    The range to find an element in.
\param direction
    (optional) The direction to traverse the range in.
\param predicate
    The function that is called with every element until it returns \c true.
\param actor_non_empty
    (optional)
    The function that is called with the remaining range that starts with the
    first element that \a predicate returns \c true for.
    By default, the range itself is returned.
\param actor_empty
    (optional)
    The function that is called with the empty range if \c predicate is not
    \c true for any element.
    By default, \a actor_non_empty is used.
*/
static const auto find = callable::find();

} // namespace range

#endif // RANGE_FIND_HPP_INCLUDED
