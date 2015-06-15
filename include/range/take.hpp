/*
Copyright 2013-2015 Rogier van Dalen.

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

#ifndef RANGE_TAKE_HPP_INCLUDED
#define RANGE_TAKE_HPP_INCLUDED

#include <type_traits>
#include <utility>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/and.hpp>

#include "utility/overload_order.hpp"

#include "rime/core.hpp"
#include "rime/merge_types.hpp"
#include "rime/assert.hpp"
#include "rime/sign.hpp"
#include "rime/cast.hpp"
#include "rime/min.hpp"
#include "rime/call_if.hpp"

#include "core.hpp"
#include "helper/underlying.hpp"

namespace range {

/*
There are two aspects of the below code.
First, the logic that allows "take" to use drop (reverse (direction), n, range).
Second, if that is impossible, take_range, which wraps a view and restricts it
to a number of elements on the fly.
*/

/**
View of a range that cuts it off after a number of elements.
*/
template <class Underlying, class Limit, class Direction> class take_range;

template <class Underlying, class Limit, class Direction> inline
auto make_take_range (Underlying && underlying,
    Limit const & limit, Direction const & direction)
RETURNS (take_range <typename std::decay <Underlying>::type, Limit, Direction> (
    std::forward <Underlying> (underlying), limit, direction));

/* take_range, which limits the underlying range. */

template <class Underlying, class Limit, class Direction> class take_range {
public:
    static_assert (range::is_view <Underlying, Direction>::value,
        "Underlying range must be a view in Direction");

    typedef Underlying underlying_type;
    typedef Limit limit_type;
    typedef Direction direction_type;

private:
    Underlying underlying_;
    Limit limit_;
    Direction direction_;

    template <class Wrapper> friend class helper::callable::get_underlying;

public:
    template <class CVUnderlying>
    take_range (CVUnderlying && underlying, Limit const & limit,
        Direction const & direction)
    : underlying_ (std::forward <CVUnderlying> (underlying)),
        limit_ (limit), direction_ (direction) {}

    /// \brief Return unqualified limit.
    Limit limit() const { return limit_; }

    Direction const & direction() const { return direction_; }

private:
    friend class helper::member_access;

    auto default_direction() const
    RETURNS (range::default_direction (underlying_));

    auto empty (Direction const & direction) const
    RETURNS (rime::or_ (
        limit_ == rime::make_zero (limit_),
        range::empty (underlying_, direction)));

    template <class Underlying2 = Underlying, class Limit2 = Limit,
        class Result = decltype (range::chop_in_place (
            std::declval <Underlying2 &>(), std::declval <Direction>())),
        class Enable = typename std::enable_if <
            !rime::is_constant <Limit2>::value>::type>
    Result chop_in_place (Direction const & direction) {
        auto && result = range::chop_in_place (underlying_, direction);
        -- limit_;
        return static_cast <decltype (result)> (result);
    }
};

namespace operation {
    struct take_range_tag {};
} // namespace operation

template <class Underlying, class Limit, class Direction>
    struct tag_of_qualified <take_range <Direction, Limit, Underlying>>
{ typedef operation::take_range_tag type; };

namespace take_detail {

    /**
    Merge policy for types to do something sensible when a limits and sizes
    are of different types, e.g. int and unsigned, or constants.
    */
    struct limit_merge_policy
    : rime::merge_policy::constant <rime::merge_policy::common_type> {};

    typedef rime::callable::min <limit_merge_policy> min;

} // namespace take_detail

namespace helper {

