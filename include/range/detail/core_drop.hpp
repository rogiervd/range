/*
Copyright 2011-2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_DROP_HPP_INCLUDED
#define RANGE_DETAIL_CORE_DROP_HPP_INCLUDED

#include <type_traits>

#include "utility/overload_order.hpp"

#include "rime/core.hpp"

#include "utility/returns.hpp"

#include "core_base.hpp"

namespace range {

namespace helper {

    /** \brief
    Return the range without the first \c increment elements from the direction
    \c direction.

    The return type and value should be the same as the return type and value
    of \c increment calls to <c>drop (range, one_type(), direction)</c>.

    To implement drop(), implement one of the member functions \c drop_one,
    \c drop_constant, or \c drop, or one of the free functions
    \c implement_drop_one, \c implement_drop_constant, or \c implement_drop.

    If multiple of these are defined, then \c drop_one will be preferred if
    applicable, then \c drop_constant, and finally \c drop, and free functions
    over member functions.

    If this is not implemented, but operation::chop is implemented, then drop
    of one element is automatically implemented in terms of \c chop.
    This only works for the qualification that \c chop is implemented for.

    \param tag The range tag.
    \param range The range itself.
    \param increment The number of elements to remove.
    \param direction The direction that the elements should be removed from.
    */
    void implement_drop (unusable);

    /* \brief
    Remove a constant number of elements from the range.

    \param tag The range tag.
    \param range The range itself.
    \param increment The number of elements to remove.
    \param direction The direction that the elements should be removed from.
    */
    void implement_drop_constant (unusable);

    /* \brief
    Remove one element from the range.

    This is not passed \c increment.

    \param tag The range tag.
    \param range The range itself.
    \param direction The direction that the element should be removed from.
    */
    void implement_drop_one (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_drop_one;
        using helper::implement_drop_constant;
        using helper::implement_drop;

        typedef rime::size_t <1> one_type;

        /** \brief
        Implement "drop" only by calling the direct implementation, not through
        "chop"

        Compared to drop itself, this requires an additional argument
        \c pick_overload() to be passed in, and no argument is optional.

        \param range
        \param increment
        \param direction
        \param overload_order
        */
        struct drop_direct {
            // If \c increment is one: call drop_one().
            template <class Range, class Increment, class Direction,
                class Enable = typename std::enable_if <
                    rime::equal_constant <Increment, one_type>::value
                >::type>
                auto operator() (
                    Range && range, Increment const &,
                    Direction const & direction,
                    overload_order <1> *) const
            RETURNS (implement_drop_one (
                typename tag_of <Range>::type(),
                std::forward <Range> (range), direction));

            // Forward to member if possible.
            template <class Range, class Increment, class Direction,
                class Enable = typename std::enable_if <
                    rime::equal_constant <Increment, one_type>::value
                >::type>
                auto operator() (
                    Range && range, Increment const &,
                    Direction const & direction,
                    overload_order <2> *) const
            RETURNS (helper::member_access::drop_one (
                std::forward <Range> (range), direction));

            // If \c increment is constant: call drop_constant().
            template <class Range, class Increment, class Direction,
                class Enable = typename std::enable_if <
                    rime::is_constant <Increment>::value>::type>
                auto operator() (
                    Range && range, Increment const & increment,
                    Direction const & direction,
                    overload_order <3> *) const
            RETURNS (implement_drop_constant (
                typename tag_of <Range>::type(),
                std::forward <Range> (range), increment, direction));

            // Forward to member if possible.
            template <class Range, class Increment, class Direction,
                class Enable = typename std::enable_if <
                    rime::is_constant <Increment>::value>::type>
                auto operator() (
                    Range && range, Increment const & increment,
                    Direction const & direction,
                    overload_order <4> *) const
            RETURNS (helper::member_access::drop_constant (
                std::forward <Range> (range), increment, direction));

            // Call drop().
            template <class Range, class Increment, class Direction>
                auto operator() (
                    Range && range, Increment const & increment,
                    Direction const & direction,
                    overload_order <5> *) const
            RETURNS (implement_drop (typename tag_of <Range>::type(),
                std::forward <Range> (range), increment, direction));

            // Forward to member if possible.
            template <class Range, class Increment, class Direction>
                auto operator() (
                    Range && range, Increment const & increment,
                    Direction const & direction,
                    overload_order <6> *) const
            RETURNS (helper::member_access::drop (
                std::forward <Range> (range), increment, direction));
        };

        struct drop {
        private:
            struct dispatch : drop_direct {
                using drop_direct::operator();

                // Additional, indirect implementation: forward to chop.
                template <class Range, class Increment, class Direction,
                    class Enable = typename std::enable_if <
                        rime::is_constant <Increment>::value
                        && Increment::value == 1
                    >::type>
                    auto operator() (
                        Range && range, Increment const &,
                        Direction const & direction,
                        overload_order <7> *) const
                RETURNS (implement_chop (typename tag_of <Range>::type(),
                    std::forward <Range> (range), direction).forward_rest());

                template <class Range, class Increment, class Direction,
                    class Enable = typename std::enable_if <
                        rime::is_constant <Increment>::value
                        && Increment::value == 1
                    >::type>
                    auto operator() (
                        Range && range, Increment const &,
                        Direction const & direction,
                        overload_order <8> *) const
                RETURNS (helper::member_access::chop (
                    std::forward <Range> (range), direction).forward_rest());
            };

        public:
            // With direction and increment.
            template <class Range, class Increment, class Direction,
                class Enable = typename
                    std::enable_if <is_direction <Direction>::value>::type>
            auto operator() (Range && range, Increment const & increment,
                Direction const & direction) const
            RETURNS (dispatch() (
                std::forward <Range> (range), increment, direction,
                pick_overload()));

            // With increment but without direction: use default direction.
            template <class Range, class Increment, class Enable =
                typename std::enable_if <
                    is_range <Range>::value && !is_direction <Increment>::value
                >::type>
            auto operator() (Range && range, Increment const & increment) const
            RETURNS (dispatch() (
                std::forward <Range> (range), increment,
                range::default_direction (range),
                pick_overload()));

            // Without increment but with direction: use one_type().
            template <class Range, class Direction, class Enable = typename
                std::enable_if <is_direction <Direction>::value>::type>
            auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (dispatch() (
                std::forward <Range> (range), one_type(), direction,
                pick_overload()));

            // Without increment or direction: use one_type() and
            // default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range && range) const
            RETURNS (dispatch() (
                std::forward <Range> (range), one_type(),
                range::default_direction (range),
                pick_overload()));
        };

    } // namespace implementation

    using implementation::drop_direct;
    using implementation::drop;

} // namespace callable

/** \brief
Return the range without its first elements.

\param range
    The range to operate on.
\param increment
    The number of elements.
\param direction
    (optional) The direction from which the elements will be taken.
*/
static const auto drop = callable::drop();

} // namespace range

#endif  // RANGE_DETAIL_CORE_DROP_HPP_INCLUDED
