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
        struct tag_of_unqualified <std::vector <Type, Allocator>>
    { typedef heavyweight_tag <std::vector <Type, Allocator>> type; };

    template <class Type, class Allocator>
        struct tag_of_unqualified <std::deque <Type, Allocator>>
    { typedef heavyweight_tag <std::deque <Type, Allocator>> type; };

    template <class Type, class Allocator>
        struct tag_of_unqualified <std::forward_list <Type, Allocator>>
    { typedef heavyweight_tag <std::forward_list <Type, Allocator>> type; };

    template <class Type, class Allocator>
        struct tag_of_unqualified <std::list <Type, Allocator>>
    { typedef heavyweight_tag <std::list <Type, Allocator>> type; };

    template <class Type, class Traits, class Allocator>
        struct tag_of_unqualified <std::basic_string <Type, Traits, Allocator>>
    {
        typedef heavyweight_tag <std::basic_string <Type, Traits, Allocator>>
            type;
    };

    // Associative containers.
    template <class Key, class Compare, class Allocator>
        struct tag_of_unqualified <std::set <Key, Compare, Allocator>>
    { typedef heavyweight_tag <std::set <Key, Compare, Allocator>> type; };

    template <class Key, class Compare, class Allocator>
        struct tag_of_unqualified <std::multiset <Key, Compare, Allocator>>
    { typedef heavyweight_tag <std::multiset <Key, Compare, Allocator>> type; };

    template <class Key, class Value, class Compare, class Allocator>
    struct tag_of_unqualified <std::map <Key, Value, Compare, Allocator>> {
        typedef heavyweight_tag <std::map <Key, Value, Compare, Allocator>>
            type;
    };

    template <class Key, class Value, class Compare, class Allocator>
    struct tag_of_unqualified <std::multimap <Key, Value, Compare, Allocator>> {
        typedef heavyweight_tag <std::multimap <Key, Value, Compare, Allocator>>
            type;
    };

    // Unordered associative containers.
    template <class Key, class Hash, class KeyEqual, class Allocator>
        struct tag_of_unqualified <
            std::unordered_set <Key, Hash, KeyEqual, Allocator>>
    {
        typedef heavyweight_tag <std::unordered_set <
            Key, Hash, KeyEqual, Allocator>> type;
    };

    template <class Key, class Hash, class KeyEqual, class Allocator>
        struct tag_of_unqualified <
            std::unordered_multiset <Key, Hash, KeyEqual, Allocator>>
    {
        typedef heavyweight_tag <std::unordered_multiset <
            Key, Hash, KeyEqual, Allocator>> type;
    };

    template <class Key, class Value, class Hash, class KeyEqual,
            class Allocator>
        struct tag_of_unqualified <
            std::unordered_map <Key, Value, Hash, KeyEqual, Allocator>>
    {
        typedef heavyweight_tag <std::unordered_map <
            Key, Value, Hash, KeyEqual, Allocator>> type;
    };

    template <class Key, class Value, class Hash, class KeyEqual,
            class Allocator>
        struct tag_of_unqualified <
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
            template <class Container> struct view_std_container
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

        } // namespace detail

        /* Enable make_view for all standard containers. */

        // Sequence containers.

        template <class Type, class Allocator, class Directions>
            struct make_view <heavyweight_tag <std::vector <Type, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::vector <Type, Allocator>>> {};

        template <class Type, class Allocator, class Directions>
            struct make_view <heavyweight_tag <std::deque <Type, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::deque <Type, Allocator>>> {};

        // forward_list: only with front.
        template <class Type, class Allocator>
            struct make_view <heavyweight_tag <
                    std::forward_list <Type, Allocator>>,
                meta::vector <direction::front>>
        : helper::call_with_last <1, meta::vector <direction::front>,
            detail::view_std_container <std::forward_list <Type, Allocator>>>
        {};

        template <class Type, class Allocator, class Directions>
            struct make_view <heavyweight_tag <std::list <Type, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::list <Type, Allocator>>> {};

        template <class Type, class Traits, class Allocator, class Directions>
            struct make_view <heavyweight_tag <
                    std::basic_string <Type, Traits, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::basic_string <Type, Traits, Allocator>>> {};

        // Associative containers.
        template <class Key, class Compare, class Allocator, class Directions>
            struct make_view <heavyweight_tag <
                std::set <Key, Compare, Allocator>>, Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::set <Key, Compare, Allocator>>>
        {};

        template <class Key, class Compare, class Allocator, class Directions>
            struct make_view <heavyweight_tag <
                std::multiset <Key, Compare, Allocator>>, Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::multiset <Key, Compare, Allocator>>>
        {};

        template <class Key, class Value, class Compare, class Allocator,
                class Directions>
            struct make_view <heavyweight_tag <
                std::map <Key, Value, Compare, Allocator>>, Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::map <Key, Value, Compare, Allocator>>>
        {};

        template <class Key, class Value, class Compare, class Allocator,
                class Directions>
            struct make_view <heavyweight_tag <
                std::multimap <Key, Value, Compare, Allocator>>, Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::multimap <Key, Value, Compare, Allocator>>>
        {};

        // Unordered (hashed) associative containers.
        template <class Key, class Hash, class KeyEqual, class Allocator,
                class Directions>
            struct make_view <heavyweight_tag <
                std::unordered_set <Key, Hash, KeyEqual, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::unordered_set <Key, Hash, KeyEqual, Allocator>>>
        {};

        template <class Key, class Hash, class KeyEqual, class Allocator,
            class Directions>
            struct make_view <heavyweight_tag <
                std::unordered_multiset <Key, Hash, KeyEqual, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::unordered_multiset <Key, Hash, KeyEqual, Allocator>>>
        {};

        template <class Key, class Value, class Hash, class KeyEqual,
                class Allocator, class Directions>
            struct make_view <heavyweight_tag <
                std::unordered_map <Key, Value, Hash, KeyEqual, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::unordered_map <Key, Value, Hash, KeyEqual, Allocator>>>
        {};

        template <class Key, class Value, class Hash, class KeyEqual,
                class Allocator, class Directions>
            struct make_view <heavyweight_tag <
                std::unordered_multimap <
                    Key, Value, Hash, KeyEqual, Allocator>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions, detail::view_std_container <
            std::unordered_multimap <Key, Value, Hash, KeyEqual, Allocator>>>
        {};

    } // namespace operation

} // namespace range

#endif  // RANGE_STD_CONTAINER_HPP_INCLUDED

