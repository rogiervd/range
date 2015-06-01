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

/*
These tests should be in test-less_lexicographical-heterogeneous.cpp but
by splitting it up, the compiler requires less memory.
*/

#define BOOST_TEST_MODULE range_test_less_lexicographical_heterogeneous_2
#include "utility/test/boost_unit_test.hpp"

#include "range/less_lexicographical.hpp"

#include <functional>
#include <vector>
#include <tuple>
#include <string>

#include "range/std.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"

#include "rime/check/check_equal.hpp"

struct negate { int operator() (int i) const { return -i; } };

BOOST_AUTO_TEST_SUITE(range_test_less_lexicographical_heterogeneous)

#define CHECK_range_less_lexicographical(r1, r2, value) \
    RIME_CHECK_EQUAL (range::less_lexicographical (r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::front, r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::back, range::reverse (r1), range::reverse (r2)), value); \
    \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        std::less <int>(), r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::front, std::less <int>(), r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::back, std::less <int>(), \
        range::reverse (r1), range::reverse (r2)), value); \
    \
    RIME_CHECK_EQUAL (range::less_lexicographical (std::greater <int>(), \
        range::transform (negate(), r1), \
        range::transform (negate(), r2)), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::front, std::greater <int>(), \
        range::transform (negate(), r1), \
        range::transform (negate(), r2)), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::back, std::greater <int>(), \
        range::transform (negate(), range::reverse (r1)), \
        range::transform (negate(), range::reverse (r2))), value)

BOOST_AUTO_TEST_CASE (test_range_less_lexicographical_heterogeneous) {
    // The same lengths: the result depends on the actual values.
    CHECK_range_less_lexicographical (
        std::make_tuple (5), std::make_tuple (5), false);
    CHECK_range_less_lexicographical (
        std::make_tuple (3), std::make_tuple (7), true);
    CHECK_range_less_lexicographical (
        std::make_tuple (7), std::make_tuple (3), false);

    CHECK_range_less_lexicographical (
        std::make_tuple (1, 2), std::make_tuple (1, 2), false);
    CHECK_range_less_lexicographical (
        std::make_tuple (1, 2), std::make_tuple (1, 3), true);
    CHECK_range_less_lexicographical (
        std::make_tuple (1, 3), std::make_tuple (1, 2), false);
    CHECK_range_less_lexicographical (
        std::make_tuple (5, 2), std::make_tuple (1, 2), false);
    CHECK_range_less_lexicographical (
        std::make_tuple (1, 2), std::make_tuple (5, 2), true);

    CHECK_range_less_lexicographical (
        std::make_tuple (1, 2, 3), std::make_tuple (1, 2, 3), false);

    CHECK_range_less_lexicographical (
        std::make_tuple (6, 2, 3), std::make_tuple (5, 2, 3), false);
    CHECK_range_less_lexicographical (
        std::make_tuple (5, 2, 3), std::make_tuple (6, 2, 3), true);

    CHECK_range_less_lexicographical (
        std::make_tuple (1, 7, 3), std::make_tuple (1, 8, 3), true);
    CHECK_range_less_lexicographical (
        std::make_tuple (1, 8, 3), std::make_tuple (1, 7, 3), false);

    CHECK_range_less_lexicographical (
        std::make_tuple (1, 2, 3), std::make_tuple (1, 2, 4), true);
    CHECK_range_less_lexicographical (
        std::make_tuple (1, 2, 4), std::make_tuple (1, 2, 3), false);
}

BOOST_AUTO_TEST_SUITE_END()
