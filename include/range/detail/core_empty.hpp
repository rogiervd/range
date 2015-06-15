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

#ifndef RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED
#define RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED

#include <type_traits>

#include "utility/overload_order.hpp"

#include "rime/core.hpp"

#include "core_base.hpp"

namespace range {

namespace helper {

    /** \brief
    Return \c true if there are no elements in the range.

    This needs to be implemented (by providing the member function or by
    implementing this free function) for any range, but only for the forward
    direction.
    For example, by defining it for \c direction::front, it will automatically
    also be defined for \c direction::back.

    \param tag The range tag.
    \param range
        The range itself, qualified (as an rvalue reference if an rvalue).
    \param direction The direction.
    */
    void implement_empty (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_empty;

        struct empty {
        private:
            struct dispatch {
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <1> *) const
                RETURNS (implement_empty (typename tag_of <Range>::type(),
                    range, direction));

                // Forward to member if possible.
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <2> *) const
                RETURNS (helper::member_access::empty (
                    range, direction));

                // Use direction::make_forward in case "implement_empty" is only
                // provided for the forward direction.
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <3> *) const
                RETURNS (implement_empty (typename tag_of <Range>::type(),
                    range, direction::make_forward (direction)));

                // Member with make_forward.
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <4> *) const
                RETURNS (helper::member_access::empty (
                    range, direction::make_forward (direction)));
            };

        public:
            // With direction.
            template <class Range, class Direction, class Enable = typename
                std::enable_if <is_range <Range>::value
                    && is_direction <Direction>::value>::type>
            auto operator() (Range const & range, Direction const & direction)
                const
            RETURNS (dispatch() (range, direction, pick_overload()));

            // Without direction: use default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range const & range) const
            RETURNS (dispatch() (
                range, range::default_direction (range), pick_overload()));
        };

    } // namespace implementation

    using implementation::empty;

} // namespace callable

/** \brief
Return whether the range is empty in a direction.

\param range
    The range to operate on.
\param direction
    (optional) The direction.
*/
static const auto empty = callable::empty();

/**
Evaluate to \c true iff the range is known at compile time to be empty.
This happens when <c>empty (range, direction)</c> returns a compile-time
constant with value true.

If this evaluates to \c false, it is still possible for the range to be empty at
run time.
*/
template <class Range, class Direction> struct always_empty
: rime::equal_constant <
    decltype (empty (std::declval <Range>(), std::declval <Direction>())),
    rime::true_type> {};

/**
Evaluate to \c true iff the range is known at compile time to be not empty.
This happens when <c>empty (range, direction)</c> returns a compile-time
constant with value false.

If this evaluates to \c false, it is still possible for the range to be
non-empty at run time.
*/
template <class Range, class Direction> struct never_empty
: rime::equal_constant <
    decltype (empty (std::declval <Range>(), std::declval <Direction>())),
    rime::false_type> {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_EMPTY_HPP_INCLUDED
