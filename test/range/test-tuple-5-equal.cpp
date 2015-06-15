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
Test equality on tuples.
equal() and == and != are tested.
These tests are very similar to the ones in test-equal-*.cpp.
*/

#define BOOST_TEST_MODULE range_tuple_equal
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"
#include "range/equal.hpp"

#include "rime/check/check_equal.hpp"

using range::drop;
using range::make_tuple;
using range::equal;

BOOST_AUTO_TEST_SUITE(range_tuple_equal_suite)

bool approximately_equal (double i, double j)
{ return i-1 == j || i == j || i+1 == j; }

#define CHECK_tuple_view_equal(r1, r2, value, approximately_value) \
    RIME_CHECK_EQUAL (equal (r1, r2), value); \
    RIME_CHECK_EQUAL (equal (r2, r1), value); \
    RIME_CHECK_EQUAL (equal (r1, r2, range::front), value); \
    RIME_CHECK_EQUAL (equal (r2, r1, range::front), value); \
    RIME_CHECK_EQUAL (equal (r1, r2, range::back), value); \
    RIME_CHECK_EQUAL (equal (r2, r1, range::back), value); \
    \
    RIME_CHECK_EQUAL (equal ( \
        r1, r2, approximately_equal), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        r2, r1, approximately_equal), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        r1, r2, range::front, approximately_equal), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        r2, r1, range::front, approximately_equal), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        r1, r2, range::back, approximately_equal), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        r2, r1, range::back, approximately_equal), approximately_value)

#define CHECK_tuple_equal(r1, r2, value, approximately_value) \
    CHECK_tuple_view_equal (r1, r2, value, approximately_value); \
    RIME_CHECK_EQUAL ((r1 == r2), value); \
    RIME_CHECK_EQUAL ((r2 == r1), value); \
    RIME_CHECK_EQUAL (!(r1 != r2), value); \
    RIME_CHECK_EQUAL (!(r2 != r1), value)

BOOST_AUTO_TEST_CASE (same_type) {
    CHECK_tuple_equal (make_tuple(), make_tuple(),
        rime::true_, rime::true_);

    // Different lengths: known at compile-time.
    CHECK_tuple_equal (make_tuple(), make_tuple (1),
        rime::false_, rime::false_);
    CHECK_tuple_equal (make_tuple(), make_tuple (1, 2),
        rime::false_, rime::false_);

    CHECK_tuple_equal (make_tuple (1, 2), make_tuple (1),
        rime::false_, rime::false_);
    CHECK_tuple_equal (make_tuple (1, 2, 3), make_tuple (1, 2),
        rime::false_, rime::false_);

    CHECK_tuple_view_equal (make_tuple (1, 2), drop (make_tuple (7., 1)),
        rime::false_, rime::false_);
    CHECK_tuple_view_equal (make_tuple (1, 2, 3), drop (make_tuple (7., 1, 2)),
        rime::false_, rime::false_);

    // The same lengths: the result depends on the actual values.
    CHECK_tuple_equal (make_tuple (5), make_tuple (),
        rime::false_, rime::false_);
    CHECK_tuple_equal (make_tuple (3), make_tuple (4), false, true);
    CHECK_tuple_equal (make_tuple (3), make_tuple (7),
        false, false);

    CHECK_tuple_equal (make_tuple (1, 2), make_tuple (1, 2),
        true, true);
    CHECK_tuple_equal (make_tuple (1, 2), make_tuple (1, 3),
        false, true);
    CHECK_tuple_equal (make_tuple (1, 2), make_tuple (1, 5),
        false, false);
    CHECK_tuple_equal (make_tuple (5, 2), make_tuple (1, 2),
        false, false);
    CHECK_tuple_equal (make_tuple (5, 2), make_tuple (4, 2),
        false, true);

    CHECK_tuple_view_equal (make_tuple (1, 2), drop (make_tuple (-1, 1, 2)),
        true, true);
    CHECK_tuple_view_equal (drop (make_tuple (-1, 1, 2)), make_tuple (1, 3),
        false, true);
    CHECK_tuple_view_equal (drop (make_tuple (-1, 1, 2)), make_tuple (1, 5),
        false, false);
    CHECK_tuple_view_equal (make_tuple (5, 2), drop (make_tuple (-1, 1, 2)),
        false, false);
    CHECK_tuple_view_equal (make_tuple (5, 2), drop (make_tuple (-1, 4, 2)),
        false, true);

    CHECK_tuple_equal (
        make_tuple (1, 2, 3), make_tuple (1, 2, 3), true, true);
    CHECK_tuple_equal (
        make_tuple (6, 2, 3), make_tuple (5, 2, 3), false, true);
    CHECK_tuple_equal (
        make_tuple (1, 7, 3), make_tuple (1, 9, 3), false, false);
    CHECK_tuple_equal (
        make_tuple (1, 2, 3), make_tuple (1, 2, 5), false, false);
}

BOOST_AUTO_TEST_SUITE_END()
