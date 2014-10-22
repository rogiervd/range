/*
Copyright 2013, 2014 Rogier van Dalen.

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
#include "detail/underlying.hpp"

namespace range {

namespace apply {
    template <class ... Arguments> struct take;
} // namespace apply

namespace callable {
    struct take : generic <apply::take> {};
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

\param direction (Optional) The direction to take the elements from.
\param number The number of elements to take.
    If the underlying range has fewer elements, only those elements are taken.
\param range The range to take the elements from.
*/
static auto const take = callable::take();

/*
There are two aspects of the below code.
First, the logic that allows "take" to use drop (reverse (direction), n, range).
Second, if that is impossible, take_range, which wraps a view and restricts it
to a number of elements on the fly.
*/

/**
View of a range that cuts it off after a number of elements.
*/
template <class Direction, class Limit, class Underlying> class take_range;

template <class Direction, class Limit, class Underlying> inline
auto make_take_range (
    Direction const & direction, Limit const & limit, Underlying && underlying)
RETURNS (take_range <Direction, Limit, typename
        decayed_result_of <callable::view (Underlying)>::type> (
            direction, limit, std::forward <Underlying> (underlying)));

/* The actual operation "take". */

namespace operation {

    namespace take_detail {

        /**
        Merge policy for types to do something sensible when a limits and sizes
        are of different types, e.g. int and unsigned, or constants.
        */
        struct limit_merge_policy
        : rime::merge_policy::constant <rime::merge_policy::common_type> {};

        typedef rime::callable::min <limit_merge_policy> min;

    } // namespace take_detail

    template <class RangeTag, class Direction, class Limit, class Enable = void>
        struct take
    {
    private:
        struct when_not_take_range {
            /* Implementation that just calls drop (back, ..., range). */
            // If size (range) <= limit: return range.
            template <class Range,
                class Size = typename result_of_or <
                    callable::size (Direction, Range const &)>::type,
                class Enable2 = typename boost::enable_if <
                    rime::equal_constant <
                        decltype (rime::less_sign_safe (
                            std::declval <Limit>(), std::declval <Size>())),
                        boost::mpl::false_>>::type>
            Range operator() (Direction const & direction,
                Limit const & limit, Range && range,
                utility::overload_order <1> *)
            { return std::forward <Range> (range); }

            // Otherwise: return drop (back, size - limit, range).
            // (Well, unless size <= limit; then )
            template <class Range,
                class Size = typename
                    result_of_or <callable::size (Range const &)>::type,
                class Difference = decltype (std::declval <Size>() -
                    take_detail::min() (
                        std::declval <Limit>(), std::declval <Size>())),
                class Result = typename result_of_or <
                    callable::drop (direction::callable::reverse (Direction),
                    Difference, Range)>::type>
            Result operator() (Direction const & direction,
                Limit const & limit, Range && range,
                utility::overload_order <2> *)
            {
                auto size = range::size (direction, range);
                return range::drop (direction::reverse (direction),
                    size - take_detail::min() (limit, size),
                    std::forward <Range> (range));
            }

            /* Standard implementation. */
            template <class Range> auto operator() (
                Direction const & direction, Limit const & limit,
                Range && range, utility::overload_order <3> *)
            RETURNS (make_take_range (
                direction, limit, std::forward <Range> (range)));
        };

        /// Check that the limit is non-negative.
        // GCC 4.6 does not accept this if it is a static member function.
        struct check_limit_non_negative {
            Limit operator() (Limit const & limit) const {
                rime::assert_ (
                    !rime::less_sign_safe (limit, rime::size_t <0>()));
                return limit;
            }
        };

        struct when_take_range {
            template <class OriginalLimit, class Underlying>
            auto operator() (Direction const & direction, Limit const & limit,
                take_range <Direction, OriginalLimit, Underlying> const & range,
                utility::overload_order <1> *) const
            RETURNS (make_take_range (direction,
                rime::min_ <take_detail::limit_merge_policy> (
                    limit, range.limit()),
                range::detail::get_underlying (range)));
        };

        struct is_take_range_with_same_direction {
            template <class Range> rime::false_type operator() (
                Direction const &, Range const &)
            { return rime::false_; }

            template <class OriginalLimit, class Underlying>
                auto operator() (Direction const & direction,
                take_range <Direction, OriginalLimit, Underlying> const & range)
            RETURNS (direction == range.direction());
        };

