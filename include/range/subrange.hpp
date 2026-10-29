// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_SUBRANGE_HPP_INCLUDED
#define RANGE_SUBRANGE_HPP_INCLUDED

#include "utility/returns.hpp"

#include "rime/call_if.hpp"
#include "rime/core.hpp"

#include "core.hpp"

#include "helper/underlying.hpp"
#include "helper/with_direction.hpp"

#include "take.hpp"

namespace range {

template <class Underlying, class Original, class BeforeSize, class Direction>
class subrange_view;

namespace subrange_detail {
    struct subrange_view_tag {};
}  // namespace subrange_detail

template <class Underlying, class Original, class BeforeSize, class Direction>
struct tag_of_qualified<
    subrange_view<Underlying, Original, BeforeSize, Direction>>
{
    typedef subrange_detail::subrange_view_tag type;
};

/** \brief
Range that remembers that it is a subrange of a larger range, and provides
access to the range before.
*/
template <class Underlying, class Original, class BeforeSize, class Direction>
class subrange_view : helper::with_direction<Direction>
{
private:
    typedef helper::with_direction<Direction> with_direction_type;

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

    subrange_view(
        Underlying const & underlying, Original const & original,
        BeforeSize const & before_size, Direction const & direction)
    : with_direction_type(direction), underlying_(underlying),
      original_(original), before_size_(before_size)
    {}

public:
    // \todo useful? For users? Really?
    subrange_view(Underlying const & underlying, Direction const & direction)
    : with_direction_type(direction), underlying_(underlying), before_size_(),
      original_(underlying)
    {}

    subrange_view(subrange_view const &) = default;
    subrange_view(subrange_view &&) = default;

    subrange_view & operator=(subrange_view const &) = default;
    subrange_view & operator=(subrange_view &&) = default;

    Direction const & direction() const
    {
        return with_direction_type::direction();
    }

    /**
    \return the underlying range, with exactly the elements that are in the
    subrange, but without the operation before().
    */
    Underlying const & underlying() const { return underlying_; }

    Original const & original() const { return original_; }

    // \todo Privatise?
    BeforeSize const & before_size() const { return before_size_; }

    /**
    \return The part of the original range that was before the current range.
    */
    auto before() const -> decltype(range::take(
        original_, before_size_, std::declval<Direction>()))
    {
        return range::take(original_, before_size_, direction());
    }

private:
    friend class helper::member_access;

    // empty, size, first: forward to underlying.
    template <class DirectionArgument>
    auto empty(DirectionArgument const & direction_argument) const
        RETURNS(range::empty(underlying_, direction_argument));

    template <class DirectionArgument>
    auto size(DirectionArgument const & direction_argument) const
        RETURNS(range::size(underlying_, direction_argument));

    template <class DirectionArgument>
    auto first(DirectionArgument const & direction_argument) const
        RETURNS(range::first(underlying_, direction_argument));
};

namespace callable {

    class make_subrange
    {
    protected:
        struct dispatch
        {
            template <
                class Underlying, class Original, class BeforeSize,
                class Direction,
                class Result = subrange_view<
                    typename std::decay<Underlying>::type,
                    typename std::decay<Original>::type, BeforeSize, Direction>>
            Result operator()(
                Underlying && underlying, Original && original,
                BeforeSize const & before_size, Direction const & direction)
            {
                return Result(
                    std::forward<Underlying>(underlying),
                    std::forward<Original>(original), before_size, direction);
            }
        };

        template <class Range, class Direction = void>
        struct default_before_size
        : std::conditional<
              is_homogeneous<
                  typename decayed_result_of<callable::view(
                      Range, Direction)>::type,
                  Direction>::value,
              std::size_t, rime::size_t<0>> {};

        template <class Range> struct default_before_size<Range>
        : default_before_size<
              Range,
              typename decayed_result_of<callable::default_direction(
                  Range)>::type> {};

