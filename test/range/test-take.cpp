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

#define BOOST_TEST_MODULE test_range_take
#include "utility/test/boost_unit_test.hpp"

#include "range/take.hpp"

#include <vector>
#include <tuple>
#include <type_traits>

#include "range/std.hpp"
#include "range/count.hpp"
#include "range/function_range.hpp"

#include "check_equal.hpp"
#include "check_equal_behaviour.hpp"


BOOST_AUTO_TEST_SUITE(test_range_take)

using range::take;

using range::is_homogeneous;
using range::default_direction;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::front;
using range::back;
using range::count;

rime::size_t <0> zero;
rime::size_t <1> one;
rime::size_t <2> two;
rime::size_t <3> three;

BOOST_AUTO_TEST_CASE (test_range_take_vector_runtime) {
    std::vector <int> v;
    std::vector <int> reference;

    {
        // The obviously sensible implementation of take uses an iterator_range
        // for this:
        auto view = range::view (reference);
        auto first = take (3, reference);
        BOOST_MPL_ASSERT ((std::is_same <decltype (view), decltype (first)>));
        BOOST_CHECK (empty (first));
    }

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (0), v), range::view (v));
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::false_type, rime::true_type> (
            take (std::size_t (1), v), range::view (v));
    {
        auto take_back = take (back, std::size_t (0), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }
    {
        auto take_back = take (back, std::size_t (1), v);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }

    v.push_back (7);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (0), v), reference);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (1), v), v);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (2), v), v);
    {
        auto take_back = take (back, std::size_t (0), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }
    {
        auto take_back = take (back, std::size_t (1), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 7);
        RIME_CHECK_EQUAL (empty (back, drop (take_back)), true);
    }
    {
        auto take_back = take (back, std::size_t (5), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 7);
        RIME_CHECK_EQUAL (empty (back, drop (take_back)), true);
    }

    v.push_back (27);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (0), v), reference);
    reference.push_back (7);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (1), v), reference);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (2), v), v);
    reference.push_back (27);

    {
        auto take_back = take (back, std::size_t (0), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }
    {
        auto take_back = take (back, std::size_t (1), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 27);
        RIME_CHECK_EQUAL (empty (back, drop (take_back)), true);
    }
    {
        auto take_back = take (back, std::size_t (5), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 27);

        auto take_back_next = drop (back, take_back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next)>));
        RIME_CHECK_EQUAL (empty (take_back_next), false);
        BOOST_CHECK_EQUAL (first (back, take_back_next), 7);

        RIME_CHECK_EQUAL (empty (drop (take_back_next)), true);
    }

    v.push_back (37);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (2), v), reference);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (4), v), v);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (front, std::size_t (3), v), v);
    {
        auto take_back = take (back, std::size_t (2), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 37);

        auto take_back_next_2 = drop (2, take_back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (take_back_next_2), true);
    }
    {
        auto take_back = take (back, std::size_t (3), v);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 37);

        auto take_back_next_2 = drop (back, 2, take_back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (take_back_next_2), false);
        BOOST_CHECK_EQUAL (first (back, take_back_next_2), 7);

        RIME_CHECK_EQUAL (empty (drop (back, take_back_next_2)), true);

        RIME_CHECK_EQUAL (empty (drop (back, 3, take_back)), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_list_runtime) {
    std::list <int> l;
    std::list <int> reference;

    {
        // Unlike in the case of std::vector, take can't return an
        // iterator_range here:
        auto view = range::view (l);
        auto first = take (3, l);
        BOOST_MPL_ASSERT_NOT ((
            std::is_same <decltype (view), decltype (first)>));
        RIME_CHECK_EQUAL (empty (first), true);

        // Second take: the type does not change.
        auto first_2 = take (2, first);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (first), decltype (first_2)>));
        RIME_CHECK_EQUAL (empty (first_2), true);
    }

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (0), l), range::view (l));
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (std::size_t (1), l), range::view (l));
    {
        auto take_back = take (back, std::size_t (0), l);
        BOOST_CHECK (default_direction (take_back) == front);
        typedef range::tag_of <decltype (take_back)>::type tag;
        range::operation::empty <tag, direction::back> callable;
        callable (back, take_back);
        RIME_CHECK_EQUAL (empty (back, take_back), true);
    }
    {
        auto take_back = take (back, std::size_t (1), l);
        RIME_CHECK_EQUAL (empty (back, take_back), true);
    }

    l.push_back (7);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (0), l), reference);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (1), l), l);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (2), l), l);

    {
        auto take_back = take (back, std::size_t (0), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), true);
    }
    {
        auto take_back = take (back, std::size_t (1), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 7);
        RIME_CHECK_EQUAL (empty (back, drop (back, take_back)), true);
    }
    {
        auto take_back = take (back, std::size_t (5), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 7);
        RIME_CHECK_EQUAL (empty (back, drop (back, take_back)), true);
    }

    l.push_back (27);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (0), l), reference);
    reference.push_back (7);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (1), l), reference);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (2), l), l);
    reference.push_back (27);

    {
        auto take_back = take (back, std::size_t (0), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), true);
    }
    {
        auto take_back = take (back, std::size_t (1), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 27);
        RIME_CHECK_EQUAL (empty (back, drop (back, take_back)), true);
    }
    {
        auto take_back = take (back, std::size_t (5), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 27);

        auto take_back_next = drop (back, take_back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next)>));
        RIME_CHECK_EQUAL (empty (back, take_back_next), false);
        BOOST_CHECK_EQUAL (first (back, take_back_next), 7);

        RIME_CHECK_EQUAL (empty (back, drop (back, take_back_next)), true);
    }

    l.push_back (37);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (2), l), reference);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (4), l), l);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (front, std::size_t (3), l), l);
    {
        auto take_back = take (back, std::size_t (2), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 37);

        auto take_back_next_2 = drop (back, drop (back, take_back));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (back, take_back_next_2), true);
    }
    {
        auto take_back = take (back, std::size_t (3), l);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (back, take_back), false);
        BOOST_CHECK_EQUAL (first (back, take_back), 37);

        auto take_back_next_2 = drop (back, drop (back, take_back));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (back, take_back_next_2), false);
        BOOST_CHECK_EQUAL (first (back, take_back_next_2), 7);

        RIME_CHECK_EQUAL (empty (back, drop (back, take_back_next_2)), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_infinite_homogeneous_runtime) {
    auto c = count (5);

    {
        auto t = take (7, c);
        BOOST_MPL_ASSERT ((is_homogeneous <decltype (t)>));
        // Not currently true, but maybe it should be?
        // static_assert (range::has::size <decltype (t)>::value, "");

        RIME_CHECK_EQUAL (first (t), 5);
        t = drop (4, t);
        RIME_CHECK_EQUAL (first (t), 9);
        t = drop (t);
        RIME_CHECK_EQUAL (first (t), 10);
        t = drop (2, t);
        RIME_CHECK_EQUAL (empty (t), true);
    }
    {
        auto t = take (7, c);
        BOOST_MPL_ASSERT ((is_homogeneous <decltype (t)>));
        // Not currently true, but maybe it should be?
        // static_assert (range::has::size <decltype (t)>::value, "");

        RIME_CHECK_EQUAL (first (t), 5);
        t = drop (rime::int_ <4>(), t);
        RIME_CHECK_EQUAL (first (t), 9);
        t = drop (t);
        RIME_CHECK_EQUAL (first (t), 10);
        t = drop (rime::int_ <2>(), t);
        RIME_CHECK_EQUAL (empty (t), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_heterogeneous_constant) {
    std::tuple <> tuple_0;

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (zero, tuple_0), tuple_0);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (one, tuple_0), tuple_0);

    std::tuple <int> tuple_1 (5);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (zero, tuple_1), tuple_0);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (one, tuple_1), tuple_1);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (two, tuple_1), tuple_1);

    std::tuple <int, double> tuple_2 (5, 7.);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (zero, tuple_2), tuple_0);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (one, tuple_2), tuple_1);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (two, tuple_2), tuple_2);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (three, tuple_2), tuple_2);

    {
        auto take_back = take (back, one, tuple_2);
        RIME_CHECK_EQUAL (size (take_back), one);
        RIME_CHECK_EQUAL (first (take_back), 7.);
        RIME_CHECK_EQUAL (empty (drop (take_back)), rime::true_);
    }

    {
        auto take_back = take (back, zero, tuple_2);
        RIME_CHECK_EQUAL (empty (take_back), rime::true_);
        RIME_CHECK_EQUAL (size (take_back), zero);
    }
    {
        auto take_back = take (back, two, tuple_2);
        RIME_CHECK_EQUAL (size (back, take_back), two);
        RIME_CHECK_EQUAL (first (back, take_back), 7.);
        RIME_CHECK_EQUAL (first (back, drop (back, take_back)), 5);
        RIME_CHECK_EQUAL (empty (drop (back, one, take_back)), rime::false_);
        RIME_CHECK_EQUAL (empty (drop (back, two, take_back)), rime::true_);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_heterogeneous_runtime) {
    std::tuple <int, double> tuple_2 (2, 3.);
    auto take_view = take (3, tuple_2);

    // At compile time, it is only known that take_view has at most two
    // elements.
    RIME_CHECK_EQUAL (empty (take_view), false);
    RIME_CHECK_EQUAL (empty (drop (take_view)), false);
    RIME_CHECK_EQUAL (empty (drop (two, take_view)), rime::true_);
}