    void implement_take (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_take;

        /* The default implementation for "take". */

        struct implement_take_default {
        private:
            struct when_not_take_range {
                /* Implementation that just calls drop (back, ..., range). */
                // If size (range) <= limit: return range.
                // Range2 is Range but with an rvalue reference removed.
                template <class Range2, class Limit, class Direction,
                    class Size = typename result_of <
                        callable::size (Range2 const &, Direction)>::type,
                    class Enable2 = typename std::enable_if <
                        (Size::value <= Limit::value)>::type>
                Range2 operator() (Range2 && range,
                    Limit const & limit, Direction const & direction,
                    overload_order <1> *) const
                { return std::forward <Range2> (range); }

                // Otherwise: return drop (back, size - limit, range).
                // (Unless size <= limit; then return drop (back, 0, range).)
                // Range2 is necessary to delay the evaluation of result_of.
                template <class Range2, class Limit, class Direction,
                    class Size = typename result_of <
                        callable::size (Range2 const &, Direction)>::type,
                    class Difference = decltype (std::declval <Size>() -
                        take_detail::min() (
                            std::declval <Limit>(), std::declval <Size>())),
                    class Result = decltype (range::drop (
                        std::declval <Range2>(), std::declval <Difference>(),
                        direction::opposite (std::declval <Direction>())))>
                Result operator() (Range2 && range,
                    Limit const & limit, Direction const & direction,
                    overload_order <2> *) const
                {
                    auto size = range::size (range, direction);
                    return range::drop (std::forward <Range2> (range),
                        size - take_detail::min() (limit, size),
                        direction::opposite (direction));
                }

                /* Standard implementation. */
                template <class Range, class Limit, class Direction>
                auto operator() (Range && range, Limit const & limit,
                    Direction const & direction, overload_order <3> *)
                    const
                RETURNS (make_take_range (
                    std::forward <Range> (range), limit, direction));
            };

            /// Check that the limit is non-negative.
            // GCC 4.6 does not accept this if it is a static member function.
            struct check_limit_non_negative {
                template <class Limit>
                    Limit operator() (Limit const & limit) const
                {
                    rime::assert_ (
                        !rime::less_sign_safe (limit, rime::size_t <0>()));
                    return limit;
                }
            };

            struct when_take_range {
                template <class Underlying, class OriginalLimit, class Limit,
                    class Direction>
                auto operator() (
                    take_range <Underlying, OriginalLimit, Direction> const &
                        range,
                    Limit const & limit, Direction const & direction,
                    overload_order <1> *) const
                RETURNS (make_take_range (
                    range::helper::get_underlying <decltype (range)> (range),
                    rime::min_<take_detail::limit_merge_policy> (
                        limit, range.limit()),
                    direction));
            };

            struct is_take_range_with_same_direction {
                template <class Range2, class Direction>
                    rime::false_type operator() (
                        Range2 const &, Direction const &) const
                { return rime::false_; }

                template <class Underlying, class OriginalLimit,
                    class Direction>
                auto operator() (
                    take_range <Underlying, OriginalLimit, Direction>
                        const & range,
                    Direction const & direction) const
                RETURNS (direction == range.direction());
            };

        public:
            // Limit is unqualified; Range is a view.
            template <class Range, class Limit, class Direction>
            auto operator() (Range && range,
                Limit const & limit, Direction const & direction) const
            RETURNS (rime::call_if (
                is_take_range_with_same_direction() (range, direction),
                when_take_range(), when_not_take_range(),
                std::forward <Range> (range),
                check_limit_non_negative() (limit), direction,
                pick_overload()));
        };

        /* The main implementation of "take". */

        struct take {
        private:
            struct dispatch {
                // Call implement_take().
                template <class Range, class Limit, class Direction>
                    auto operator() (
                        Range && range, Limit const & limit,
                        Direction const & direction,
                        overload_order <1> *) const
                RETURNS (implement_take (typename tag_of <Range>::type(),
                    std::forward <Range> (range), limit, direction));

                // Default implementation
                template <class Range, class Limit, class Direction>
                    auto operator() (
                        Range && range, Limit const & limit,
                        Direction const & direction,
                        overload_order <2> *) const
                RETURNS (implement_take_default() (
                    std::forward <Range> (range), limit, direction));
            };

        public:
            // With direction and limit.
            template <class Range, class Limit, class Direction,
                class Enable = typename
                    std::enable_if <is_direction <Direction>::value>::type>
            auto operator() (
                Range && range, Limit const & limit,
                Direction const & direction) const
            RETURNS (dispatch() (
                range::view (std::forward <Range> (range), direction),
                limit, direction, pick_overload()));

            // With limit but without direction: use default direction.
            template <class Range, class Limit, class Enable =
                typename std::enable_if <
                    is_range <Range>::value && !is_direction <Limit>::value
                >::type>
            auto operator() (Range && range, Limit const & limit) const
            RETURNS (dispatch() (
                range::view (std::forward <Range> (range),
                    range::default_direction (range)),
                limit, range::default_direction (range), pick_overload()));
        };

    } // namespace implementation

