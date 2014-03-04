/*
Copyright 2013 Rogier van Dalen.

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

namespace range {

template <class Underlying> struct reverse_view {
    static_assert (is_view <Underlying>::value,
        "reverse_view only works with a view.");

    Underlying underlying_;
public:
    template <class Argument> explicit reverse_view (Argument && argument)
    : underlying_ (std::forward <Argument> (argument)) {}

    Underlying & underlying() { return underlying_; }
    Underlying const & underlying() const { return underlying_; }
};

template <class UnderlyingTag> struct reverse_view_tag;

template <class Underlying> struct tag_of_bare <reverse_view <Underlying>>
{ typedef reverse_view_tag <typename tag_of <Underlying>::type> type; };

namespace reverse_detail {
    template <class Range> class reverse {
        typedef typename range::result_of <range::callable::view (Range)>::type
            view;
        typedef typename std::decay <view>::type decayed_view;
    public:
        typedef reverse_view <decayed_view> type;
    };
} // namespace reverse_detail

/**
Returns a view of the range with the elements reversed.
This works for any direction that the range is used in that has a reverse.
For example, first (front, reverse (r)) is equivalent to first (back, r).
This simply wraps the range so that the directions for all operations are
converted on the fly by direction::reverse().
\param range
    The range to be reversed.
    It is turned into a view before it is stored inside the return type.
*/
template <class Range> inline
    typename reverse_detail::reverse <Range>::type
    reverse (Range && range)
{
    return typename reverse_detail::reverse <Range>::type (
        range::view (std::forward <Range> (range)));
}

namespace operation {

    template <class UnderlyingTag>
        struct default_direction <reverse_view_tag <UnderlyingTag>>
    {
        template <class ReverseView>
            auto operator() (ReverseView const & r) const
        RETURNS (range::default_direction (r.underlying()))
    };

    namespace reverse_detail {

        /**
        Give the underlying type, using the reference-specification of
        ReverseView.
        */
        template <class ReverseView> struct underlying_type;

        template <class Underlying>
            struct underlying_type <reverse_view <Underlying> const &>
        { typedef Underlying const & type; };
        template <class Underlying>
            struct underlying_type <reverse_view <Underlying> &>
        { typedef Underlying & type; };
        template <class Underlying>
            struct underlying_type <reverse_view <Underlying>>
        { typedef Underlying type; };

        /**
        \return The underlying range, using the reference-specification of the
            argument.
        */
        template <class Underlying> inline Underlying const &
            underlying (reverse_view <Underlying> const & r)
        { return r.underlying(); }

        template <class Underlying> inline Underlying &
            underlying (reverse_view <Underlying> & r)
        { return r.underlying(); }

        template <class Underlying> inline Underlying
            underlying (reverse_view <Underlying> && r)
        { return std::move (r.underlying()); }

        /**
        Helper for is_reverse_implemented.
        */
        template <class OperationLambda, class Direction>
            struct is_reverse_implemented_implementation
        : operation::is_implemented <
            typename boost::mpl::apply <OperationLambda,
                typename result_of <direction::callable::reverse (Direction)
                >::type>::type>
        {};

        /**
        \return true iff Direction has reverse() and the operation, given as an
        MPL Lambda function with mpl::_1 for the direction, is implemented.
        This avoids a compiler error if Direction is not reversable.
        */
        template <class OperationLambda, class Direction>
            struct is_reverse_implemented
        : boost::mpl::and_ <
            has <direction::callable::reverse (Direction)>,
            is_reverse_implemented_implementation <OperationLambda, Direction>>
        {};

    } // namespace reverse_detail

    template <class UnderlyingTag, class Direction>
        struct empty <reverse_view_tag <UnderlyingTag>, Direction,
            typename boost::enable_if <reverse_detail::is_reverse_implemented <
                empty <UnderlyingTag, boost::mpl::_1>, Direction>>::type>
    {
        // The return type is given explicitly because otherwise GCC 4.6
        // fails.
        template <class ReverseView>
            typename range::result_of <range::callable::empty (
                direction::callable::reverse (Direction),
                typename reverse_detail::underlying_type <ReverseView>::type)
            >::type operator() (Direction const & direction,
                ReverseView && reverse_view) const
        {
            return range::empty (direction::reverse (direction),
                reverse_detail::underlying (
                    std::forward <ReverseView> (reverse_view)));
        }
    };

    template <class UnderlyingTag, class Direction>
        struct size <reverse_view_tag <UnderlyingTag>, Direction,
            typename boost::enable_if <reverse_detail::is_reverse_implemented <
                size <UnderlyingTag, boost::mpl::_1>, Direction>>::type>
    {
        template <class ReverseView>
            typename range::result_of <range::callable::size (
                direction::callable::reverse (Direction),
                typename reverse_detail::underlying_type <ReverseView>::type)
            >::type operator() (Direction const & direction,
                ReverseView && reverse_view) const
        {
            return range::size (direction::reverse (direction),
                reverse_detail::underlying (
                    std::forward <ReverseView> (reverse_view)));
        }
    };

    template <class UnderlyingTag, class Direction>
        struct first <reverse_view_tag <UnderlyingTag>, Direction,
            typename boost::enable_if <reverse_detail::is_reverse_implemented <
                first <UnderlyingTag, boost::mpl::_1>, Direction>>::type>
    {
        template <class ReverseView>
            typename range::result_of <range::callable::first (
                direction::callable::reverse (Direction),
                typename reverse_detail::underlying_type <ReverseView>::type)
            >::type operator() (Direction const & direction,
                ReverseView && reverse_view) const
        {
            return range::first (direction::reverse (direction),
                reverse_detail::underlying (
                    std::forward <ReverseView> (reverse_view)));
        }
    };

    /**
    Implement "drop".
    There is no need for specific implementations for drop_one or for different
    types for Increment: passing them through to the underlying range works.
    */
    template <class UnderlyingTag, class Direction, class Increment>
        struct drop <reverse_view_tag <UnderlyingTag>, Direction, Increment,
            typename boost::enable_if <reverse_detail::is_reverse_implemented <
                drop <UnderlyingTag, boost::mpl::_1, Increment>, Direction>
            >::type>
    {
        template <class ReverseView>
            typename range::reverse_detail::reverse <
                typename result_of <range::callable::drop (
                    direction::callable::reverse (Direction), Increment,
                    typename reverse_detail::underlying_type <ReverseView>::type
                )>::type
            >::type operator() (Direction const & direction,
                Increment const & increment, ReverseView && reverse_view) const
        {
            return range::reverse (range::drop (direction::reverse (direction),
                increment, reverse_detail::underlying (
                    std::forward <ReverseView> (reverse_view))));
        }
    };

} // namespace operation
} // namespace range

#endif  // RANGE_REVERSE_HPP_INCLUDED
