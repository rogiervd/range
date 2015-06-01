/*
Copyright 2011, 2012 Rogier van Dalen.

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
#include "detail/callable_traits.hpp"

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

    namespace callable {
        struct reverse;
        struct make_forward;
        struct ensure_forward;
    } // namespace callable

    using callable_traits::has;
    using callable_traits::result_of;
    using callable_traits::result_of_or;

    namespace operation {

        using ::callable_traits::unimplemented;
        using ::callable_traits::is_implemented;

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
            boost::mpl::if_ <has <callable::make_forward (Direction)>,
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

    namespace callable {

        using ::callable_traits::generic;

        struct reverse : generic <apply::reverse> {};
        struct make_forward : generic <apply::make_forward> {};
        struct ensure_forward : generic <apply::ensure_forward> {};

    } // namespace callable

    /**
    \return The reverse of direction.
    */
    static const auto reverse = callable::reverse();

    /**
    \return The forward equivalent of a backward direction.
    */
    static const auto make_forward = callable::make_forward();

    /**
    \return The reverse of the direction if it is backward.
        Otherwise, return the argument.
    */
    static const auto ensure_forward = callable::ensure_forward();

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
        rime::true_type operator == (front const &) const
        { return rime::true_; }

        template <class OtherDirection>
            rime::false_type operator == (OtherDirection const &) const
        { return rime::false_; }
    };
    struct back {
        rime::true_type operator == (back const &) const
        { return rime::true_; }

        template <class OtherDirection>
            rime::false_type operator == (OtherDirection const &) const
        { return rime::false_; }
    };

    namespace operation {

        template <> struct reverse <front>
        { back operator() (front const &) const { return back(); } };

        template <> struct make_forward <back>
        { front operator() (back const &) const { return front(); } };

    } // namespace operation

} // namespace direction

#endif  // RANGE_DIRECTION_HPP_INCLUDED

