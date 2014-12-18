/*
Copyright 2013, 2014 Rogier van Dalen.

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
#include "utility/test/boost_unit_test.hpp"

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

#include "utility/test/tracked.hpp"

#include "rime/check/check_equal.hpp"
#include "../check_equal_behaviour.hpp"

BOOST_AUTO_TEST_SUITE(test_container_std_adaptor)

using range::empty;
using range::size;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;
using range::at;
using range::at_c;
using range::second;
using range::third;

using range::view;
using range::view_once;

using range::front;
using range::back;

using range::is_view;
using range::is_homogeneous;
using range::always_empty;
using range::never_empty;
using range::has;

namespace callable = range::callable;

rime::size_t <0> zero;
rime::size_t <1> one;
rime::size_t <2> two;

using rime::false_type;
using rime::true_type;

using utility::tracked;

BOOST_AUTO_TEST_CASE (test_std_vector_adaptor) {
    std::vector <int> v;

    {
        auto view = range::view (v);
        auto other_view = range::view_once (v);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (view), decltype (other_view)>));

        BOOST_MPL_ASSERT ((has <callable::empty (decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::empty (decltype (v))>));
        BOOST_MPL_ASSERT_NOT ((always_empty <direction::front, decltype (v)>));
        BOOST_MPL_ASSERT_NOT ((never_empty <direction::front, decltype (v)>));

        BOOST_CHECK (empty (v));
        BOOST_CHECK_EQUAL (size (v), 0);

        BOOST_MPL_ASSERT_NOT ((has <callable::chop_in_place (decltype (v))>));
        BOOST_MPL_ASSERT_NOT ((
            has <callable::chop_in_place (decltype (view))>));
    }

    v.push_back (5);

    {
        BOOST_CHECK (!empty (v));
        BOOST_CHECK_EQUAL (size (v), 1);

        BOOST_CHECK_EQUAL (first (v), 5);
        BOOST_CHECK_EQUAL (first (front, v), 5);
        BOOST_CHECK_EQUAL (first (back, v), 5);

        BOOST_CHECK_EQUAL (at (0, v), 5);
        BOOST_CHECK_EQUAL (at (front, 0, v), 5);
        BOOST_CHECK_EQUAL (at (back, 0, v), 5);
        BOOST_CHECK_EQUAL (at (zero, v), 5);
        BOOST_CHECK_EQUAL (at (front, zero, v), 5);
        BOOST_CHECK_EQUAL (at (back, zero, v), 5);

        BOOST_CHECK (empty (drop (v)));
        BOOST_CHECK (empty (drop (1, v)));
        BOOST_CHECK (empty (drop (one, v)));

        auto first_and_empty = chop (v);
        BOOST_CHECK_EQUAL (first_and_empty.first(), 5);
        BOOST_CHECK (empty (first_and_empty.rest()));

        auto mutated = view (v);
        BOOST_CHECK (!empty (mutated));
        BOOST_CHECK_EQUAL (chop_in_place (mutated), 5);
        BOOST_CHECK (empty (mutated));
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

        BOOST_CHECK_EQUAL (at (0, v), 5);
        BOOST_CHECK_EQUAL (at (1, v), 6);
        BOOST_CHECK_EQUAL (at (2, v), 7);
        BOOST_CHECK_EQUAL (at (front, 0, v), 5);
        BOOST_CHECK_EQUAL (at (front, 1, v), 6);
        BOOST_CHECK_EQUAL (at (front, 2, v), 7);
        BOOST_CHECK_EQUAL (at (back, 0, v), 7);
        BOOST_CHECK_EQUAL (at (back, 1, v), 6);
        BOOST_CHECK_EQUAL (at (back, 2, v), 5);

        BOOST_CHECK_EQUAL (at_c <0> (v), 5);
        BOOST_CHECK_EQUAL (at_c <1> (v), 6);
        BOOST_CHECK_EQUAL (at_c <2> (v), 7);
        BOOST_CHECK_EQUAL (at_c <0> (front, v), 5);
        BOOST_CHECK_EQUAL (at_c <1> (front, v), 6);
        BOOST_CHECK_EQUAL (at_c <2> (front, v), 7);
        BOOST_CHECK_EQUAL (at_c <0> (back, v), 7);
        BOOST_CHECK_EQUAL (at_c <1> (back, v), 6);
        BOOST_CHECK_EQUAL (at_c <2> (back, v), 5);

        BOOST_CHECK_EQUAL (second (v), 6);
        BOOST_CHECK_EQUAL (third (v), 7);
        BOOST_CHECK_EQUAL (second (front, v), 6);
        BOOST_CHECK_EQUAL (third (front, v), 7);
        BOOST_CHECK_EQUAL (second (back, v), 6);
        BOOST_CHECK_EQUAL (third (back, v), 5);

        BOOST_CHECK_EQUAL (at (two, v), 7);

        auto first_and_empty = chop (v);
        BOOST_CHECK_EQUAL (first_and_empty.first(), 5);
        BOOST_CHECK_EQUAL (first (first_and_empty.rest()), 6);

        auto last_and_empty = chop (back, v);
        BOOST_CHECK_EQUAL (last_and_empty.first(), 7);
        BOOST_CHECK_EQUAL (first (back, last_and_empty.rest()), 6);

        auto mutated = view (v);
        BOOST_CHECK (!empty (mutated));
        BOOST_CHECK_EQUAL (chop_in_place (mutated), 5);
        BOOST_CHECK (!empty (mutated));
        BOOST_CHECK_EQUAL (chop_in_place (mutated), 6);
        BOOST_CHECK (!empty (mutated));
        BOOST_CHECK_EQUAL (chop_in_place (mutated), 7);
        BOOST_CHECK (empty (mutated));
    }

    utility::tracked_registry r;
    {
        // Use view_once: elements are returned by rvalue reference.
        std::vector <tracked <int>> c;
        c.reserve (2);
        c.push_back (tracked <int> (r, 7));
        c.push_back (tracked <int> (r, 45));

        // Check the status quo.
        RIME_CHECK_EQUAL (first (c).content(), 7);
        RIME_CHECK_EQUAL (first (back, c).content(), 45);
        r.check_counts (2, 0, 2, 0, 0, 0, 0, 2);

        auto v = view_once (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (v)), tracked <int> &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (back, v)), tracked <int> &&>));

        // The elements should be moved out.
        tracked <int> i = at (0, v);
        BOOST_CHECK_EQUAL (i.content(), 7);
        r.check_counts (2, 0, 3, 0, 0, 0, 0, 2);

        tracked <int> d = at (1, v);
        BOOST_CHECK_EQUAL (d.content(), 45);
        r.check_counts (2, 0, 4, 0, 0, 0, 0, 2);
    }
}

BOOST_AUTO_TEST_CASE (test_std_list_adaptor) {
    std::list <int> l;

    auto view = range::view (l);
    static_assert (has <callable::empty (decltype (view))>::value, "");
    static_assert (!has <callable::size (decltype (view))>::value, "");

    static_assert (has <callable::first (decltype (view))>::value, "");
    // second (l) is equivalent to first (drop (l)) and should therefore be
    // available.
    static_assert (has <callable::second (decltype (view))>::value, "");
    static_assert (!has <callable::third (decltype (view))>::value, "");
    static_assert (!has <callable::seventh (decltype (view))>::value, "");

    static_assert (!has <callable::at (decltype (view))>::value, "");
}

template <class Type>
    void compare_sequence_containers (std::vector <Type> const & v)
{
    std::deque <Type> d (v.begin(), v.end());
    std::forward_list <Type> fl (v.begin(), v.end());
    std::list <Type> l (v.begin(), v.end());

    BOOST_MPL_ASSERT_NOT ((is_view <std::deque <Type>>));
    BOOST_MPL_ASSERT_NOT ((is_homogeneous <std::deque <Type>>));
    BOOST_MPL_ASSERT ((is_view <decltype (view (d))>));
    BOOST_MPL_ASSERT ((is_homogeneous <decltype (view (d)) &>));

    BOOST_MPL_ASSERT_NOT ((is_view <std::list <Type> &>));
    BOOST_MPL_ASSERT_NOT ((is_homogeneous <std::forward_list <Type>>));
    BOOST_MPL_ASSERT ((is_view <decltype (view (fl))>));
    BOOST_MPL_ASSERT ((is_homogeneous <decltype (view (l)) const>));

    check_equal_behaviour <true_type, true_type, false_type, true_type> (v, v);

    // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <true_type, true_type, false_type, true_type> (d, v);
    check_equal_behaviour <false_type, false_type, false_type, false_type> (
        fl, v);
    check_equal_behaviour <false_type, true_type, false_type, false_type> (
        l, v);
}

template <class Type> void compare_string (std::vector <Type> const & v) {
    std::basic_string <Type> s (v.begin(), v.end());

    check_equal_behaviour <true_type, true_type, false_type, true_type> (v, v);

    // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <true_type, true_type, false_type, true_type> (s, v);
}

template <class HasBack, class OtherContainer, class MultiContainer>
    void compare_associative_containers (MultiContainer const & ms)
{
    typedef typename MultiContainer::value_type value_type;
    {
        std::vector <value_type> v (ms.begin(), ms.end());

        // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
        check_equal_behaviour <false_type, HasBack, false_type, false_type> (
            ms, v);
    }
    {
        // Convert multiset to set (removing
        OtherContainer s (ms.begin(), ms.end());
        std::vector <value_type> v (s.begin(), s.end());

        // Parameters: HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
        check_equal_behaviour <false_type, HasBack, false_type, false_type> (
            s, v);
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
        compare_associative_containers <true_type, std::set <int>> (s);

        s.insert (12);
        compare_associative_containers <true_type, std::set <int>> (s);

        s.insert (14);
        compare_associative_containers <true_type, std::set <int>> (s);
        s.insert (14);
        compare_associative_containers <true_type, std::set <int>> (s);

        s.insert (17);
        compare_associative_containers <true_type, std::set <int>> (s);

        s.insert (14);
        compare_associative_containers <true_type, std::set <int>> (s);

        s.insert (20);
        compare_associative_containers <true_type, std::set <int>> (s);
        s.insert (20);
        compare_associative_containers <true_type, std::set <int>> (s);
    }

    // Maps.
    {
        std::multimap <int, char> s;
        compare_associative_containers <true_type, std::map <int, char>> (s);

        s.insert (std::make_pair (14, 'b'));
        compare_associative_containers <true_type, std::map <int, char>> (s);

        s.insert (std::make_pair (12, 'c'));
        compare_associative_containers <true_type, std::map <int, char>> (s);
        s.insert (std::make_pair (14, 'a'));
        compare_associative_containers <true_type, std::map <int, char>> (s);

        s.insert (std::make_pair (17, 'd'));
        compare_associative_containers <true_type, std::map <int, char>> (s);

        s.insert (std::make_pair (14, 'e'));
        compare_associative_containers <true_type, std::map <int, char>> (s);

        s.insert (std::make_pair (20, 'f'));
        compare_associative_containers <true_type, std::map <int, char>> (s);
        s.insert (std::make_pair (20, 'g'));
        compare_associative_containers <true_type, std::map <int, char>> (s);
    }

    // Unordered sets.
    {
        std::unordered_multiset <int> s;
        typedef std::unordered_set <int> other_container;
        compare_associative_containers <false_type, other_container> (s);

        s.insert (12);
        compare_associative_containers <false_type, other_container> (s);

        s.insert (14);
        compare_associative_containers <false_type, other_container> (s);
        s.insert (14);
        compare_associative_containers <false_type, other_container> (s);

        s.insert (17);
        compare_associative_containers <false_type, other_container> (s);

        s.insert (14);
        compare_associative_containers <false_type, other_container> (s);

        s.insert (20);
        compare_associative_containers <false_type, other_container> (s);
        s.insert (20);
        compare_associative_containers <false_type, other_container> (s);
    }

    // Unordered maps.
    {
        std::unordered_multimap <int, char> s;
        typedef std::unordered_map <int, char> other_container;
        compare_associative_containers <false_type, other_container> (s);

        s.insert (std::make_pair (14, 'b'));
        compare_associative_containers <false_type, other_container> (s);

        s.insert (std::make_pair (12, 'c'));
        compare_associative_containers <false_type, other_container> (s);
        s.insert (std::make_pair (14, 'a'));
        compare_associative_containers <false_type, other_container> (s);

        s.insert (std::make_pair (17, 'd'));
        compare_associative_containers <false_type, other_container> (s);

        s.insert (std::make_pair (14, 'e'));
        compare_associative_containers <false_type, other_container> (s);

        s.insert (std::make_pair (20, 'f'));
        compare_associative_containers <false_type, other_container> (s);
        s.insert (std::make_pair (20, 'g'));
        compare_associative_containers <false_type, other_container> (s);
    }
}

// Test that there is no difference between a container and its view regarding
// the const- and reference-qualification and calling std::begin() and
// std::end().

template <class Type> struct type {};

template <class Container>
    auto get_iterator_type (Container &&)
RETURNS (type <typename std::decay <
    decltype (std::begin (std::declval <Container>()))>::type>());

BOOST_AUTO_TEST_CASE (test_std_container_const) {
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (get_iterator_type (std::vector <int>())),
        decltype (get_iterator_type (view (std::vector <int>())))
        >));

    BOOST_MPL_ASSERT ((std::is_same <
        decltype (get_iterator_type (view_once (std::vector <int>()))),
        type <std::move_iterator <std::vector <int>::iterator>>
        >));

    {
        std::vector <int> v;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (get_iterator_type (v)),
            decltype (get_iterator_type (view (v)))
            >));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (get_iterator_type (v)),
            decltype (get_iterator_type (view_once (v)))
            >));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (get_iterator_type (std::move (v))),
            decltype (get_iterator_type (view (std::move (v))))
            >));
    }

    {
        std::vector <int> const v;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (get_iterator_type (v)),
            decltype (get_iterator_type (view (v)))
            >));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (get_iterator_type (v)),
            decltype (get_iterator_type (view_once (v)))
            >));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (get_iterator_type (std::move (v))),
            decltype (get_iterator_type (view (std::move (v))))
            >));
    }
}

BOOST_AUTO_TEST_SUITE_END()
