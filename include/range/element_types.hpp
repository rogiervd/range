/*
Copyright 2014 Rogier van Dalen.

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
