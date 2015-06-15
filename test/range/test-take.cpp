/*
Copyright 2013-2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_take
#include "utility/test/boost_unit_test.hpp"

#include "range/take.hpp"

#include <vector>
#include <tuple>
#include <type_traits>

#include "range/std.hpp"
#include "range/count.hpp"
#include "range/function_range.hpp"

#include "rime/check/check_equal.hpp"
#include "check_equal_behaviour.hpp"


BOOST_AUTO_TEST_SUITE(test_range_take)

using range::take;

using range::is_homogeneous;
using range::default_direction;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;
using range::front;
using range::back;
using range::count_from;

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
        auto first = take (reference, 3);
        BOOST_MPL_ASSERT ((std::is_same <decltype (view), decltype (first)>));
        BOOST_CHECK (empty (first));
    }

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (0), front), range::view (v));
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (1)), range::view (v));
    {
        auto take_back = take (v, std::size_t (0), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }
    {
        auto take_back = take (v, std::size_t (1), back);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }

    v.push_back (7);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (0), front), reference);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (1), front), v);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (2), front), v);
    {
        auto take_back = take (v, std::size_t (0), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }
    {
        auto take_back = take (v, std::size_t (1), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 7);
        RIME_CHECK_EQUAL (empty (drop (take_back), back), true);
    }
    {
        auto take_back = take (v, std::size_t (5), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 7);
        RIME_CHECK_EQUAL (empty (drop (take_back), back), true);
    }

    v.push_back (27);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (0), front), reference);
    reference.push_back (7);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (1), front), reference);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (2), front), v);
    reference.push_back (27);

    {
        auto take_back = take (v, std::size_t (0), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back), true);
    }
    {
        auto take_back = take (v, std::size_t (1), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 27);
        RIME_CHECK_EQUAL (empty (drop (take_back, back)), true);
    }
    {
        auto take_back = take (v, std::size_t (5), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 27);

        auto take_back_next = drop (take_back, back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next)>));
        RIME_CHECK_EQUAL (empty (take_back_next), false);
        BOOST_CHECK_EQUAL (first (take_back_next, back), 7);

        RIME_CHECK_EQUAL (empty (drop (take_back_next)), true);
    }

    v.push_back (37);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (2), front), reference);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (4), front), v);
    check_equal_behaviour <
        rime::true_type, rime::true_type, rime::false_type, rime::true_type> (
            take (v, std::size_t (3), front), v);
    {
        auto take_back = take (v, std::size_t (2), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 37);

        auto take_back_next_2 = drop (take_back, 2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (take_back_next_2), true);
    }
    {
        auto take_back = take (v, std::size_t (3), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 37);

        auto take_back_next_2 = drop (take_back, 2, back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (take_back_next_2), false);
        BOOST_CHECK_EQUAL (first (take_back_next_2, back), 7);

        RIME_CHECK_EQUAL (empty (drop (take_back_next_2, back)), true);

        RIME_CHECK_EQUAL (empty (drop (take_back, 3, back)), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_list_runtime) {
    std::list <int> l;
    std::list <int> reference;

    {
        // Unlike in the case of std::vector, take can't return an
        // iterator_range here:
        auto view = range::view (l);
        auto first = take (l, 3);
        BOOST_MPL_ASSERT_NOT ((
            std::is_same <decltype (view), decltype (first)>));
        static_assert (
            range::has <range::callable::first (decltype (first))>::value, "");
        RIME_CHECK_EQUAL (empty (first), true);

        // Second take: the type does not change.
        auto first_2 = take (first, 2);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (first), decltype (first_2)>));
        RIME_CHECK_EQUAL (empty (first_2), true);
    }

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (0), front), range::view (l));
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (1)), range::view (l));
    {
        auto take_back = take (l, std::size_t (0), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), true);
    }
    {
        auto take_back = take (l, std::size_t (1), back);
        RIME_CHECK_EQUAL (empty (take_back, back), true);
    }

    l.push_back (7);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (0), front), reference);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (1), front), l);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (2), front), l);

    {
        auto take_back = take (l, std::size_t (0), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), true);
    }
    {
        auto take_back = take (l, std::size_t (1), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 7);
        RIME_CHECK_EQUAL (empty (drop (take_back, back), back), true);
    }
    {
        auto take_back = take (l, std::size_t (5), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 7);
        RIME_CHECK_EQUAL (empty (drop (take_back, back), back), true);
    }

    l.push_back (27);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (0), front), reference);
    reference.push_back (7);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (1), front), reference);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (2), front), l);
    reference.push_back (27);

    {
        auto take_back = take (l, std::size_t (0), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), true);
    }
    {
        auto take_back = take (l, std::size_t (1), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 27);
        RIME_CHECK_EQUAL (empty (drop (take_back, back), back), true);
    }
    {
        auto take_back = take (l, std::size_t (5), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 27);

        auto take_back_next = drop (take_back, back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next)>));
        RIME_CHECK_EQUAL (empty (take_back_next, back), false);
        BOOST_CHECK_EQUAL (first (take_back_next, back), 7);

        RIME_CHECK_EQUAL (empty (drop (take_back_next, back), back), true);
    }

    l.push_back (37);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (2), front), reference);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (4), front), l);
    check_equal_behaviour <
        rime::false_type, rime::false_type, rime::false_type, rime::false_type>
            (take (l, std::size_t (3), front), l);
    {
        auto take_back = take (l, std::size_t (2), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 37);

        auto take_back_next_2 = drop (drop (take_back, back), back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (take_back_next_2, back), true);
    }
    {
        auto take_back = take (l, std::size_t (3), back);
        BOOST_CHECK (default_direction (take_back) == front);
        RIME_CHECK_EQUAL (empty (take_back, back), false);
        BOOST_CHECK_EQUAL (first (take_back, back), 37);

        auto take_back_next_2 = drop (drop (take_back, back), back);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (take_back), decltype (take_back_next_2)>));
        RIME_CHECK_EQUAL (empty (take_back_next_2, back), false);
        BOOST_CHECK_EQUAL (first (take_back_next_2, back), 7);

        RIME_CHECK_EQUAL (empty (drop (take_back_next_2, back), back), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_infinite_homogeneous_runtime) {
    auto c = count_from (5);

    {
        auto t = take (c, 7);
        BOOST_MPL_ASSERT ((is_homogeneous <decltype (t)>));
        // Not currently true, but maybe it should be?
        // static_assert (range::has::size <decltype (t)>::value, "");

        RIME_CHECK_EQUAL (first (t), 5);
        t = drop (t, 4);
        RIME_CHECK_EQUAL (first (t), 9);
        t = drop (t);
        RIME_CHECK_EQUAL (first (t), 10);
        t = drop (t, 2);
        RIME_CHECK_EQUAL (empty (t), true);
    }
    {
        auto t = take (c, 7);
        BOOST_MPL_ASSERT ((is_homogeneous <decltype (t)>));
        // Not currently true, but maybe it should be?
        // static_assert (range::has::size <decltype (t)>::value, "");

        RIME_CHECK_EQUAL (first (t), 5);
        t = drop (t, rime::int_ <4>());
        RIME_CHECK_EQUAL (first (t), 9);
        t = drop (t);
        RIME_CHECK_EQUAL (first (t), 10);
        t = drop (t, rime::int_ <2>());
        RIME_CHECK_EQUAL (empty (t), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_heterogeneous_constant) {
    std::tuple <> tuple_0;

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_0, zero), tuple_0);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_0, one), tuple_0);

    std::tuple <int> tuple_1 (5);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_1, zero), tuple_0);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_1, one), tuple_1);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_1, two), tuple_1);

    std::tuple <int, double> tuple_2 (5, 7.);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_2, zero), tuple_0);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_2, one), tuple_1);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_2, two), tuple_2);
    check_equal_behaviour <
        rime::true_type, rime::false_type, rime::true_type, rime::false_type> (
            take (tuple_2, three), tuple_2);

    {
        auto take_back = take (tuple_2, one, back);
        RIME_CHECK_EQUAL (size (take_back), one);
        RIME_CHECK_EQUAL (first (take_back), 7.);
        RIME_CHECK_EQUAL (empty (drop (take_back)), rime::true_);
    }

    {
        auto take_back = take (tuple_2, zero, back);
        RIME_CHECK_EQUAL (empty (take_back), rime::true_);
        RIME_CHECK_EQUAL (size (take_back), zero);
    }
    {
        auto take_back = take (tuple_2, two, back);
        RIME_CHECK_EQUAL (size (take_back, back), two);
        RIME_CHECK_EQUAL (first (take_back, back), 7.);
        RIME_CHECK_EQUAL (first (drop (take_back, back), back), 5);
        RIME_CHECK_EQUAL (empty (drop (take_back, one, back)), rime::false_);
        RIME_CHECK_EQUAL (empty (drop (take_back, two, back)), rime::true_);
    }
}

BOOST_AUTO_TEST_CASE (test_range_take_heterogeneous_runtime) {
    std::tuple <int, double> tuple_2 (2, 3.);
    auto take_view = take (tuple_2, 3);

    // At compile time, it is only known that take_view has at most two
    // elements.
    RIME_CHECK_EQUAL (empty (take_view), false);
    RIME_CHECK_EQUAL (empty (drop (take_view)), false);
    RIME_CHECK_EQUAL (empty (drop (take_view, two)), rime::true_);
}

BOOST_AUTO_TEST_CASE (test_range_take_infinite_heterogeneous_constant) {
    auto c = count_from (rime::size_t <5>());

    auto t = take (c, rime::int_ <7>());
    BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (t)>));

    RIME_CHECK_EQUAL (first (t), rime::size_t <5>());
    auto t2 = drop (t, rime::int_ <4>());
    RIME_CHECK_EQUAL (first (t2), rime::size_t <9>());
    auto t3 = drop (t2);
    RIME_CHECK_EQUAL (first (t3), rime::size_t <10>());
    auto t4 = drop (t3, rime::int_ <2>());
    RIME_CHECK_EQUAL (empty (t4), rime::true_);
}

BOOST_AUTO_TEST_CASE (test_range_take_multiple) {
    {
        std::tuple <int, double, char> t (5, 7.75, 'A');
        auto taken = take (take (t, 5), 2);
        BOOST_CHECK_EQUAL (first (taken), 5);
        BOOST_CHECK_EQUAL (first (drop (taken)), 7.75);
        RIME_CHECK_EQUAL (empty (drop (taken, two)), true);
    }

    // Different types for the first take and the second.
    {
        std::tuple <int, double, char> t (5, 7.75, 'A');
        auto taken = take (take (t, 5u), 2);
        BOOST_CHECK_EQUAL (first (taken), 5);
        BOOST_CHECK_EQUAL (first (drop (taken)), 7.75);
        RIME_CHECK_EQUAL (empty (drop (taken, two)), true);
    }

    // Different types for the first take and the second.
    {
        std::tuple <int, double, char> t (5, 7.75, 'A');
        auto taken = take (take (t, 5u), two);
        BOOST_CHECK_EQUAL (first (taken), 5);
        BOOST_CHECK_EQUAL (first (drop (taken)), 7.75);
        // Limit becomes runtime value, so empty() returns a runtime value.
        RIME_CHECK_EQUAL (empty (drop (taken, two)), true);
    }
}

int produce_consecutive() {
    static int current = 0;
    return ++ current;
}

BOOST_AUTO_TEST_CASE (test_range_take_input_range) {
    // Test chop.
    {
        auto t = range::take (
            range::make_function_range (produce_consecutive), 3);
        auto result1 = chop (std::move (t));
        BOOST_CHECK_EQUAL (result1.first(), 1);
        auto result2 = chop (std::move (result1.forward_rest()));
        BOOST_CHECK_EQUAL (result2.first(), 2);
        auto result3 = chop (std::move (result2.forward_rest()));
        BOOST_CHECK_EQUAL (result3.first(), 3);
        BOOST_CHECK (range::empty (result3.rest()));
    }

    // Test chop_in_place.
    {
        auto t = range::take (
            range::make_function_range (produce_consecutive), 3);
        int result = chop_in_place (t);
        BOOST_CHECK_EQUAL (result, 4);
        result = chop_in_place (t);
        BOOST_CHECK_EQUAL (result, 5);
        result = chop_in_place (t);
        BOOST_CHECK_EQUAL (result, 6);
        BOOST_CHECK (empty (t));
    }

    // Test that chop_in_place with non-homogeneous limit does not exist.
    {
        auto t = take (
            range::make_function_range (produce_consecutive), rime::int_ <2>());
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (t) &)>));
    }
}

BOOST_AUTO_TEST_SUITE_END()
