/*
Copyright 2014 Rogier van Dalen.

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
Provide a view of boost::optional as a range.
boost::optional is not a range (this would cause confusing overloads), so it
must be explicitly converted into a range.
*/

#ifndef RANGE_STD_VIEW_OPTIONAL_HPP_INCLUDED
#define RANGE_STD_VIEW_OPTIONAL_HPP_INCLUDED

#include <type_traits>

#include "rime/always.hpp"

#include "range/core.hpp"
#include "range/empty_view.hpp"

#include "range/detail/enable_if_front_back.hpp"

namespace range {

template <class Optional> class optional_view;
struct optional_view_tag;

template <class Optional> struct tag_of_qualified <optional_view <Optional>>
{ typedef optional_view_tag type; };

/**
A view of an optional in directions "front" and "back".
This is empty iff the optional is empty, or a one-element range if the optional
contains a value.
*/
template <class Optional> class optional_view {
    Optional & optional_;

    typedef typename std::decay <Optional>::type::value_type decayed_value_type;
public:
    optional_view (Optional & optional) : optional_ (optional) {}

    // Constness op optional propagates to value type.
    typedef typename std::conditional <std::is_const <Optional>::value,
        decayed_value_type const, decayed_value_type>::type value_type;

    bool empty() const { return !this->optional_; }
    value_type & first() const { return this->optional_.get(); }
};

namespace apply {

    template <class ...> struct view_optional : operation::unimplemented {};

    // Do not accept temporaries because that can only lead to tears.
    template <class Optional> struct view_optional <Optional &> {
        optional_view <Optional> operator() (Optional & o) const
        { return optional_view <Optional> (o); }
    };

} // namespace apply

namespace callable {

    struct view_optional : generic <apply::view_optional> {};

} // namespace callable

/**
View an "optional" as a range containing zero or one elements.
\param optional
    A boost::optional or std::optional (when that becomes available).
    This should not be an rvalue, since a reference to the optional is stored.
*/
static auto const view_optional = callable::view_optional();

namespace operation {

    // empty. (Automatically implemented for "back" too.)
    template <> struct empty <optional_view_tag, direction::front> {
        template <class OptionalView> bool operator() (
            direction::front, OptionalView const & v) const
        { return v.empty(); }
    };

    // size. (Automatically implemented for "back" too.)
    template <> struct size <optional_view_tag, direction::front> {
        template <class OptionalView> std::size_t operator() (
            direction::front,  OptionalView const & v) const
        {
            if (v.empty())
                return 0u;
            else
                return 1u;
        }
    };

    // first.
    template <class Direction>
        struct first <optional_view_tag, Direction, typename
            detail::enable_if_front_back <meta::vector <Direction>>::type>
    {
        template <class OptionalView>
            typename OptionalView::value_type & operator() (
                Direction, OptionalView const & v) const
        {
            assert (!v.empty());
            return v.first();
        }
    };

    // drop_one. Always results in an empty range.
    template <class Direction>
        struct drop_one <optional_view_tag, Direction, typename
            detail::enable_if_front_back <meta::vector <Direction>>::type>
    {
        template <class Increment, class OptionalView> empty_view operator() (
            Direction, Increment, OptionalView const & v) const
        {
            assert (!v.empty());
            return empty_view();
        }
    };

} // namespace operation

} // namespace range

#endif  // RANGE_STD_VIEW_OPTIONAL_HPP_INCLUDED
