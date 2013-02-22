/*
Copyright 2011, 2012 Rogier van Dalen.

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

/** \file
Directions of iteration through ranges.
This extend the directions declared in ../direction/tag.hpp to run-time
sequences.
*/

#ifndef RANGE_DIRECTION_HPP_INCLUDED
#define RANGE_DIRECTION_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/not.hpp>

#include "rime/core.hpp"

#include "direction/tag.hpp"

/*
Directions are often, but not always, tags.
They should be small enough to pass around.
(A sensible optimisation deals with empty directions.)
Directions are assumed immutable by operations on ranges, and are passed by
const reference.

Backward directions can be converted into a corresponding forward direction.
Reversible directions can be converted into the opposite direction.
Backward directions are by definition reversible.
*/

namespace range {
    using direction::is_direction;
} // namespace range

namespace direction {
    namespace has {
        template <class Direction> struct reverse;
        template <class Direction> struct make_forward;
        template <class Direction> struct ensure_forward;
    } // namespace has

    namespace operation {

        struct unimplemented {};

        template <class Operation> struct is_implemented
        : boost::mpl::not_ <std::is_base_of <unimplemented, Operation>> {};

        /**
        Convert direction into a forward direction.
        Specialise this for a direction that is not a forward direction.
        make_forward should not be implemented for the result type.
        */
        template <class Direction, class Enable = void> struct make_forward
        : unimplemented {/*
            ... operator() (Direction const & direction) const;
        */};

        /**
        Specialise this for all forward directions.
        Otherwise, this automatically forwards to make_forward (which may well
        be unimplemented).
        */
        template <class Direction, class Enable = void>
            struct reverse
        : make_forward <Direction> {/*
            ... operator() (Direction const & direction) const;
        */};

        template <class Direction> struct pass_through {
            Direction operator() (Direction const & direction) const
            { return direction; }
        };

        /**
        Convert Direction with "make_forward" if available.
        Otherwise, return Direction.
        This does not have to be specialised.
        */
        template <class Direction> struct ensure_forward
        : boost::mpl::eval_if <is_direction <Direction>,
            boost::mpl::if_ <has::make_forward <Direction>,
                make_forward <Direction>,
                pass_through <Direction>>,
            boost::mpl::identity <unimplemented>
        >::type {};

    } // namespace operation

    // Normalise arguments.
    namespace apply {

        template <class Direction> struct reverse
        : operation::reverse <typename std::decay <Direction>::type> {};

        template <class Direction> struct make_forward
        : operation::make_forward <typename std::decay <Direction>::type> {};

        template <class Direction> struct ensure_forward
        : operation::ensure_forward <typename std::decay <Direction>::type> {};

    } // namespace apply

    namespace has {

        template <class Direction> struct reverse
        : operation::is_implemented <apply::reverse <Direction>> {};

        template <class Direction> struct make_forward
        : operation::is_implemented <apply::make_forward <Direction>> {};

        template <class Direction> struct ensure_forward
        : operation::is_implemented <apply::ensure_forward <Direction>> {};

    } // namespace has

    namespace result_of {

        template <class Direction, class Enable = void> struct make_forward;
        template <class Direction>
            struct make_forward <Direction, typename boost::enable_if <
                has::make_forward <Direction>>::type>
        : std::decay <decltype (apply::make_forward <Direction>() (
            std::declval <Direction>()))> {};

        template <class Direction, class Enable = void> struct reverse;
        template <class Direction>
            struct reverse <Direction, typename boost::enable_if <
                has::reverse <Direction>>::type>
        : std::decay <decltype (apply::reverse <Direction>() (
            std::declval <Direction>()))> {};

        template <class Direction, class Enable = void> struct ensure_forward;
        template <class Direction>
            struct ensure_forward <Direction, typename boost::enable_if <
                is_direction <Direction>>::type>
        : std::decay <decltype (apply::ensure_forward <Direction>() (
            std::declval <Direction>()))> {};

    } // namespace result_of

    /**
    \return The reverse of direction.
    */
    template <class Direction>
        auto reverse (Direction const & direction)
        -> decltype (apply::reverse <Direction>() (direction))
    { return apply::reverse <Direction>() (direction); }

    /**
    \return The forward equivalent of a backward direction.
    */
    template <class Direction> auto make_forward (Direction const & direction)
        -> decltype (apply::make_forward <Direction>() (direction))
    { return apply::make_forward <Direction>() (direction); }

    /**
    \return The reverse of the direction if it is backward.
        Otherwise, return the argument.
    */
    template <class Direction> auto ensure_forward (Direction const & direction)
        -> decltype (apply::ensure_forward <Direction>() (direction))
    { return apply::ensure_forward <Direction>() (direction); }

} // namespace direction

/*
Predefine standard directions "front" and "back".
They are not included in namespace range (unlike is_direction, and unlike
namespace meta) because range::front and range::back are static variables with
types direction::front and direction::back, respectively.
(These are defined in range/core.hpp.)
*/
namespace direction {

    struct front {
        rime::bool_ <true> operator == (front const &) const
        { return rime::true_; }
    };
    struct back {
        rime::bool_ <true> operator == (back const &) const
        { return rime::true_; }
    };

    namespace operation {

        template <> struct reverse <front>
        { back operator() (front const &) const { return back(); } };

        template <> struct make_forward <back>
        { front operator() (back const &) const { return front(); } };

    } // namespace operation

} // namespace direction

#endif  // RANGE_DIRECTION_HPP_INCLUDED

