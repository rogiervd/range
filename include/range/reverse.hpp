/*
Copyright 2013, 2015 Rogier van Dalen.

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

#include "detail/underlying.hpp"

namespace range {

template <class Underlying> struct reverse_view {
    static_assert (is_view <Underlying>::value,
        "reverse_view only works with a view.");

    friend class ::range::detail::callable::get_underlying;
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
    friend class operation::member_access;

    auto default_direction() const
    RETURNS (range::default_direction (underlying_));

    template <class Direction>
        typename result_of_or <callable::empty (
            direction::callable::reverse (Direction), Underlying const &)
    >::type empty (Direction const & direction) const
    { return range::empty (direction::reverse (direction), underlying_); }

    template <class Direction>
        typename result_of_or <callable::size (
            direction::callable::reverse (Direction), Underlying const &)
    >::type size (Direction const & direction) const
    { return range::size (direction::reverse (direction), underlying_); }

    // first and drop are implemented in namespace operation (below) where it is
    // easier to distinguish between qualifications.
};

struct reverse_view_tag;

template <class Underlying> struct tag_of_qualified <reverse_view <Underlying>>
{ typedef reverse_view_tag type; };

namespace operation {

    namespace reverse_detail {

        struct make_reverse_view {
            template <class Range> auto operator() (Range && range) const
            RETURNS (range::reverse_view <typename std::decay <Range>::type>
                (std::forward <Range> (range)));
        };

    } // namespace reverse_detail

    template <class RangeTag, class Directions, class Range,
            class Enable = void>
        struct reverse
    : helper::call_with_last <1, Directions, reverse_detail::make_reverse_view>
    {};

} // namespace operation

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct reverse : operation::unimplemented {};

        template <class Directions, class Range>
            struct reverse <Directions, meta::vector<>, meta::vector <Range>>
        : operation::reverse <
            typename range::tag_of <Range>::type, Directions, Range &&>
        {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct reverse
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view <automatic_arguments::reverse
            >::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

namespace callable {
    struct reverse : generic <apply::reverse> {};
} // namespace callable

/**
Return a view of the range with the elements reversed.
This works for any direction that the range is used in that has a reverse.
For example, first (front, reverse (r)) is equivalent to first (back, r).
This simply wraps the range so that the directions for all operations are
converted on the fly by direction::reverse().
\param directions
    The directions that the view should be in.
    If no directions are given, the default direction is used.
\param range
    The range to be reversed.
    It is turned into a view before it is stored inside the return type.
*/
static auto const reverse = callable::reverse();

namespace operation {

    template <class Direction, class ReverseView>
        struct first <reverse_view_tag, Direction, ReverseView,
            typename boost::enable_if <has <
                callable::first (direction::callable::reverse (Direction),
                    range::detail::callable::get_underlying (ReverseView))
                >>::type>
    {
        auto operator() (Direction const & direction,
            ReverseView && reverse_view) const
        RETURNS (range::first (direction::reverse (direction),
            range::detail::get_underlying (
                std::forward <ReverseView> (reverse_view))));
    };

    /**
    Implement "drop".
    There is no need for specific implementations for drop_one or for different
    types for Increment: passing them through to the underlying range works.
    */
    template <class Direction, class Increment, class ReverseView>
        struct drop <reverse_view_tag, Direction, Increment, ReverseView,
            typename boost::enable_if <has <
                callable::drop (direction::callable::reverse (Direction),
                    Increment,
                    range::detail::callable::get_underlying (ReverseView))
                >>::type>
    {
        auto operator() (Direction const & direction,
            Increment const & increment, ReverseView && reverse_view) const
        RETURNS (range::reverse (range::drop (direction::reverse (direction),
            increment, range::detail::get_underlying (
                std::forward <ReverseView> (reverse_view)))));
    };

}} // namespace range::operation

#endif  // RANGE_REVERSE_HPP_INCLUDED