BOOST_AUTO_TEST_CASE (test_range_take_infinite_heterogeneous_constant) {
    auto c = count (rime::size_t <5>());

    auto t = take (rime::int_ <7>(), c);
    BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (t)>));

    RIME_CHECK_EQUAL (first (t), rime::size_t <5>());
    auto t2 = drop (rime::int_ <4>(), t);
    RIME_CHECK_EQUAL (first (t2), rime::size_t <9>());
    auto t3 = drop (t2);
    RIME_CHECK_EQUAL (first (t3), rime::size_t <10>());
    auto t4 = drop (rime::int_ <2>(), t3);
    RIME_CHECK_EQUAL (empty (t4), rime::true_);
}

BOOST_AUTO_TEST_CASE (test_range_take_multiple) {
    {
        std::tuple <int, double, char> t (5, 7.75, 'A');
        auto taken = take (2, take (5, t));
        BOOST_CHECK_EQUAL (first (taken), 5);
        BOOST_CHECK_EQUAL (first (drop (taken)), 7.75);
        RIME_CHECK_EQUAL (empty (drop (two, taken)), true);
    }

    // Different types for the first take and the second.
    {
        std::tuple <int, double, char> t (5, 7.75, 'A');
        auto taken = take (2, take (5u, t));
        BOOST_CHECK_EQUAL (first (taken), 5);
        BOOST_CHECK_EQUAL (first (drop (taken)), 7.75);
        RIME_CHECK_EQUAL (empty (drop (two, taken)), true);
    }

    // Different types for the first take and the second.
    {
        std::tuple <int, double, char> t (5, 7.75, 'A');
        auto taken = take (two, take (5u, t));
        BOOST_CHECK_EQUAL (first (taken), 5);
        BOOST_CHECK_EQUAL (first (drop (taken)), 7.75);
        // Limit becomes runtime value, so empty() returns a runtime value.
        RIME_CHECK_EQUAL (empty (drop (two, taken)), true);
    }
}

