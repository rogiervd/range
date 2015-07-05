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

#ifndef RANGE_SUBRANGE_HPP_INCLUDED
#define RANGE_SUBRANGE_HPP_INCLUDED

#include "utility/returns.hpp"

#include "rime/core.hpp"
#include "rime/call_if.hpp"

#include "core.hpp"

#include "helper/underlying.hpp"
#include "helper/with_direction.hpp"

#include "take.hpp"

namespace range {

template <class Underlying, class Original, class BeforeSize, class Direction>
    class subrange_view;

namespace subrange_detail {
    struct subrange_view_tag {};
} // namespace subrange_detail

template <class Underlying, class Original, class BeforeSize, class Direction>
    struct tag_of_qualified <
        subrange_view <Underlying, Original, BeforeSize, Direction>>
{ typedef subrange_detail::subrange_view_tag type; };

/**
Range that remembers that it is a subrange of a larger range, and provides
access to the ranges before and after it.
*/
template <class Underlying, class Original, class BeforeSize, class Direction>
    class subrange_view
: helper::with_direction <Direction>
{
private:
    typedef helper::with_direction <Direction> with_direction_type;
public:
    // \todo assert view.
    typedef Underlying underlying_type;
    typedef Original original_type;
    typedef BeforeSize before_size_type;
    typedef Direction direction_type;

private:
    // \todo Privatise.
public:
    template <class> friend class helper::callable::get_underlying;
    Underlying underlying_;
    original_type original_;
    before_size_type before_size_;

    subrange_view (Underlying const & underlying, Original const & original,
        BeforeSize const & before_size, Direction const & direction)
    : with_direction_type (direction),
        underlying_ (underlying), original_ (original), before_size_ (before_size)
        {}

public:
    // \todo useful? For users? Really?
    subrange_view (Direction const & direction, Underlying const & underlying)
    : with_direction_type (direction), underlying_ (underlying),
        before_size_(), original_ (underlying) {}

    subrange_view (subrange_view const &) = default;

    subrange_view & operator= (subrange_view const &) = default;

    Direction const & direction() const
    { return with_direction_type::direction(); }

    /**
    \return the underlying range, with exactly the elements that are in the
    subrange, but without the operation before().
    */
    Underlying const & underlying() const { return underlying_; }

    Original const & original() const { return original_; }

    BeforeSize const & before_size() const { return before_size_; }

    /**
    \return The part of the original range that was before the current range.
    */
    auto before() const
    -> decltype (range::take (original_, before_size_, std::declval <Direction>()))
    { return range::take (original_, before_size_, direction()); }

private:
    friend class helper::member_access;

    // empty, size, first: forward to underlying.
    template <class DirectionArgument>
        auto empty (DirectionArgument const & direction_argument) const
    RETURNS (range::empty (underlying_, direction_argument));

    template <class DirectionArgument>
        auto size (DirectionArgument const & direction_argument) const
    RETURNS (range::size (underlying_, direction_argument));

    template <class DirectionArgument>
        auto first (DirectionArgument const & direction_argument) const
    RETURNS (range::first (underlying_, direction_argument));
};

namespace callable {

    class make_subrange {
    private:
        struct dispatch {
            template <class Underlying, class Original, class BeforeSize, class Direction,
                class Result = subrange_view <typename std::decay <Underlying>::type,
                    typename std::decay <Original>::type, BeforeSize, Direction>>
            Result operator() (Underlying && underlying, Original && original,
                BeforeSize const & before_size, Direction const & direction)
            {
                return Result (std::forward <Underlying> (underlying),
                    std::forward <Original> (original), before_size, direction);
            }
        };

        template <class Range, class Direction = void> struct default_before_size
        : std::conditional <is_homogeneous <typename
                decayed_result_of <callable::view (Range, Direction)>::type, Direction>::value,
            std::size_t, rime::size_t <0>> {};

        template <class Range> struct default_before_size <Range>
        : default_before_size <Range, typename
            decayed_result_of <callable::default_direction (Range)>::type> {};

    public:
        template <class Underlying, class Original, class BeforeSize, class Direction>
            auto operator() (Underlying && underlying, Original && original,
                BeforeSize const & before_size, Direction const & direction) const
        RETURNS (dispatch() (
            range::view (std::forward <Underlying> (underlying)),
            range::view (std::forward <Original> (original)),
            before_size, direction));

