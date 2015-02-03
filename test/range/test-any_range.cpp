/*
Copyright 2013, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_any_range
#include "utility/test/boost_unit_test.hpp"

#include "range/any_range.hpp"

#include <list>
#include <vector>
#include <tuple>

#include "range/std.hpp"
#include "range/tuple.hpp"
#include "range/function_range.hpp"

#include "weird_count.hpp"
#include "unique_range.hpp"

using range::front;
using range::back;

using range::default_direction;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

using range::has;
namespace callable = range::callable;

using range::any_range;
using range::make_any_range;

BOOST_AUTO_TEST_SUITE(test_range_any_range)

BOOST_AUTO_TEST_CASE (test_any_range_has) {
    {
        typedef any_range <int, range::capability::unique_capabilities> range;
        // first and drop only available for rvalue references.
        static_assert (has <callable::empty (range const &)>::value, "");
        static_assert (!has <callable::first (range const &)>::value, "");
        static_assert (!has <callable::size (range const &)>::value, "");
        static_assert (!has <callable::drop (range const &)>::value, "");
        static_assert (!has <callable::drop (int, range const &)>::value, "");
        static_assert (has <callable::chop (range &&)>::value, "");
        static_assert (has <callable::chop_in_place (range &)>::value, "");

        static_assert (has <
            callable::empty (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::first (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::size (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::drop (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::drop (direction::back, int, range const &)>::value, "");
        static_assert (!has <
            callable::chop (direction::back, range &&)>::value, "");
        static_assert (!has <
            callable::chop_in_place (direction::back, range &)>::value, "");
    }
    {
        typedef any_range <int, range::capability::forward_capabilities> range;
        static_assert (has <callable::empty (range const &)>::value, "");
        static_assert (has <callable::first (range const &)>::value, "");
        static_assert (!has <callable::size (range const &)>::value, "");
        static_assert (has <callable::drop (range const &)>::value, "");
        static_assert (!has <callable::drop (int, range const &)>::value, "");
        static_assert (has <callable::chop (range &&)>::value, "");
        static_assert (has <callable::chop_in_place (range &)>::value, "");

        static_assert (has <
            callable::empty (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::first (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::size (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::drop (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::drop (direction::back, int, range const &)>::value, "");
        static_assert (!has <
            callable::chop (direction::back, range &&)>::value, "");
        static_assert (!has <
            callable::chop_in_place (direction::back, range &)>::value, "");
    }
    {
        typedef any_range <int, range::capability::bidirectional_capabilities>
            range;
        static_assert (has <callable::empty (range const &)>::value, "");
        static_assert (has <callable::first (range const &)>::value, "");
        static_assert (!has <callable::size (range const &)>::value, "");
        static_assert (has <callable::drop (range const &)>::value, "");
        static_assert (!has <callable::drop (int, range const &)>::value, "");
        static_assert (has <callable::chop (range &&)>::value, "");
        static_assert (has <callable::chop_in_place (range &)>::value, "");

        static_assert (has <
            callable::empty (direction::back, range const &)>::value, "");
        static_assert (has <
            callable::first (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::size (direction::back, range const &)>::value, "");
        static_assert (has <
            callable::drop (direction::back, range const &)>::value, "");
        static_assert (!has <
            callable::drop (direction::back, int, range const &)>::value, "");
        static_assert (has <
            callable::chop (direction::back, range &&)>::value, "");
        static_assert (has <
            callable::chop_in_place (direction::back, range &)>::value, "");
    }
    {
        typedef any_range <int, range::capability::random_access_capabilities>
            range;
        static_assert (has <callable::empty (range const &)>::value, "");
        static_assert (has <callable::first (range const &)>::value, "");
        static_assert (has <callable::size (range const &)>::value, "");
        static_assert (has <callable::drop (range const &)>::value, "");
        static_assert (has <callable::drop (int, range const &)>::value, "");
        static_assert (has <callable::chop (range &&)>::value, "");
        static_assert (has <callable::chop_in_place (range &)>::value, "");

        static_assert (has <
            callable::empty (direction::back, range const &)>::value, "");
        static_assert (has <
            callable::first (direction::back, range const &)>::value, "");
        static_assert (has <
            callable::size (direction::back, range const &)>::value, "");
        static_assert (has <
            callable::drop (direction::back, range const &)>::value, "");
        static_assert (has <
            callable::drop (direction::back, int, range const &)>::value, "");
        static_assert (has <
            callable::chop (direction::back, range &&)>::value, "");
        static_assert (has <
            callable::chop_in_place (direction::back, range &)>::value, "");
    }
}

BOOST_AUTO_TEST_CASE (test_any_range_homogeneous) {
    std::vector <int> v;
    v.push_back (4);
    v.push_back (5);
    v.push_back (6);
    v.push_back (7);

    {
        any_range <int &> a (v);

        BOOST_CHECK (!range::empty (a));
        BOOST_CHECK_EQUAL (range::first (a), 4);

        a = range::drop (a);
        BOOST_CHECK (!range::empty (a));
        auto chopped = range::chop (a);
        BOOST_CHECK_EQUAL (chopped.first(), 5);

        BOOST_CHECK (!range::empty (chopped.rest()));

        a = chopped.move_rest();
        BOOST_CHECK (!range::empty (a));
        BOOST_CHECK_EQUAL (range::first (a), 6);

        int & e = range::chop_in_place (a);
        BOOST_CHECK_EQUAL (e, 6);
        BOOST_CHECK_EQUAL (&e, &v [2]);

        BOOST_CHECK (!range::empty (a));
        BOOST_CHECK_EQUAL (range::first (a), 7);

        a = range::drop (a);
        BOOST_CHECK (range::empty (a));
    }
    {
        auto a = range::make_any_range (v);

        BOOST_MPL_ASSERT ((
            range::has <callable::empty (direction::back, decltype (a))>));
        BOOST_MPL_ASSERT ((
            range::has <callable::first (direction::front, decltype (a))>));
        BOOST_MPL_ASSERT ((
            range::has <callable::size (direction::back, decltype (a))>));
        BOOST_MPL_ASSERT ((
            range::has <callable::drop (direction::back, decltype (a))>));
        BOOST_MPL_ASSERT ((
            range::has <callable::drop (direction::front, int, decltype (a))>));

        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (size (a), 4u);
        BOOST_CHECK_EQUAL (size (drop (a)), 3u);
        BOOST_CHECK_EQUAL (first (drop (a)), 5);
        BOOST_CHECK_EQUAL (first (drop (2, a)), 6);
        BOOST_CHECK_EQUAL (first (back, a), 7);
        BOOST_CHECK_EQUAL (first (back, drop (back, a)), 6);
        BOOST_CHECK_EQUAL (first (back, drop (back, 2, a)), 5);
        BOOST_CHECK (empty (back, drop (4, a)));

        first (drop (a)) = 14;
        BOOST_CHECK_EQUAL (v[1], 14);

        // Convert to default capabilities

        any_range <int &> a2 (a);

        BOOST_CHECK (!empty (a2));

        a2 = drop (a2);
        BOOST_CHECK (!empty (a2));
        // Was 5, now 14.
        BOOST_CHECK_EQUAL (first (a2), 14);

        a2 = drop (a2);
        BOOST_CHECK (!empty (a2));
        BOOST_CHECK_EQUAL (first (a2), 6);

        a2 = drop (a2);
        BOOST_CHECK (!empty (a2));
        BOOST_CHECK_EQUAL (first (a2), 7);

        a2 = drop (a2);
        BOOST_CHECK (empty (a2));

        // Convert to different type: int & -> long should be fine.

        any_range <long> al (a);
        BOOST_CHECK (!empty (al));
        BOOST_CHECK_EQUAL (first (al), 4l);
        BOOST_CHECK_EQUAL (first (drop (al)), 14l);
        al = drop (drop (al));
        BOOST_CHECK (!empty (al));
        BOOST_CHECK_EQUAL (first (al), 6l);
        BOOST_CHECK (!empty (drop (al)));
        BOOST_CHECK_EQUAL (first (drop (al)), 7l);
        al = drop (drop (al));
        BOOST_CHECK (empty (al));
    }
}

BOOST_AUTO_TEST_CASE (test_any_range_unique) {
    std::vector <int> v;
    v.push_back (4);
    v.push_back (5);
    v.push_back (6);
    v.push_back (7);

    {
        auto a = make_any_range (unique_view (v));

        static_assert (!std::is_constructible <
            decltype (a), decltype (a) const &>::value, "");

        BOOST_CHECK_EQUAL (first (a), 4);
        a = drop (std::move (a));
        BOOST_CHECK_EQUAL (chop_in_place (a), 5);
        auto b = std::move (a);
        BOOST_CHECK_EQUAL (chop_in_place (b), 6);
        BOOST_CHECK_EQUAL (chop_in_place (b), 7);
        BOOST_CHECK (empty (b));
    }
    {
        any_range <int, range::capability::unique_capabilities> a (
            one_time_view (v));
        // first and drop are not available.
        BOOST_CHECK_EQUAL (chop_in_place (a), 4);

        auto chopped = chop (std::move (a));
        BOOST_CHECK_EQUAL (chopped.first(), 5);
        a = chopped.move_rest();

        BOOST_CHECK_EQUAL (chop_in_place (a), 6);
        auto b = std::move (a);
        BOOST_CHECK_EQUAL (chop_in_place (b), 7);
        BOOST_CHECK (empty (b));
    }
    {
        struct count {
            int i;
            count() : i (0) {}

            int operator() () { return ++i; }
        };

        any_range <int, range::capability::unique_capabilities> a (
            range::make_function_range (count()));

        BOOST_CHECK_EQUAL (chop_in_place (a), 1);
        BOOST_CHECK_EQUAL (chop_in_place (a), 2);
    }
}

BOOST_AUTO_TEST_CASE (test_any_range_heterogeneous) {
    {
        std::tuple <> t;
        any_range <int> a (t);
        BOOST_CHECK (empty (a));
    }
    {
        std::tuple <int> t (7);
        any_range <int> a (t);
        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (first (a), 7);
        any_range <int> a_next = drop (a);
        BOOST_CHECK (empty (a_next));
    }
    {
        std::tuple <int, char, long> t (7, 'a', 294l);
        any_range <long> a (t);
        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (first (a), 7l);
        a = drop (a);
        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (first (a), long ('a'));
        a = drop (a);
        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (first (a), 294l);
        a = drop (a);
        BOOST_CHECK (empty (a));
    }
    {
        std::tuple <int, char, long> t (7, 'a', 294l);
        any_range <long, range::capability::bidirectional_capabilities> a (t);
        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (chop_in_place (a), 7l);
        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (chop_in_place (back, a), 294l);
        BOOST_CHECK (!empty (a));
        BOOST_CHECK_EQUAL (chop_in_place (back, a), long ('a'));
        BOOST_CHECK (empty (a));
    }
}

BOOST_AUTO_TEST_CASE (test_any_range_copy_move) {
    typedef any_range <int, meta::map <
        meta::map_element <range::capability::default_direction,
            direction::front>,
        meta::map_element <direction::front, meta::set <
            range::capability::empty, range::capability::size,
                range::capability::first>>>>
        range_with_size;

    typedef any_range <int, meta::map <
        meta::map_element <range::capability::default_direction,
            direction::front>,
        meta::map_element <direction::front, meta::set <
            range::capability::empty, range::capability::first>>>>
        range_without_size;

    std::vector <int> v;
    v.push_back (26);

    static_assert (
        !std::is_constructible <range_with_size, range_with_size const &
        >::value, "The range cannot be copied so it is not copy-constructible");
    static_assert (
        !std::is_constructible <range_without_size, range_with_size const &
        >::value, "The range cannot be copied so it is not copy-constructible");

    static_assert (
        std::is_constructible <range_with_size, range_with_size &&>::value,
        "Moving to the same type is a pointer operation.");
    static_assert (
        !std::is_constructible <range_without_size, range_with_size &&>::value,
        "The range cannot be copied so it is not move-constructible "
        "with different capabilities.");

    range_with_size r (v);
    range_with_size r2 (std::move (r));
    BOOST_CHECK_EQUAL (size (r2), 1);
}

BOOST_AUTO_TEST_SUITE_END()
