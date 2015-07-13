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
Turn containers in std:: into ranges.
Note that this file does not included the standard headers: instead it includes
forward declarations.
*/

#ifndef RANGE_STD_CONTAINER_HPP_INCLUDED
#define RANGE_STD_CONTAINER_HPP_INCLUDED

#include "meta/vector.hpp"
#include "meta/transform.hpp"
#include "meta/enumerate.hpp"

#include "rime/enable_if_constant.hpp"

#include "range/core.hpp"
#include "range/heavyweight.hpp"
#include "range/iterator_range.hpp"

namespace std {

    // Sequence containers.
    template <class Type, std::size_t N> struct array;
    template <class Type, class Allocator> class vector;
    template <class Type, class Allocator> class deque;
    template <class Type, class Allocator> class forward_list;
    template <class Type, class Allocator> class list;
    template <class Type, class Traits, class Allocator> class basic_string;

    // Associative containers.
    template <class Key, class Compare, class Allocator> class set;
    template <class Key, class Compare, class Allocator> class multiset;
    template <class Key, class Value, class Compare, class Allocator> class map;
    template <class Key, class Value, class Compare, class Allocator>
        class multimap;

    // Unordered (hashed) associative containers.
    template <class Key, class Hash, class KeyEqual, class Allocator>
        class unordered_set;
    template <class Key, class Hash, class KeyEqual, class Allocator>
        class unordered_multiset;
    template <class Key, class Value, class Hash, class KeyEqual,
        class Allocator> class unordered_map;
    template <class Key, class Value, class Hash, class KeyEqual,
        class Allocator> class unordered_multimap;

} // namespace std

namespace range {

    namespace std_container_operation {

        struct std_front_container_tag : heavyweight::heavyweight_tag {};
        struct std_front_back_container_tag : std_front_container_tag {};

    } // namespace std_container_operation

    // Sequence containers.
    /// \cond DONT_DOCUMENT
    template <class Type, class Allocator>
        struct tag_of_qualified <std::vector <Type, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Type, class Allocator>
        struct tag_of_qualified <std::deque <Type, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Type, class Allocator>
        struct tag_of_qualified <std::forward_list <Type, Allocator>>
    { typedef std_container_operation::std_front_container_tag type; };

    template <class Type, class Allocator>
        struct tag_of_qualified <std::list <Type, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Type, class Traits, class Allocator>
        struct tag_of_qualified <std::basic_string <Type, Traits, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    // Associative containers.
    template <class Key, class Compare, class Allocator>
        struct tag_of_qualified <std::set <Key, Compare, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Key, class Compare, class Allocator>
        struct tag_of_qualified <std::multiset <Key, Compare, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Key, class Value, class Compare, class Allocator>
    struct tag_of_qualified <std::map <Key, Value, Compare, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Key, class Value, class Compare, class Allocator>
    struct tag_of_qualified <std::multimap <Key, Value, Compare, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    // Unordered associative containers.
    template <class Key, class Hash, class KeyEqual, class Allocator>
        struct tag_of_qualified <
            std::unordered_set <Key, Hash, KeyEqual, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Key, class Hash, class KeyEqual, class Allocator>
        struct tag_of_qualified <
            std::unordered_multiset <Key, Hash, KeyEqual, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Key, class Value, class Hash, class KeyEqual,
            class Allocator>
        struct tag_of_qualified <
            std::unordered_map <Key, Value, Hash, KeyEqual, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };

    template <class Key, class Value, class Hash, class KeyEqual,
            class Allocator>
        struct tag_of_qualified <
            std::unordered_multimap <Key, Value, Hash, KeyEqual, Allocator>>
    { typedef std_container_operation::std_front_back_container_tag type; };
    /// \endcond

    namespace std_container_operation {

        /* make_iterator_range_with_once. */

        // If Once is not compile-time true.
        template <class Once, class Container, class Enable
            = typename rime::disable_if_constant_true <Once>::type> inline
            auto make_iterator_range_with_once (
                Once once, Container && container)
        RETURNS (make_iterator_range (std::forward <Container> (container)));

        // If Once is compile-time true.
        template <class Once, class Container, class Enable
            = typename rime::enable_if_constant_true <Once>::type>
        inline auto make_iterator_range_with_once (
            Once once, Container && container)
        RETURNS (make_move_iterator_range (
            std::forward <Container> (container)));

        /* implement_make_view. */
        /* Allow the correct combinations of front and back. */
        template <class Once, class Container>
        inline auto implement_make_view (std_front_container_tag,
            Once once, Container && container, direction::front)
        RETURNS (make_iterator_range_with_once (once,
            std::forward <Container> (container)));

        template <class Once, class Container>
        inline auto implement_make_view (std_front_back_container_tag,
            Once once, Container && container, direction::back)
        RETURNS (make_iterator_range_with_once (once,
            std::forward <Container> (container)));

        template <class Once, class Container>
        inline auto implement_make_view (std_front_back_container_tag,
            Once once, Container && container,
                direction::front, direction::back)
        RETURNS (make_iterator_range_with_once (once,
            std::forward <Container> (container)));

        template <class Once, class Container>
        inline auto implement_make_view (std_front_back_container_tag,
            Once once, Container && container,
                direction::back, direction::front)
        RETURNS (make_iterator_range_with_once (once,
            std::forward <Container> (container)));

    } // namespace std_container_operation

} // namespace range

#endif  // RANGE_STD_CONTAINER_HPP_INCLUDED
