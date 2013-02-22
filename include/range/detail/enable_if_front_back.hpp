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

#ifndef RANGE_RANGE_DETAIL_ENABLE_IF_FRONT_BACK_HPP_INCLUDED
#define RANGE_RANGE_DETAIL_ENABLE_IF_FRONT_BACK_HPP_INCLUDED

namespace range { namespace operation { namespace detail {

    /**
    Contains type "type" iff the list of directions is front, back, or a
    combination.
    */
    template <class Directions> struct enable_if_front_back {};

    template <>
        struct enable_if_front_back <meta::vector <direction::front>>
    { typedef void type; };

    template <>
        struct enable_if_front_back <meta::vector <direction::back>>
    { typedef void type; };

    template <> struct enable_if_front_back <
        meta::vector <direction::front, direction::back>>
    { typedef void type; };

    template <> struct enable_if_front_back <
        meta::vector <direction::back, direction::front>>
    { typedef void type; };

}}} // namespace range::operation::detail

#endif  // RANGE_RANGE_DETAIL_ENABLE_IF_FRONT_BACK_HPP_INCLUDED