    public:
        // Limit is unqualified; Range is a view.
        template <class Range> auto operator() (Direction const & direction,
            Limit const & limit, Range && range) const
        RETURNS (rime::call_if (
            is_take_range_with_same_direction() (direction, range),
            when_take_range(), when_not_take_range(),
            direction, check_limit_non_negative() (limit),
            std::forward <Range> (range), utility::pick_overload()));
    };

} // namespace operation

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct take : operation::unimplemented {};

        template <class Direction, class Limit, class Range>
            struct take <meta::vector <Direction>, meta::vector <Limit>,
                meta::vector <Range>>
        : operation::take <typename range::tag_of <Range>::type,
            Direction, typename std::decay <Limit>::type> {};

    } // namespace automatic_arguments

    // Make sure that operation::take receives a view.
    template <class ... Arguments> struct take
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view <automatic_arguments::take>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

/* take_range, which limits the underlying range. */

template <class Direction, class Limit, class Underlying> class take_range {
public:
    static_assert (range::is_view <Underlying>::value,
        "Underlying range must be a view");

    typedef Direction direction_type;
    typedef Limit limit_type;
    typedef Underlying underlying_type;

private:
    Direction direction_;
    Limit limit_;
    Underlying underlying_;
    friend class ::range::detail::callable::get_underlying;

    void decrement_limit() { -- limit_; }

    template <class RangeTag, class Direction2, class Enable>
        friend struct operation::chop_in_place;

public:
    template <class CVUnderlying>
    take_range (Direction const & direction, Limit const & limit,
        CVUnderlying && underlying)
    : direction_ (direction), limit_ (limit),
        underlying_ (std::forward <CVUnderlying> (underlying)) {}

    Direction const & direction() const { return direction_; }
    // Return unqualified limit.
    Limit limit() const { return limit_; }
};

template <class UnderlyingTag, class Direction, bool HomogeneousLimit>
    struct take_range_tag;

// The underlying tag can depend on how the range is qualified.
// Therefore, it must be forwarded.
template <class Direction, class Limit, class Underlying>
    struct tag_of <take_range <Direction, Limit, Underlying>>
{
    typedef take_range_tag <typename tag_of <Underlying>::type,
        Direction, !rime::is_constant <Limit>::value> type;
};
template <class Direction, class Limit, class Underlying>
    struct tag_of <take_range <Direction, Limit, Underlying> &>
{
    typedef take_range_tag <typename tag_of <Underlying &>::type,
        Direction, !rime::is_constant <Limit>::value> type;
};
template <class Direction, class Limit, class Underlying>
    struct tag_of <take_range <Direction, Limit, Underlying> &&>
{
    typedef take_range_tag <typename tag_of <Underlying &&>::type,
        Direction, !rime::is_constant <Limit>::value> type;
};

template <class Direction, class Limit, class Underlying>
    struct tag_of <take_range <Direction, Limit, Underlying> const>
{
    typedef take_range_tag <typename tag_of <Underlying const>::type,
        Direction, !rime::is_constant <Limit>::value> type;
};
template <class Direction, class Limit, class Underlying>
    struct tag_of <take_range <Direction, Limit, Underlying> const &>
{
    typedef take_range_tag <typename tag_of <Underlying const &>::type,
        Direction, !rime::is_constant <Limit>::value> type;
};
template <class Direction, class Limit, class Underlying>
    struct tag_of <take_range <Direction, Limit, Underlying> const &&>
{
    typedef take_range_tag <typename tag_of <Underlying const &&>::type,
        Direction, !rime::is_constant <Limit>::value> type;
};

namespace operation {

    template <class UnderlyingTag, class Direction, bool HomogeneousLimit>
        struct default_direction <take_range_tag <
            UnderlyingTag, Direction, HomogeneousLimit>>
    : forward_to_underlying <default_direction <UnderlyingTag>> {};

    template <class UnderlyingTag, class Direction, bool HomogeneousLimit>
        struct empty <take_range_tag <
            UnderlyingTag, Direction, HomogeneousLimit>, Direction>
    {
        template <class TakeRange>
            auto operator() (Direction const & direction, TakeRange const & r)
                const
        RETURNS (rime::or_ (r.limit() == rime::make_zero (r.limit()),
            range::empty (direction, range::detail::get_underlying (r))));
    };

    template <class UnderlyingTag, class Direction, bool HomogeneousLimit>
        struct size <take_range_tag <
                UnderlyingTag, Direction, HomogeneousLimit>,
            Direction, typename boost::enable_if <is_implemented <
                size <UnderlyingTag, Direction>>>::type>
    {
        template <class TakeRange>
            auto operator() (Direction const & direction, TakeRange const & r)
                const
        RETURNS (rime::min_ <take_detail::limit_merge_policy> (r.limit(),
            range::size (direction, range::detail::get_underlying (r))));
    };

