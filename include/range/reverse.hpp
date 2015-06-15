/*
Copyright 2013, 2015 Rogier van Dalen.

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
Define a reverse() function, which reverses a range by swapping directions on
the fly.
*/

#ifndef RANGE_REVERSE_HPP_INCLUDED
#define RANGE_REVERSE_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>

#include "utility/returns.hpp"

#include "core.hpp"

#include "helper/underlying.hpp"

namespace range {

template <class Underlying> struct reverse_view {
    static_assert (is_view <Underlying>::value,
        "reverse_view only works with a view.");

    template <class Wrapper> friend class helper::callable::get_underlying;
    Underlying underlying_;

public:
    explicit reverse_view (Underlying && underlying)
    : underlying_ (std::forward <Underlying> (underlying)) {}

    explicit reverse_view (Underlying const & underlying)
    : underlying_ (underlying) {}

    typedef Underlying underlying_type;

    Underlying & underlying() { return underlying_; }
    Underlying const & underlying() const { return underlying_; }

private:
    friend class helper::member_access;

    auto default_direction() const
    RETURNS (range::default_direction (underlying_));

    template <class Direction>
        decltype (range::empty (
            std::declval <Underlying const &>(),
            direction::opposite (std::declval <Direction>())))
    empty (Direction const & direction) const
    { return range::empty (underlying_, direction::opposite (direction)); }

    template <class Direction>
        decltype (range::size (
            std::declval <Underlying const &>(),
            direction::opposite (std::declval <Direction>())))
    size (Direction const & direction) const
    { return range::size (underlying_, direction::opposite (direction)); }

    // first and drop are implemented in namespace operation (below) where it is
    // easier to distinguish between qualifications.
};

namespace operation {
    struct reverse_view_tag {};
} // namespace operation

template <class Underlying> struct tag_of_qualified <reverse_view <Underlying>>
{ typedef operation::reverse_view_tag type; };

namespace callable {

    struct reverse {
    private:
        struct make_reverse_view {
            template <class View> auto operator() (View && view) const
            RETURNS (range::reverse_view <typename std::decay <View>::type>
                (std::forward <View> (view)));
        };

    public:
        // Turn the range into a view, and then wrap it in a reverse_view.
        template <class Range, class ... Directions>
            auto operator() (Range && range, Directions const & ... directions)
            const
        RETURNS (make_reverse_view() (
            range::view (std::forward <Range> (range), directions ...)));
    };

} // namespace callable

/** \brief

Return a view of the range with the elements reversed.
This works for any direction that the range is used in that has a reverse.
For example, first (reverse (r), front) is equivalent to first (r, back).
This simply wraps the range so that the directions for all operations are
converted on the fly by direction::opposite().
\param range
    The range to be reversed.
    It is turned into a view before it is stored inside the return type.
\param directions
    The directions that the view should be in.
    If no directions are given, the default direction is used.
*/
static auto const reverse = callable::reverse();

namespace operation {

    template <class ReverseView, class Direction> inline
        auto implement_first (reverse_view_tag, ReverseView && r,
            Direction const & direction)
    RETURNS (range::first (helper::get_underlying <ReverseView> (r),
        direction::opposite (direction)));

    template <class ReverseView, class Increment, class Direction> inline
        auto implement_drop (reverse_view_tag, ReverseView && r,
            Increment const & increment, Direction const & direction)
    RETURNS (range::reverse (range::drop (
        helper::get_underlying <ReverseView> (r),
        increment, direction::opposite (direction))));

} // namespace operation

} // namespace range

#endif  // RANGE_REVERSE_HPP_INCLUDED