int produce_consecutive() {
    static int current = 0;
    return ++ current;
}

BOOST_AUTO_TEST_CASE (test_range_take_input_range) {
    // Test chop.
    {
        auto t = range::take (3,
            range::make_function_range (produce_consecutive));
        auto result1 = range::chop (std::move (t));
        BOOST_CHECK_EQUAL (result1.first(), 1);
        auto result2 = range::chop (std::move (result1.forward_rest()));
        BOOST_CHECK_EQUAL (result2.first(), 2);
        auto result3 = range::chop (std::move (result2.forward_rest()));
        BOOST_CHECK_EQUAL (result3.first(), 3);
        BOOST_CHECK (range::empty (result3.rest()));
    }

    // Test chop_in_place.
    {
        auto t = range::take (3,
            range::make_function_range (produce_consecutive));
        int result = range::chop_in_place (t);
        BOOST_CHECK_EQUAL (result, 4);
        result = range::chop_in_place (t);
        BOOST_CHECK_EQUAL (result, 5);
        result = range::chop_in_place (t);
        BOOST_CHECK_EQUAL (result, 6);
        BOOST_CHECK (range::empty (t));
    }

    // Test that chop_in_place with non-homogeneous limit does not exist.
    {
        auto t = range::take (rime::int_ <2>(),
            range::make_function_range (produce_consecutive));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (t) &)>));
    }
}

BOOST_AUTO_TEST_SUITE_END()
