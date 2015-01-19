/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_EMPTY_VIEW_HPP_INCLUDED
#define RANGE_EMPTY_VIEW_HPP_INCLUDED

#include "rime/core.hpp"
#include "rime/always.hpp"

#include "core.hpp"

namespace range {

/**
A view that is empty from all directions.
This can be used from all directions, with "front" as its default direction.
It is essentially equivalent to member_view<>, but more explicit in its
intention.
*/
class empty_view;

struct empty_view_tag;

template <> struct tag_of_qualified <empty_view>
{ typedef empty_view_tag type; };

class empty_view {
private:
    friend class operation::member_access;

    template <class Direction> rime::true_type empty (Direction const &) const
    { return rime::true_; }

    template <class Direction> rime::size_t <0> size (Direction const &) const
    { return rime::size_t <0>(); }
};

} // namespace range

#endif // RANGE_EMPTY_VIEW_HPP_INCLUDED