    public:
        // Normal invocation: no original, no before_size.
        template <class Underlying, class Direction> auto operator()(
            Underlying && underlying, Direction const & direction) const
            RETURNS(
                dispatch()(
                    range::view(
                        std::forward<Underlying>(underlying), direction),
                    range::view(
                        std::forward<Underlying>(underlying), direction),
                    typename default_before_size<Underlying, Direction>::type(),
                    direction));

        // No direction: use default.
        template <class Underlying>
        auto operator()(Underlying && underlying) const RETURNS(
            dispatch()(
                range::view(std::forward<Underlying>(underlying)),
                range::view(std::forward<Underlying>(underlying)),
                typename default_before_size<Underlying>::type(),
                range::default_direction(underlying)));
    };

}  // namespace callable

/**
Return a range that represents another range, but when elements are dropped,
keeps track of the part of the original range that are before it.

\code
std::vector <int> v {3, 6, 7};
auto tail = drop (make_subrange (v));
// tail now is a view on {6, 7}.
auto head = tail.before();
// head now is a view on {3}.
\endcode

Algorithms that indicate a specific element in a range could return a subrange
so that it is possible to use the range starting from that element or the range
up to that element.

This is also useful to implement algorithms, especially ones that edit the
underlying range.
For example, QuickSort partitions a range by moving elements less than the pivot
to before it, and elements greater than the pivot to after it.
A natural way to implement this uses <c>subrange(subrange(r, back), front)</c>.

The subrange that is returned always offers the same interface as
\c subrange_view.
However, though by default it is actually \c subrange_view, a more specialised
type may be returned for reasons of efficiency.

\todo Specialise for iterator_range.
*/
static auto const make_subrange = callable::make_subrange();

namespace subrange_detail {

    /**
    Internal version of make_subrange.
    underlying and original are given separately, and before_size explicitly.
    */
    struct make_subrange : ::range::callable::make_subrange
    {
    public:
        template <
            class Underlying, class Original, class BeforeSize, class Direction>
        auto operator()(
            Underlying && underlying, Original && original,
            BeforeSize const & before_size, Direction const & direction) const
            RETURNS(
                dispatch()(
                    range::view(std::forward<Underlying>(underlying)),
                    range::view(std::forward<Original>(original)), before_size,
                    direction));
    };

    struct drop_same_direction
    {
        // \todo Qualification of underlying and original
        template <class Subrange, class Increment, class Direction>
        auto operator()(
            Subrange && subrange, Increment const & increment,
            Direction const & direction) const
            // Dropping in the direction of the subrange.
            RETURNS(
                make_subrange()(
                    // Only change the underlying range...
                    range::drop(subrange.underlying(), increment, direction),
                    // ... not the original ...
                    subrange.original(),
                    // ... so before_size does change.
                    subrange.before_size() + increment, subrange.direction()));
    };

    struct drop_different_direction
    {
        // \todo Qualification of underlying and original
        template <class Subrange, class Increment, class Direction>
        auto operator()(
            Subrange && subrange, Increment const & increment,
            Direction const & direction) const
            // Dropping in a different direction from the subrange.
            // Apply to the underlying and original ranges.
            RETURNS(
                make_subrange()(
                    // Change the underlying range ...
                    range::drop(subrange.underlying(), increment, direction),
                    // ... as well as the original ...
                    range::drop(subrange.original(), increment, direction),
                    // ... with no difference to before_size.
                    subrange.before_size(), subrange.direction()));
    };

    template <class Subrange, class Increment, class Direction>
    auto implement_drop(
        subrange_view_tag, Subrange && range, Increment const & increment,
        Direction const & direction)
        RETURNS(
            rime::call_if(
                range.direction() == direction, drop_same_direction(),
                drop_different_direction(), std::forward<Subrange>(range),
                increment, direction));

}  // namespace subrange_detail

}  // namespace range

#endif  // RANGE_SUBRANGE_HPP_INCLUDED
