/*
Copyright 2014, 2015 Rogier van Dalen.

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
Test order comparison on tuples.
less_lexicographical and <, >, <=, >= are tested.
These tests are very similar to the ones in test-less-lexicographical-*.cpp.
*/

#define BOOST_TEST_MODULE range_test_tuple_less
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"
#include "range/less_lexicographical.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"

#include "rime/check/check_equal.hpp"

struct negate { double operator() (double i) const { return -i; } };

BOOST_AUTO_TEST_SUITE(range_test_tuple_less)

using range::make_tuple;
using range::less_lexicographical;

using range::drop;
using range::back;
using range::front;

using range::reverse;
using range::transform;

#define CHECK_tuple_view_less(r1, r2, value) \
    RIME_CHECK_EQUAL (less_lexicographical (r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        front, r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        back, make_tuple_from (reverse (r1)), make_tuple_from (reverse (r2))), \
        value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        std::less <double>(), r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        front, std::less <double>(), r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        back, std::less <double>(), \
        make_tuple_from (reverse (r1)), make_tuple_from (reverse (r2))), \
        value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical (std::greater <double>(), \
        make_tuple_from (range::transform (negate(), r1)), \
        make_tuple_from (range::transform (negate(), r2))), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        front, std::greater <double>(), \
        make_tuple_from (range::transform (negate(), r1)), \
        make_tuple_from (range::transform (negate(), r2))), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        back, std::greater <double>(), \
        make_tuple_from (range::transform (negate(), reverse (r1))), \
        make_tuple_from (range::transform (negate(), reverse (r2)))), value)

#define CHECK_tuple_less(r1, r2, value) \
    CHECK_tuple_view_less (r1, r2, value); \
    RIME_CHECK_EQUAL ((r1 < r2), value); \
    RIME_CHECK_EQUAL (!(r1 >= r2), value); \
    RIME_CHECK_EQUAL ((r2 > r1), value); \
    RIME_CHECK_EQUAL (!(r2 <= r1), value)

BOOST_AUTO_TEST_CASE (heterogeneous) {
    CHECK_tuple_less (make_tuple(), make_tuple(), rime::false_);

    // Different lengths: known at compile-time.
    CHECK_tuple_less (make_tuple(), make_tuple (1), rime::true_);
    CHECK_tuple_less (make_tuple (1), make_tuple(), rime::false_);
    CHECK_tuple_less (make_tuple(), make_tuple (1, 2), rime::true_);
    CHECK_tuple_less (make_tuple (1, 2), make_tuple(), rime::false_);

    CHECK_tuple_less (make_tuple (1, 2), make_tuple (1), false);
    CHECK_tuple_less (make_tuple (1), make_tuple (1, 2), true);
    CHECK_tuple_less (make_tuple (1, 2, 3), make_tuple (1, 2), false);
    CHECK_tuple_less (make_tuple (1, 2), make_tuple (1, 2, 3), true);

    // The same lengths: the result depends on the actual values.
    CHECK_tuple_less (make_tuple (5), make_tuple (5), false);
    CHECK_tuple_less (make_tuple (3), make_tuple (7), true);
    CHECK_tuple_less (make_tuple (7), make_tuple (3), false);

    CHECK_tuple_less (make_tuple (1, 2), make_tuple (1, 2), false);
    CHECK_tuple_less (make_tuple (1, 2), make_tuple (1, 3), true);
    CHECK_tuple_less (make_tuple (1, 3), make_tuple (1, 2), false);
    CHECK_tuple_less (make_tuple (5, 2), make_tuple (1, 2), false);
    CHECK_tuple_less (make_tuple (1, 2), make_tuple (5, 2), true);

    CHECK_tuple_view_less (make_tuple (1, 2), drop (make_tuple (7., 1, 2)),
        false);
    CHECK_tuple_view_less (drop (make_tuple (7., 1, 2)), make_tuple (1, 3),
        true);
    CHECK_tuple_view_less (drop (make_tuple (7., 1, 3)),
        drop (make_tuple (7., 1, 2)), false);
    CHECK_tuple_view_less (make_tuple (5, 2), drop (make_tuple (7., 1, 2)),
        false);
    CHECK_tuple_view_less (drop (make_tuple (7., 1, 2)),
        drop (make_tuple (7., 5, 2)), true);

    CHECK_tuple_less (make_tuple (1, 2, 3), make_tuple (1, 2, 3), false);

    CHECK_tuple_less (make_tuple (6, 2, 3), make_tuple (5, 2, 3), false);
    CHECK_tuple_less (make_tuple (5, 2, 3), make_tuple (6, 2, 3), true);

    CHECK_tuple_less (make_tuple (1, 7, 3), make_tuple (1, 8, 3), true);
    CHECK_tuple_less (make_tuple (1, 8, 3), make_tuple (1, 7, 3), false);

    CHECK_tuple_less (make_tuple (1, 2, 3), make_tuple (1, 2, 4), true);
    CHECK_tuple_less (make_tuple (1, 2, 4), make_tuple (1, 2, 3), false);
}

BOOST_AUTO_TEST_SUITE_END()
