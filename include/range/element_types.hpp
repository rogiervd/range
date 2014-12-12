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

#ifndef RANGE_ELEMENT_TYPES_HPP_INCLUDED
#define RANGE_ELEMENT_TYPES_HPP_INCLUDED

#include "meta/range.hpp"

#include "core.hpp"

namespace range {
    struct element_types_tag;

    /**
    Meta-range with the types that traversing over \a Range result in.
    If the range is homogeneous, this has infinite length.
    The direction that this range is traversed in should be the direction of
    interest of the underlying range.
    The default direction is that of the underlying range.

    view_once() is called on the range first.
    The resulting types will often be reference types.
    For example, this is the case when \a Range is a container.
    */
    template <class Range> struct element_types {
        typedef Range underlying_type;

        template <class Direction> struct view_once
        : ::std::result_of < ::range::callable::view_once (Direction, Range)>
        {};

        typedef element_types type;
    };

} // namespace range

namespace meta {

    template <class Range> struct range_tag <range::element_types <Range>>
    { typedef range::element_types_tag type; };

    namespace operation {

        // default_direction.
        template <> struct default_direction <range::element_types_tag> {
            template <class TypesFrom> struct apply
            : std::result_of < ::range::callable::default_direction (
                typename TypesFrom::underlying_type)> {};
        };

        // empty.
        template <class Direction>
            struct empty <range::element_types_tag, Direction>
        {
            template <class TypesFrom> struct apply
            : ::range::always_empty <Direction,
                typename TypesFrom::template view_once <Direction>::type> {};
        };

        // size.
        template <class Direction>
            struct size <range::element_types_tag, Direction>
        {
            // Expect an error here if the size of the underlying range is not
            // known at compile time.
            template <class TypesFrom> struct apply
            : std::result_of < ::range::callable::size (Direction,
                typename TypesFrom::template view_once <Direction>::type)>::type
            {};
        };

        // first.
        template <class Direction>
            struct first <range::element_types_tag, Direction>
        {
            template <class TypesFrom> struct apply
            : std::result_of < ::range::callable::first (Direction,
                typename TypesFrom::template view_once <Direction>::type)> {};
        };

        // drop.
        template <typename Direction, typename Increment>
            struct drop <range::element_types_tag, Direction, Increment>
        {
            template <class TypesFrom> struct apply {
                typedef ::range::element_types <typename
                    ::range::decayed_result_of < ::range::callable::drop (
                        Direction, Increment, typename
                        TypesFrom::template view_once <Direction>::type)>::type>
                    type;
            };
        };

}} // namespace meta::operation

#endif // RANGE_ELEMENT_TYPES_HPP_INCLUDED
