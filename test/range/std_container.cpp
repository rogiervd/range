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

#define BOOST_TEST_MODULE test_container_std_adaptor
#include "../boost_unit_test.hpp"

#include "range/std.hpp"

#include <vector>
#include <string>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "../range/check_equal_behaviour.hpp"

BOOST_AUTO_TEST_SUITE(test_container_std_adaptor)

BOOST_AUTO_TEST_CASE (test_std_vector_adaptor) {
    using range::empty;
    using range::size;
    using range::first;
    using range::drop;
    using range::front;
    using range::back;

    std::vector <int> v;

    {
        auto view = range::view (v);

        BOOST_MPL_ASSERT ((range::has::empty <decltype (view)>));
        BOOST_MPL_ASSERT ((range::has::empty <decltype (v)>));

        BOOST_CHECK (empty (v));
        BOOST_CHECK_EQUAL (size (v), 0);
    }

    v.push_back (5);

    {
        BOOST_CHECK (!empty (v));
        BOOST_CHECK_EQUAL (size (v), 1);
        BOOST_CHECK_EQUAL (first (v), 5);
        BOOST_CHECK_EQUAL (first (front, v), 5);
        BOOST_CHECK_EQUAL (first (back, v), 5);
        BOOST_CHECK (empty (drop (v)));
        BOOST_CHECK (empty (drop (1, v)));
    }

    v.push_back (6);
    v.push_back (7);

    {
        BOOST_CHECK (!empty (v));
        BOOST_CHECK_EQUAL (size (v), 3);
        BOOST_CHECK_EQUAL (first (v), 5);
        BOOST_CHECK (!empty (drop (v)));
        BOOST_CHECK (empty (drop (3, v)));
        BOOST_CHECK (empty (drop (back, 3, v)));
        BOOST_CHECK (empty (drop (front, 1, drop (back, 2, v))));
        BOOST_CHECK_EQUAL (first (front, drop (front, v)), 6);
        BOOST_CHECK_EQUAL (first (back, v), 7);
    }
}

template <class Type>
    void compare_sequence_containers (std::vector <Type> const & v)
{
    std::deque <Type> d (v.begin(), v.end());
    std::forward_list <Type> fl (v.begin(), v.end());
    std::list <Type> l (v.begin(), v.end());

    BOOST_MPL_ASSERT_NOT ((range::is_view <std::deque <Type>>));
    BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <std::deque <Type>>));
    BOOST_MPL_ASSERT ((range::is_view <decltype (range::view (d))>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <decltype (range::view (d)) &>));

    BOOST_MPL_ASSERT_NOT ((range::is_view <std::list <Type> &>));
    BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <std::forward_list <Type>>));
    BOOST_MPL_ASSERT ((range::is_view <decltype (range::view (fl))>));
    BOOST_MPL_ASSERT ((
        range::is_homogeneous <decltype (range::view (l)) const>));

    check_equal_behaviour <rime::true_type, rime::true_type,
        rime::false_type, rime::true_type> (v, v);

    // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <rime::true_type, rime::true_type,
        rime::false_type, rime::true_type> (d, v);
    check_equal_behaviour <rime::false_type, rime::false_type,
        rime::false_type, rime::false_type> (fl, v);
    check_equal_behaviour <rime::false_type, rime::true_type,
        rime::false_type, rime::false_type> (l, v);
}

template <class Type> void compare_string (std::vector <Type> const & v) {
    std::basic_string <Type> s (v.begin(), v.end());

    check_equal_behaviour <rime::true_type, rime::true_type,
        rime::false_type, rime::true_type> (v, v);

    // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <rime::true_type, rime::true_type,
        rime::false_type, rime::true_type> (s, v);
}

template <class HasBack, class OtherContainer, class MultiContainer>
    void compare_associative_containers (MultiContainer const & ms)
{
    typedef typename MultiContainer::value_type value_type;
    {
        std::vector <value_type> v (ms.begin(), ms.end());

        // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
        check_equal_behaviour <rime::false_type, HasBack,
            rime::false_type, rime::false_type> (ms, v);
    }
    {
        // Convert multiset to set (removing
        OtherContainer s (ms.begin(), ms.end());
        std::vector <value_type> v (s.begin(), s.end());

        // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
        check_equal_behaviour <rime::false_type, HasBack,
            rime::false_type, rime::false_type> (s, v);
    }
}

BOOST_AUTO_TEST_CASE (test_other_homogeneous_containers) {
    {
        std::vector <int> v;
        compare_sequence_containers (v);

        v.push_back (12);
        compare_sequence_containers (v);

        v.push_back (14);
        compare_sequence_containers (v);

        v.push_back (17);
        compare_sequence_containers (v);

        v.push_back (20);
        compare_sequence_containers (v);
    }
    {
        std::vector <char> v;
        compare_string (v);

        v.push_back ('a');
        compare_string (v);

        v.push_back ('C');
        compare_string (v);

        v.push_back (char (0));
        compare_string (v);
    }

    // Sets.
    {
        std::multiset <int> s;
        compare_associative_containers <rime::true_type, std::set <int>> (s);

        s.insert (12);
        compare_associative_containers <rime::true_type, std::set <int>> (s);

        s.insert (14);
        compare_associative_containers <rime::true_type, std::set <int>> (s);
        s.insert (14);
        compare_associative_containers <rime::true_type, std::set <int>> (s);

        s.insert (17);
        compare_associative_containers <rime::true_type, std::set <int>> (s);

        s.insert (14);
        compare_associative_containers <rime::true_type, std::set <int>> (s);

        s.insert (20);
        compare_associative_containers <rime::true_type, std::set <int>> (s);
        s.insert (20);
        compare_associative_containers <rime::true_type, std::set <int>> (s);
    }

    // Maps.
    {
        std::multimap <int, char> s;
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);

        s.insert (std::make_pair (14, 'b'));
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);

        s.insert (std::make_pair (12, 'c'));
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);
        s.insert (std::make_pair (14, 'a'));
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);

        s.insert (std::make_pair (17, 'd'));
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);

        s.insert (std::make_pair (14, 'e'));
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);

        s.insert (std::make_pair (20, 'f'));
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);
        s.insert (std::make_pair (20, 'g'));
        compare_associative_containers <rime::true_type,
            std::map <int, char>> (s);
    }

    // Unordered sets.
    {
        std::unordered_multiset <int> s;
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);

        s.insert (12);
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);

        s.insert (14);
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);
        s.insert (14);
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);

        s.insert (17);
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);

        s.insert (14);
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);

        s.insert (20);
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);
        s.insert (20);
        compare_associative_containers <rime::false_type,
            std::unordered_set <int>> (s);
    }

    // Unordered maps.
    {
        std::unordered_multimap <int, char> s;
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);

        s.insert (std::make_pair (14, 'b'));
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);

        s.insert (std::make_pair (12, 'c'));
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);
        s.insert (std::make_pair (14, 'a'));
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);

        s.insert (std::make_pair (17, 'd'));
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);

        s.insert (std::make_pair (14, 'e'));
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);

        s.insert (std::make_pair (20, 'f'));
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);
        s.insert (std::make_pair (20, 'g'));
        compare_associative_containers <rime::false_type,
            std::unordered_map <int, char>> (s);
    }
}

BOOST_AUTO_TEST_SUITE_END()

