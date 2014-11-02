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

class empty_view {};

namespace operation {

    // empty.
    template <class Direction> struct empty <empty_view_tag, Direction>
    : rime::callable::always_default <rime::true_type> {};

    // size.
    template <class Direction> struct size <empty_view_tag, Direction>
    : rime::callable::always_default <rime::size_t <0>> {};

} // namespace operation

} // namespace range

#endif // RANGE_EMPTY_VIEW_HPP_INCLUDED
