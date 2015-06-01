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

#include "range/core.hpp"
#include "range/heavyweight.hpp"
#include "range/iterator_range.hpp"

#include "range/detail/enable_if_front_back.hpp"

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

    // Sequence containers.
    template <class Type, class Allocator>
        struct tag_of_qualified <std::vector <Type, Allocator>>
    { typedef heavyweight_tag <std::vector <Type, Allocator>> type; };

    template <class Type, class Allocator>
        struct tag_of_qualified <std::deque <Type, Allocator>>
    { typedef heavyweight_tag <std::deque <Type, Allocator>> type; };

    template <class Type, class Allocator>
        struct tag_of_qualified <std::forward_list <Type, Allocator>>
    { typedef heavyweight_tag <std::forward_list <Type, Allocator>> type; };

    template <class Type, class Allocator>
        struct tag_of_qualified <std::list <Type, Allocator>>
    { typedef heavyweight_tag <std::list <Type, Allocator>> type; };

    template <class Type, class Traits, class Allocator>
        struct tag_of_qualified <std::basic_string <Type, Traits, Allocator>>
    {
        typedef heavyweight_tag <std::basic_string <Type, Traits, Allocator>>
            type;
    };

    // Associative containers.
    template <class Key, class Compare, class Allocator>
        struct tag_of_qualified <std::set <Key, Compare, Allocator>>
    { typedef heavyweight_tag <std::set <Key, Compare, Allocator>> type; };

    template <class Key, class Compare, class Allocator>
        struct tag_of_qualified <std::multiset <Key, Compare, Allocator>>
    { typedef heavyweight_tag <std::multiset <Key, Compare, Allocator>> type; };

    template <class Key, class Value, class Compare, class Allocator>
    struct tag_of_qualified <std::map <Key, Value, Compare, Allocator>> {
        typedef heavyweight_tag <std::map <Key, Value, Compare, Allocator>>
            type;
    };

    template <class Key, class Value, class Compare, class Allocator>
    struct tag_of_qualified <std::multimap <Key, Value, Compare, Allocator>> {
        typedef heavyweight_tag <std::multimap <Key, Value, Compare, Allocator>>
            type;
    };

    // Unordered associative containers.
    template <class Key, class Hash, class KeyEqual, class Allocator>
        struct tag_of_qualified <
            std::unordered_set <Key, Hash, KeyEqual, Allocator>>
    {
        typedef heavyweight_tag <std::unordered_set <
            Key, Hash, KeyEqual, Allocator>> type;
    };

    template <class Key, class Hash, class KeyEqual, class Allocator>
        struct tag_of_qualified <
            std::unordered_multiset <Key, Hash, KeyEqual, Allocator>>
    {
        typedef heavyweight_tag <std::unordered_multiset <
            Key, Hash, KeyEqual, Allocator>> type;
    };

    template <class Key, class Value, class Hash, class KeyEqual,
            class Allocator>
        struct tag_of_qualified <
            std::unordered_map <Key, Value, Hash, KeyEqual, Allocator>>
    {
        typedef heavyweight_tag <std::unordered_map <
            Key, Value, Hash, KeyEqual, Allocator>> type;
    };

    template <class Key, class Value, class Hash, class KeyEqual,
            class Allocator>
        struct tag_of_qualified <
            std::unordered_multimap <Key, Value, Hash, KeyEqual, Allocator>>
    {
        typedef heavyweight_tag <std::unordered_multimap <
            Key, Value, Hash, KeyEqual, Allocator>> type;
    };

    namespace operation {

        namespace detail {

            /**
            Implement the make_view operation for a container that provides
            begin() and end() methods.
            This returns an iterator_range.
            */
            template <bool Move, class Container> struct view_std_container;

            template <class Container>
                struct view_std_container <false, Container>
            {
                typedef typename Container::iterator iterator;
                typedef typename Container::const_iterator const_iterator;

                // One direction, mutable container
                iterator_range <iterator> operator() (Container & container)
                    const
                {
                    return iterator_range <iterator> (
                        container.begin(), container.end());
                }

                // One direction, const container
                iterator_range <const_iterator> operator() (
                    Container const & container) const
                {
                    return iterator_range <const_iterator> (
                        container.begin(), container.end());
                }
            };

            template <class Container>
                struct view_std_container <true, Container>
            : view_std_container <false, Container>
            {
                using view_std_container <false, Container>::operator();

                typedef std::move_iterator <typename Container::iterator>
                    move_iterator;

                iterator_range <move_iterator>
                    operator() (Container && container) const
                {
                    return iterator_range <move_iterator> (
                        move_iterator (container.begin()),
                        move_iterator (container.end()));
                }
            };

        } // namespace detail

        /* Enable make_view for all standard containers. */

        // Sequence containers.

        template <bool Move, class Type, class Allocator, class Directions,
                class Range>
            struct make_view <Move,
                heavyweight_tag <std::vector <Type, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::vector <Type, Allocator>>> {};

        template <bool Move, class Type, class Allocator, class Directions,
                class Range>
            struct make_view <Move,
                heavyweight_tag <std::deque <Type, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::deque <Type, Allocator>>> {};

        // forward_list: only with front.
        template <bool Move, class Type, class Allocator, class Range>
            struct make_view <Move,
                heavyweight_tag <std::forward_list <Type, Allocator>>,
                meta::vector <direction::front>, Range>
        : helper::call_with_last <1, meta::vector <direction::front>,
            detail::view_std_container <Move,
                std::forward_list <Type, Allocator>>>
        {};

        template <bool Move, class Type, class Allocator, class Directions,
                class Range>
            struct make_view <Move,
                heavyweight_tag <std::list <Type, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::list <Type, Allocator>>> {};

        template <bool Move, class Type, class Traits, class Allocator,
                class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                    std::basic_string <Type, Traits, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::basic_string <Type, Traits, Allocator>>> {};

        // Associative containers.
        template <bool Move, class Key, class Compare, class Allocator,
                class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::set <Key, Compare, Allocator>>, Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::set <Key, Compare, Allocator>>>
        {};

        template <bool Move, class Key, class Compare, class Allocator,
                class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::multiset <Key, Compare, Allocator>>, Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::multiset <Key, Compare, Allocator>>>
        {};

        template <bool Move, class Key, class Value, class Compare,
                class Allocator, class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::map <Key, Value, Compare, Allocator>>, Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::map <Key, Value, Compare, Allocator>>>
        {};

        template <bool Move, class Key, class Value, class Compare,
                class Allocator, class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::multimap <Key, Value, Compare, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::multimap <Key, Value, Compare, Allocator>>>
        {};

        // Unordered (hashed) associative containers.
        template <bool Move, class Key, class Hash, class KeyEqual,
                class Allocator, class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::unordered_set <Key, Hash, KeyEqual, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::unordered_set <Key, Hash, KeyEqual, Allocator>>>
        {};

        template <bool Move, class Key, class Hash, class KeyEqual,
                class Allocator, class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::unordered_multiset <Key, Hash, KeyEqual, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::unordered_multiset <Key, Hash, KeyEqual, Allocator>>>
        {};

        template <bool Move, class Key, class Value, class Hash, class KeyEqual,
                class Allocator, class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::unordered_map <Key, Value, Hash, KeyEqual, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move, std::unordered_map <Key, Value, Hash, KeyEqual, Allocator>>>
        {};

        template <bool Move, class Key, class Value, class Hash, class KeyEqual,
                class Allocator, class Directions, class Range>
            struct make_view <Move, heavyweight_tag <
                std::unordered_multimap <
                    Key, Value, Hash, KeyEqual, Allocator>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            Move,
            std::unordered_multimap <Key, Value, Hash, KeyEqual, Allocator>>>
        {};

    } // namespace operation

} // namespace range

#endif  // RANGE_STD_CONTAINER_HPP_INCLUDED
