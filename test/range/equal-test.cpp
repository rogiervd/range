/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE range_equal_test
#include "utility/test/boost_unit_test.hpp"

#include "range/equal.hpp"

#include <vector>
#include <tuple>

#include "range/std.hpp"

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(range_equal_test_suite)

#define CHECK_range_equal(r1, r2, value) \
    RIME_CHECK_EQUAL (range::equal (r1, r2), value); \
    RIME_CHECK_EQUAL (range::equal (r2, r1), value); \
    RIME_CHECK_EQUAL (range::equal (range::front, r1, r2), value); \
    RIME_CHECK_EQUAL (range::equal (range::front, r2, r1), value); \
    RIME_CHECK_EQUAL (range::equal (range::back, r1, r2), value); \
    RIME_CHECK_EQUAL (range::equal (range::back, r2, r1), value)

BOOST_AUTO_TEST_CASE (test_range_equal_homogeneous) {
    std::vector <int> v1, v2;

    CHECK_range_equal (v1, v2, true);

    v1.push_back (1);
    CHECK_range_equal (v1, v2, false);

    v2.push_back (2);
    CHECK_range_equal (v1, v2, false);

    v2.pop_back();
    v2.push_back (1);
    CHECK_range_equal (v1, v2, true);

    v2.push_back (3);
    CHECK_range_equal (v1, v2, false);

    v1.push_back (4);
    CHECK_range_equal (v1, v2, false);

    v1.pop_back();
    v1.push_back(3);
    CHECK_range_equal (v1, v2, true);
}

BOOST_AUTO_TEST_CASE (test_range_equal_heterogeneous) {
    CHECK_range_equal (std::make_tuple(), std::make_tuple(), rime::true_);

    // Different lengths: known at compile-time.
    CHECK_range_equal (std::make_tuple(), std::make_tuple (1), rime::false_);
    CHECK_range_equal (std::make_tuple(), std::make_tuple (1, 2), rime::false_);

    CHECK_range_equal (
        std::make_tuple (1, 2), std::make_tuple (1), rime::false_);
    CHECK_range_equal (
        std::make_tuple (1, 2, 3), std::make_tuple (1, 2), rime::false_);

    // The same lengths: the result depends on the actual values.
    CHECK_range_equal (std::make_tuple (5), std::make_tuple (5), true);
    CHECK_range_equal (std::make_tuple (3), std::make_tuple (7), false);

    CHECK_range_equal (std::make_tuple (1, 2), std::make_tuple (1, 2), true);
    CHECK_range_equal (std::make_tuple (1, 2), std::make_tuple (1, 3), false);
    CHECK_range_equal (std::make_tuple (5, 2), std::make_tuple (1, 2), false);

    CHECK_range_equal (
        std::make_tuple (1, 2, 3), std::make_tuple (1, 2, 3), true);
    CHECK_range_equal (
        std::make_tuple (6, 2, 3), std::make_tuple (5, 2, 3), false);
    CHECK_range_equal (
        std::make_tuple (1, 7, 3), std::make_tuple (1, 8, 3), false);
    CHECK_range_equal (
        std::make_tuple (1, 2, 3), std::make_tuple (1, 2, 4), false);
}

// Mix homogeneous and heterogeneous ranges.
BOOST_AUTO_TEST_CASE (test_range_equal_mixed) {
    std::vector <int> v1, v2;

    CHECK_range_equal (v1, std::make_tuple(), true);
    CHECK_range_equal (v1, std::make_tuple (1), false);
    CHECK_range_equal (v1, std::make_tuple (1, 2), false);

    v1.push_back (1);
    CHECK_range_equal (v1, std::make_tuple (1), true);
    CHECK_range_equal (v1, std::make_tuple(), false);
    CHECK_range_equal (v1, std::make_tuple (2), false);
    CHECK_range_equal (v1, std::make_tuple(1, 3), false);

    v1.push_back (4);
    CHECK_range_equal (v1, std::make_tuple(1, 4), true);
    CHECK_range_equal (v1, std::make_tuple(), false);
    CHECK_range_equal (v1, std::make_tuple(1), false);
    CHECK_range_equal (v1, std::make_tuple(1, 3), false);
    CHECK_range_equal (v1, std::make_tuple(1, 4, 5), false);
}

BOOST_AUTO_TEST_SUITE_END()