        // Normal invocation: no original, no before_size.
        template <class Underlying, class Direction>
            auto operator() (
                Underlying const & underlying, Direction const & direction) const
        RETURNS (dispatch() (range::view (underlying, direction), range::view (underlying, direction),
            typename default_before_size <Underlying, Direction>::type(),
            direction));

        // No direction: use default.
        template <class Underlying>
            auto operator() (Underlying const & underlying) const
        RETURNS (dispatch() (range::view (underlying), range::view (underlying),
            typename default_before_size <Underlying>::type(),
            range::default_direction (underlying)));
    };

} // namespace callable

static auto const make_subrange = callable::make_subrange();

namespace subrange_detail {

    struct drop_same_direction {
        // \todo Qualification of underlying and original
        template <class Subrange, class Increment, class Direction>
            auto operator() (Subrange && subrange, Increment const & increment, Direction const & direction) const
        RETURNS (make_subrange (
            // New subrange.
            range::drop (subrange.underlying(), increment, direction),
            subrange.original(), subrange.before_size() + increment,
            subrange.direction()));
    };

    struct drop_different_direction {
        // \todo Qualification of underlying and original
        template <class Subrange, class Increment, class Direction>
            auto operator() (Subrange && subrange, Increment const & increment, Direction const & direction) const
        RETURNS (make_subrange (
            // New subrange.
            range::drop (subrange.underlying(), increment, direction),
            range::drop (subrange.original(), increment, direction),
            subrange.before_size() + increment,
            subrange.direction()));
    };

    template <class Subrange, class Increment, class Direction>
        auto implement_drop (subrange_view_tag, Subrange && range,
            Increment const & increment, Direction const & direction)
    RETURNS (rime::call_if (
        range.direction() == direction,
        drop_same_direction(), drop_different_direction(),
        std::forward <Subrange> (range), increment, direction));

} // namespace subrange_detail

/*
template <class Range> inline auto
    subrange (Range && range)
RETURNS (subrange_view <typename result_of <callable::default_direction (Range)>::type,
    typename decayed_result_of <callable::view (Range)>::type>
        (default_direction (range), view (std::forward <Range> (range))));

template <class Direction, class Range> inline auto
    subrange (Direction const & direction, Range && range)
RETURNS (subrange_view <Direction,
    typename decayed_result_of <callable::view (Direction, Range)>::type>
        (direction, view (direction, std::forward <Range> (range))));
*/

/*namespace operation {

    // default_direction.

    template <class Direction1, class Direction2, class UnderlyingTag>
        struct empty <subrange_view_tag <Direction1, UnderlyingTag>, Direction2>
    : forward_to_underlying <empty <UnderlyingTag, Direction2>> {};

    template <class Direction1, class Direction2, class UnderlyingTag>
        struct size <subrange_view_tag <Direction1, UnderlyingTag>, Direction2>
    : forward_to_underlying <size <UnderlyingTag, Direction2>> {};

    template <class Direction1, class Direction2, class UnderlyingTag>
        struct first <subrange_view_tag <Direction1, UnderlyingTag>, Direction2>
    : forward_to_underlying <first <UnderlyingTag, Direction2>> {};

    template <class Direction, class UnderlyingTag, class Increment>
        struct drop <subrange_view_tag <Direction, UnderlyingTag>,
            Direction, Increment>
    {
        template <class Underlying>
            subrange_view <Direction, Underlying>
            operator() (Direction const & direction,
                Increment const & increment,
                subrange_view <Direction, Underlying> const & r) const
        {
            // \todo Should forward to other implementation if direction != r.direction?
            return subrange_view <Direction, Underlying> (
                r.direction_, range::drop (direction, increment, r.underlying_),
                r.before_size_ + increment, r.original_ );
        }
    };
    // Directions are different.
    // \todo Should be disabled if Underlying does not support this.
    template <class Direction1, class Direction2, class UnderlyingTag, class Increment>
        struct drop <subrange_view_tag <Direction1, UnderlyingTag>,
            Direction2, Increment>
    {
        template <class Underlying>
            subrange_view <Direction1, Underlying>
            operator() (Direction2 const & direction2,
                Increment const & increment,
                subrange_view <Direction1, Underlying> const & r) const
        {
            return subrange_view <Direction1, Underlying> (
                r.direction_, range::drop (direction2, increment, r.underlying_),
                r.before_size_, range::drop (direction2, increment, r.original_));
        }
    };

} // namespace operation
*/

} // namespace range

#endif // RANGE_SUBRANGE_HPP_INCLUDED
