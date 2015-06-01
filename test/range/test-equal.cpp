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

#define BOOST_TEST_MODULE range_equal_test
#include "utility/test/boost_unit_test.hpp"

#include "range/equal.hpp"

#include <vector>
#include <tuple>

#include "range/std.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(range_equal_test_suite)

bool approximately_equal (int i, int j)
{ return i-1 == j || i == j || i+1 == j; }

#define CHECK_range_equal(r1, r2, value, approximately_value) \
    RIME_CHECK_EQUAL (range::equal (r1, r2), value); \
    RIME_CHECK_EQUAL (range::equal (r2, r1), value); \
    RIME_CHECK_EQUAL (range::equal (range::front, r1, r2), value); \
    RIME_CHECK_EQUAL (range::equal (range::front, r2, r1), value); \
    RIME_CHECK_EQUAL (range::equal (range::back, r1, r2), value); \
    RIME_CHECK_EQUAL (range::equal (range::back, r2, r1), value); \
    \
    RIME_CHECK_EQUAL (range::equal ( \
        approximately_equal, r1, r2), approximately_value); \
    RIME_CHECK_EQUAL (range::equal ( \
        approximately_equal, r2, r1), approximately_value); \
    RIME_CHECK_EQUAL (range::equal ( \
        range::front, approximately_equal, r1, r2), approximately_value); \
    RIME_CHECK_EQUAL (range::equal ( \
        range::front, approximately_equal, r2, r1), approximately_value); \
    RIME_CHECK_EQUAL (range::equal ( \
        range::back, approximately_equal, r1, r2), approximately_value); \
    RIME_CHECK_EQUAL (range::equal ( \
        range::back, approximately_equal, r2, r1), approximately_value)

BOOST_AUTO_TEST_CASE (test_range_equal_homogeneous) {
    std::vector <int> v1, v2;

    CHECK_range_equal (v1, v2, true, true);

    v1.push_back (1);
    CHECK_range_equal (v1, v2, false, false);

    v2.push_back (2);
    CHECK_range_equal (v1, v2, false, true);

    v2.pop_back();
    v2.push_back (1);
    CHECK_range_equal (v1, v2, true, true);

    v2.push_back (3);
    CHECK_range_equal (v1, v2, false, false);

    v1.push_back (4);
    CHECK_range_equal (v1, v2, false, true);

    v1.pop_back();
    v1.push_back(7);
    CHECK_range_equal (v1, v2, false, false);

    v1.pop_back();
    v1.push_back(3);
    CHECK_range_equal (v1, v2, true, true);
}

BOOST_AUTO_TEST_CASE (test_range_equal_heterogeneous) {
    CHECK_range_equal (std::make_tuple(), std::make_tuple(),
        rime::true_, rime::true_);

    // Different lengths: known at compile-time.
    CHECK_range_equal (std::make_tuple(), std::make_tuple (1),
        rime::false_, rime::false_);
    CHECK_range_equal (std::make_tuple(), std::make_tuple (1, 2),
        rime::false_, rime::false_);

    CHECK_range_equal (std::make_tuple (1, 2), std::make_tuple (1),
        rime::false_, rime::false_);
    CHECK_range_equal (std::make_tuple (1, 2, 3), std::make_tuple (1, 2),
        rime::false_, rime::false_);

    // The same lengths: the result depends on the actual values.
    CHECK_range_equal (std::make_tuple (5), std::make_tuple (),
        rime::false_, rime::false_);
    CHECK_range_equal (std::make_tuple (3), std::make_tuple (4), false, true);
    CHECK_range_equal (std::make_tuple (3), std::make_tuple (7),
        false, false);

    CHECK_range_equal (std::make_tuple (1, 2), std::make_tuple (1, 2),
        true, true);
    CHECK_range_equal (std::make_tuple (1, 2), std::make_tuple (1, 3),
        false, true);
    CHECK_range_equal (std::make_tuple (1, 2), std::make_tuple (1, 5),
        false, false);
    CHECK_range_equal (std::make_tuple (5, 2), std::make_tuple (1, 2),
        false, false);
    CHECK_range_equal (std::make_tuple (5, 2), std::make_tuple (4, 2),
        false, true);

    CHECK_range_equal (
        std::make_tuple (1, 2, 3), std::make_tuple (1, 2, 3), true, true);
    CHECK_range_equal (
        std::make_tuple (6, 2, 3), std::make_tuple (5, 2, 3), false, true);
    CHECK_range_equal (
        std::make_tuple (1, 7, 3), std::make_tuple (1, 9, 3), false, false);
    CHECK_range_equal (
        std::make_tuple (1, 2, 3), std::make_tuple (1, 2, 5), false, false);
}

// Mix homogeneous and heterogeneous ranges.
BOOST_AUTO_TEST_CASE (test_range_equal_mixed) {
    std::vector <int> v1, v2;

    CHECK_range_equal (v1, std::make_tuple(), true, true);
    CHECK_range_equal (v1, std::make_tuple (1), false, false);
    CHECK_range_equal (v1, std::make_tuple (1, 2), false, false);

    v1.push_back (1);
    CHECK_range_equal (v1, std::make_tuple (1), true, true);
    CHECK_range_equal (v1, std::make_tuple(), false, false);
    CHECK_range_equal (v1, std::make_tuple (2), false, true);
    CHECK_range_equal (v1, std::make_tuple (3), false, false);
    CHECK_range_equal (v1, std::make_tuple(1, 3), false, false);

    v1.push_back (4);
    CHECK_range_equal (v1, std::make_tuple(1, 4), true, true);
    CHECK_range_equal (v1, std::make_tuple(), false, false);
    CHECK_range_equal (v1, std::make_tuple(1), false, false);
    CHECK_range_equal (v1, std::make_tuple(1, 3), false, true);
    CHECK_range_equal (v1, std::make_tuple(1, 7), false, false);
    CHECK_range_equal (v1, std::make_tuple(1, 4, 5), false, false);
}

// Mix homogeneous and heterogeneous ranges.
BOOST_AUTO_TEST_CASE (test_range_equal_constants) {
    rime::int_ <1> one;
    rime::int_ <3> three;
    rime::int_ <4> four;

    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple(), std::make_tuple()), rime::true_);

    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple (one), std::make_tuple (one)), rime::true_);
    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple (one), std::make_tuple (one, three)), rime::false_);

    RIME_CHECK_EQUAL (range::equal (
            std::make_tuple (one, three), std::make_tuple (one, three)),
        rime::true_);
    RIME_CHECK_EQUAL (range::equal (
            std::make_tuple (one, three), std::make_tuple (one, four)),
        rime::false_);

    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple (one), std::make_tuple (one, 3)), rime::false_);
    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple (1), std::make_tuple (one)), true);
    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple (2), std::make_tuple (one)), false);
    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple (four, 1), std::make_tuple (four, one)), true);
    RIME_CHECK_EQUAL (range::equal (
        std::make_tuple (four, 2), std::make_tuple (four, one)), false);
}

BOOST_AUTO_TEST_SUITE_END()
