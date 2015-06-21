/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_CHOP_IN_PLACE_HPP_INCLUDED
#define RANGE_DETAIL_CORE_CHOP_IN_PLACE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include "meta/vector.hpp"

#include "utility/storage.hpp"

#include "core_base.hpp"

namespace range {

namespace helper {

    /** \brief
    Return the first element of the range, and replace the range in place by
    the range without the first element.

    This can only be used for homogeneous ranges.

    If this is not implemented, a default implementation is provided if either
    \a first and \a drop are implemented, or if \a chop is implemented.

    \param tag The range tag.
    \param range The range.
    \param direction The direction.
    */
    void implement_chop_in_place (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_chop_in_place;

        /** \brief
        Call implementation for range::chop_in_place, but do not synthesise
        implementations.

        This requires an extra argument \c pick_overload().

        \param range
        \param direction
        \param overload_order
        */
        struct chop_in_place_direct {
            template <class Range, class Direction>
                auto operator() (Range & range, Direction const & direction,
                    overload_order <1> *) const
            RETURNS (implement_chop_in_place (typename tag_of <Range>::type(),
                range, direction));

            // Forward to member if possible.
            template <class Range, class Direction>
                auto operator() (Range & range, Direction const & direction,
                    overload_order <2> *) const
            RETURNS (helper::member_access::chop_in_place (range, direction));
        };

        struct chop_in_place {
        private:
            struct dispatch : public chop_in_place_direct {
                using chop_in_place_direct::operator();

                // Use "first" and "drop".
                // Only enabled if "drop" returns a range of the same type.
                template <class Range, class Direction,
                    class Result = decltype (std::declval <first_direct>() (
                        std::declval <Range &>(), std::declval <Direction>(),
                        pick_overload())),
                    class Enable = typename std::enable_if <std::is_same <
                        typename std::decay <Range>::type,
                        typename std::decay <decltype (
                            std::declval <drop_direct>() (
                                std::declval <Range>(), one_type(),
                                std::declval <Direction>(), pick_overload()))
                        >::type
                    >::value>::type>
                Result operator() (Range & range, Direction const & direction,
                    overload_order <3> *) const
                {
                    Result element = first_direct() (range, direction,
                        pick_overload());
                    range = drop_direct() (std::move (range),
                        one_type(), direction, pick_overload());
                    return static_cast <Result &&> (element);
                }

                // Use "chop".
                // Only enabled if "drop" returns a range of the same type.
                // This is less preferred than using "first" and "drop", since
                // this is usually slightly slower.
                template <class Range, class Direction,
                    class Result = decltype (std::declval <chop_direct>() (
                        std::declval <Range &&>(), std::declval <Direction>(),
                        pick_overload()).forward_first()),
                    class Enable = typename std::enable_if <std::is_same <
                        typename std::decay <Range>::type,
                        typename std::decay <decltype (
                            std::declval <chop_direct>() (
                                std::declval <Range>(),
                                std::declval <Direction>(), pick_overload()
                            ).forward_rest())>::type
                        >::value>::type>
                Result operator() (Range & range, Direction const & direction,
                    overload_order <4> *) const
                {
                    auto chopped = chop_direct() (
                        std::move (range), direction, pick_overload());
                    range = chopped.move_rest();
                    return chopped.move_first();
                }
            };

        public:
            // With direction.
            template <class Range, class Direction, class Enable = typename
                std::enable_if <is_range <Range>::value
                    && !std::is_const <Range>::value
                    && is_direction <Direction>::value>::type>
            auto operator() (Range & range, Direction const & direction)
                const
            RETURNS (dispatch() (range, direction, pick_overload()));

            // Without direction: use default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range & range) const
            RETURNS (dispatch() (
                range, range::default_direction (range), pick_overload()));
        };

    } // namespace implementation

    using implementation::chop_in_place_direct;
    using implementation::chop_in_place;

} // namespace callable

/**
Return the first element of the range, and remove this element from the range
itself.

\param direction
    (optional) The direction from which the first element is taken.
\param range
    The range to operate on.
    It must be homogeneous, so that without the first element it has the same
    type.
    This is normally required to be a reference.
*/
static const auto chop_in_place = callable::chop_in_place();

namespace helper {

    /** \brief
    Implement chop by using chop_in_place.

    It is often straightforward to implement chop_in_place() for a range.
    If move construction is cheap, then chop() can be implemented in terms of
    chop_in_place.
    To do so, implement_chop should forward to this.

    For example,
    \code
    template <class MyRange>
    inline auto implement_chop (my_range_tag const & tag,
        MyRange && range, direction::front const & direction)
    RETURNS (helper::chop_by_chop_in_place (
        tag, std::move (range), direction, pick_overload()));
    \endcode
    */
    template <class Range, class Direction,
        class DecayedRange = typename std::decay <Range>::type,
        class First = decltype (
            callable::chop_in_place_direct() (
                std::declval <DecayedRange &>(), std::declval <Direction>(),
                pick_overload())),
        class Result = chopped <First, DecayedRange>,
        class Enable = typename std::enable_if <
            !std::is_reference <Range>::value>::type>
    inline Result chop_by_chop_in_place (
        Range && range, Direction const & direction)
    {
        auto new_range = std::move (range);
        auto && first = callable::chop_in_place_direct() (
            new_range, direction, pick_overload());
        return Result (static_cast <First &&> (first), std::move (new_range));
    }

} // namespace helper

} // namespace range

#endif  // RANGE_DETAIL_CORE_CHOP_IN_PLACE_HPP_INCLUDED