    using implementation::take;

} // namespace callable

/**
Return a number of elements from a range, as a range.

The default direction is the same as the underlying range.
If take() is called with a different direction than the default direction, the
default direction will therefore often not actually work.

Calling take with the same direction multiple times will not lead
to a recursive type.
If size (direction, range) and drop (reverse (direction), n, range) are
available, then the latter is used directly.
take() will therefore often return exactly the same type as \c range.

\param range The range to take the elements from.
\param number The number of elements to take.
    If the underlying range has fewer elements, only those elements are taken.
\param direction (Optional) The direction to take the elements from.
*/
static auto const take = callable::take();


/* Operations on take_range. */
// These require take to be defined, so they are implemented down here.

namespace operation {

    template <class TakeRange, class Direction>
        inline auto implement_size (take_range_tag const &,
            TakeRange && r, Direction const & direction)
    RETURNS (rime::min_ <take_detail::limit_merge_policy> (
        r.limit(), range::size (range::helper::get_underlying <TakeRange> (r),
            direction)));

    template <class TakeRange, class Direction>
        inline auto implement_first (take_range_tag const &,
            TakeRange && r, Direction const & direction)
    -> decltype (range::first (range::helper::get_underlying <TakeRange> (r),
        direction))
    {
        rime::assert_ (direction == r.direction());
        rime::assert_ (!range::empty (r, direction));
        return range::first (range::helper::get_underlying <TakeRange> (r),
            direction);
    }

    template <class TakeRange, class Increment, class Direction>
        inline auto implement_drop (take_range_tag const &,
            TakeRange && r, Increment const & increment,
            Direction const & direction)
    -> decltype (range::take (
        range::drop (range::helper::get_underlying <TakeRange> (
                std::declval <TakeRange &>()),
            increment, direction),
        rime::cast_value <typename std::decay <TakeRange>::type::limit_type> (
            r.limit() - increment),
        direction))
    {
        rime::assert_ (direction == r.direction());
        rime::assert_ (!rime::less_sign_safe (
            increment, rime::size_t <0>()));
        rime::assert_ (!rime::less_sign_safe (r.limit(), increment));
        typedef typename std::decay <TakeRange>::type::limit_type
            limit_type;
        return range::take (
            range::drop (range::helper::get_underlying <TakeRange> (r),
                increment, direction),
            rime::cast_value <limit_type> (r.limit() - increment), direction);
    }

    // chop: enable only if this is implemented on the underlying range.
    // Otherwise, the generic implementation that uses "first" and "drop" should
    // work on the take_range, not on the underlying range.

    /*
    Dear reader: apologies for the following code.
    It calls "chop" on the underlying range, which returns a chopped<> with
    the first element (to be returned as-is) and the rest (to be wrapped in
    a new take_range).
    The best way to take this in is to read the body of the function, and
    only then convince yourself that the types make sense.
    */
    template <class TakeRange, class Direction,
        class Limit = typename std::decay <TakeRange>::type::limit_type,
        class NewLimit = decltype (
            rime::cast_value <Limit> (Limit()
                - callable::implementation::one_type())),
        class Underlying = typename helper::underlying_type <TakeRange>::type,
        class FirstAndUnderlyingRest = decltype (callable::chop_direct() (
            std::declval <Underlying>(), std::declval <Direction>(),
            pick_overload())),
        class NewTakeRange = take_range <
            typename FirstAndUnderlyingRest::rest_type, NewLimit, Direction>,
        class Result = chopped <
            typename FirstAndUnderlyingRest::first_type, NewTakeRange>>
    inline Result implement_chop (take_range_tag const &,
        TakeRange && r, Direction const & direction)
    {
        callable::implementation::one_type one;
        rime::assert_ (direction == r.direction());
        rime::assert_ (!rime::less_sign_safe (r.limit(), one));
        auto first_and_underlying_rest = callable::chop_direct() (
            range::helper::get_underlying <TakeRange> (r), direction,
            pick_overload());
        return Result (first_and_underlying_rest.forward_first(),
            range::take (first_and_underlying_rest.move_rest(),
                rime::cast_value <Limit> (r.limit() - one), direction));
    }

} // namespace operation

} // namespace range

#endif // RANGE_TAKE_HPP_INCLUDED
