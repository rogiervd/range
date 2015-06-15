/*
Copyright 2011, 2012, 2015 Rogier van Dalen.

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

#include "utility/returns.hpp"
#include "utility/overload_order.hpp"

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

    using callable_traits::has;
    using callable_traits::result_of;

    namespace helper {

        struct unusable;

        /** \brief
        Convert direction into a forward direction.

        Provide this for a direction that is not a forward direction.
        make_forward should not be implemented for the result type.
        */
        void implement_make_forward (unusable);

        /** \brief
        Convert direction into a backward direction.

        Specialise this for all forward directions.
        Otherwise, this automatically forwards to make_forward (which may well
        be unimplemented).
        */
        void implement_opposite (unusable);

    } // namespace helper

    namespace callable {

        namespace implementation {

            using helper::implement_make_forward;
            using helper::implement_opposite;

            struct make_forward {
                template <class Direction, class Enable = typename
                    std::enable_if <is_direction <Direction>::value>::type>
                auto operator() (Direction const & direction) const
                RETURNS (implement_make_forward (direction));
            };

            struct opposite {
                // If the direction is a forward direction.
                template <class Direction, class Enable = typename
                    std::enable_if <is_direction <Direction>::value>::type>
                auto operator() (Direction const & direction) const
                RETURNS (implement_opposite (direction));

                // If the direction is a backward direction.
                template <class Direction, class Enable = typename
                    std::enable_if <is_direction <Direction>::value>::type>
                auto operator() (Direction const & direction) const
                RETURNS (implement_make_forward (direction));
            };

            struct ensure_forward {
            private:
                struct dispatch {
                    template <class Direction>
                        auto operator() (Direction const & direction,
                            utility::overload_order <1> *) const
                    RETURNS (implement_make_forward (direction));

                    template <class Direction>
                        Direction operator() (Direction const & direction,
                            utility::overload_order <2> *) const
                    { return direction; }
                };

            public:
                template <class Direction, class Enable = typename
                    std::enable_if <is_direction <Direction>::value>::type>
                auto operator() (Direction const & direction) const
                RETURNS (dispatch() (direction, utility::pick_overload()));
            };

        } // namespace implementation

        using implementation::opposite;
        using implementation::make_forward;
        using implementation::ensure_forward;

    } // namespace callable

    /**
    \return The opposite of direction.
    */
    static const auto opposite = callable::opposite();

    /**
    \return The forward equivalent of a backward direction.
    */
    static const auto make_forward = callable::make_forward();

    /**
    \return The opposite of the direction if it is backward.
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

    inline back implement_opposite (front) { return back(); }

    inline front implement_make_forward (back) { return front(); }

} // namespace direction

#endif  // RANGE_DIRECTION_HPP_INCLUDED
