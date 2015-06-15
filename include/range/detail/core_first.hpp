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

#ifndef RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED
#define RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED

#include <type_traits>

#include "utility/overload_order.hpp"
#include "utility/returns.hpp"

#include "rime/core.hpp"

#include "core_base.hpp"

namespace range {

namespace helper {

    /** \brief
    Return the first element in the range.

    This normally should be implemented (by providing the member function or by
    specialising this) for any range.

    If this is not implemented, but operation::chop is implemented, then "first"
    is automatically implemented in terms of \c chop.
    This only works for the qualification that \c chop is implemented for.

    \param tag The range tag.
    \param direction The direction.
    \param range The range.
    */
    void implement_first (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_first;
        using helper::implement_chop;

        /** \brief
        Implement "first" only by calling the direct implementation, not through
        "chop"

        Compared to first itself, this requires an additional argument
        \c pick_overload() to be passed in, and no argument is optional.

        \param range
        \param direction
        \param overload_order
        */
        struct first_direct {
            template <class Range, class Direction>
                auto operator() (
                    Range && range, Direction const & direction,
                    overload_order <1> *) const
            RETURNS (implement_first (typename tag_of <Range>::type(),
                std::forward <Range> (range), direction));

            // Forward to member if possible.
            template <class Range, class Direction>
                auto operator() (
                    Range && range, Direction const & direction,
                    overload_order <2> *) const
            RETURNS (helper::member_access::first (
                std::forward <Range> (range), direction));
        };

        struct first {
        private:
            struct dispatch : first_direct {
                using first_direct::operator();

                // Additional overloads.

                // Forward to chop if possible.
                template <class Range, class Direction>
                    auto operator() (
                        Range && range, Direction const & direction,
                        overload_order <3> *) const
                RETURNS (implement_chop (typename tag_of <Range>::type(),
                    std::forward <Range> (range), direction).forward_first());

                template <class Range, class Direction>
                    auto operator() (
                        Range && range, Direction const & direction,
                        overload_order <4> *) const
                RETURNS (helper::member_access::chop (
                    std::forward <Range> (range), direction).forward_first());
            };

        public:
            // With direction.
            template <class Range, class Direction, class Enable = typename
                std::enable_if <is_direction <Direction>::value>::type>
            auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (dispatch() (std::forward <Range> (range), direction,
                pick_overload()));

            // Without direction: use default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range && range) const
            RETURNS (dispatch() (
                std::forward <Range> (range), range::default_direction (range),
                pick_overload()));
        };

    } // namespace implementation

    using implementation::first_direct;
    using implementation::first;

} // namespace callable

/** \brief
Return the first element from a range.

\param range
    The range to operate on.
\param direction
    (optional) The direction from which the first element will be taken.
*/
static const auto first = callable::first();

} // namespace range

#endif  // RANGE_DETAIL_CORE_FIRST_HPP_INCLUDED