    template <class UnderlyingTag, class Direction, bool HomogeneousLimit>
        struct first <take_range_tag <
                UnderlyingTag, Direction, HomogeneousLimit>,
            Direction, typename boost::enable_if <is_implemented <
                first <UnderlyingTag, Direction>>>::type>
    {
        template <class TakeRange>
            auto operator() (Direction const & direction, TakeRange && r)
                const
        -> typename result_of <callable::first (Direction,
            range::detail::callable::get_underlying (TakeRange))>::type
        {
            rime::assert_ (direction == r.direction());
            rime::assert_ (!range::empty (direction, r));
            return range::first (direction,
                range::detail::get_underlying (std::forward <TakeRange> (r)));
        }
    };

    template <class UnderlyingTag, class Direction, bool HomogeneousLimit,
        class Increment>
    struct drop <take_range_tag <UnderlyingTag, Direction, HomogeneousLimit>,
        Direction, Increment, typename boost::enable_if <is_implemented <
            drop <UnderlyingTag, Direction, Increment>>>::type>
    {
        template <class TakeRange>
            auto operator() (Direction const & direction,
                Increment const & increment, TakeRange && r) const
        -> decltype (range::take (direction, rime::cast_value <typename
            std::decay <TakeRange>::type::limit_type> (r.limit() - increment),
            range::drop (direction, increment,
                range::detail::get_underlying (std::declval <TakeRange>()))))
        {
            rime::assert_ (direction == r.direction());
            rime::assert_ (!rime::less_sign_safe (
                increment, rime::size_t <0>()));
            rime::assert_ (!rime::less_sign_safe (r.limit(), increment));
            typedef typename std::decay <TakeRange>::type::limit_type
                limit_type;
            return range::take (direction,
                rime::cast_value <limit_type> (r.limit() - increment),
                range::drop (direction, increment,
                    range::detail::get_underlying (
                        std::forward <TakeRange> (r))));
        }
    };

    // chop: enable only if this is implemented on the underlying range.
    // Otherwise, the generic implementation that uses "first" and "drop" should
    // work on the take_range, not on the underlying range.
    template <class UnderlyingTag, class Direction, bool HomogeneousLimit>
        struct chop <
            take_range_tag <UnderlyingTag, Direction, HomogeneousLimit>,
            Direction, typename boost::enable_if <is_implemented <
                chop <UnderlyingTag, Direction>>>::type>
    {
        /*
        To the reader: apologies for the following code.
        It calls "chop" on the underlying range, which returns a chopped<> with
        the first element (to be returned as-is) and the rest (to be wrapped in
        a new take_range).
        The best way to take this in is to read the body of the function, and
        only then convince yourself that the types make sense.
        */
        template <class TakeRange,
            class Limit = typename std::decay <TakeRange>::type::limit_type,
            class NewLimit = decltype (
                rime::cast_value <Limit> (Limit() - one_type())),
            class FirstAndUnderlyingRest = typename
                std::result_of <callable::chop (Direction, decltype (
                    range::detail::get_underlying (std::declval <TakeRange>())))
                >::type,
            class NewTakeRange = take_range <Direction, NewLimit,
                typename FirstAndUnderlyingRest::rest_type>,
            class Result = chopped <
                typename FirstAndUnderlyingRest::first_type, NewTakeRange>>
        Result operator() (Direction const & direction, TakeRange && r) const
        {
            one_type one;
            rime::assert_ (direction == r.direction());
            rime::assert_ (!rime::less_sign_safe (r.limit(), one));
            auto first_and_underlying_rest = range::chop (direction,
                range::detail::get_underlying (std::forward <TakeRange> (r)));
            return Result (first_and_underlying_rest.forward_first(),
                range::take (direction,
                    rime::cast_value <Limit> (r.limit() - one),
                    first_and_underlying_rest.move_rest()));
        }
    };

    // chop_in_place: enable only if this is implemented on the underlying
    // range.
    // In that case, the underlying range tag should tell us whether the range
    // is an lvalue or not.
    template <class UnderlyingTag, class Direction>
        struct chop_in_place <
            take_range_tag <UnderlyingTag, Direction, true>, Direction,
            typename boost::enable_if <is_implemented <
                chop_in_place <UnderlyingTag, Direction>>>::type>
    {
        template <class TakeRange>
            typename result_of <callable::chop_in_place (
                    Direction, typename TakeRange::underlying_type &)>::type
        operator() (Direction const & direction, TakeRange & range) const
        {
            auto && result = range::chop_in_place (
                direction, range::detail::get_underlying (range));
            range.decrement_limit();
            return static_cast <decltype (result)> (result);
        }
    };

} // namespace operation

} // namespace range

#endif // RANGE_TAKE_HPP_INCLUDED
