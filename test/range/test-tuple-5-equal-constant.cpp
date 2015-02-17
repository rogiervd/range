/*
Copyright 2014, 2015 Rogier van Dalen.

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
    RIME_CHECK_EQUAL (equal (range::front, r1, r2), value); \
    RIME_CHECK_EQUAL (equal (range::front, r2, r1), value); \
    RIME_CHECK_EQUAL (equal (range::back, r1, r2), value); \
    RIME_CHECK_EQUAL (equal (range::back, r2, r1), value); \
    \
    RIME_CHECK_EQUAL (equal ( \
        approximately_equal, r1, r2), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        approximately_equal, r2, r1), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        range::front, approximately_equal, r1, r2), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        range::front, approximately_equal, r2, r1), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        range::back, approximately_equal, r1, r2), approximately_value); \
    RIME_CHECK_EQUAL (equal ( \
        range::back, approximately_equal, r2, r1), approximately_value)

#define CHECK_tuple_equal(r1, r2, value, approximately_value) \
    CHECK_tuple_view_equal (r1, r2, value, approximately_value); \
    RIME_CHECK_EQUAL ((r1 == r2), value); \
    RIME_CHECK_EQUAL ((r2 == r1), value); \
    RIME_CHECK_EQUAL (!(r1 != r2), value); \
    RIME_CHECK_EQUAL (!(r2 != r1), value)

BOOST_AUTO_TEST_CASE (constants) {
    rime::int_ <1> one;
    rime::int_ <3> three;
    rime::int_ <4> four;

    RIME_CHECK_EQUAL (equal (
        make_tuple(), make_tuple()), rime::true_);

    RIME_CHECK_EQUAL (equal (
        make_tuple (one), make_tuple (one)), rime::true_);
    RIME_CHECK_EQUAL (equal (
        make_tuple (one), make_tuple (one, three)), rime::false_);

    RIME_CHECK_EQUAL (equal (
            make_tuple (one, three), make_tuple (one, three)),
        rime::true_);
    RIME_CHECK_EQUAL (equal (
            make_tuple (one, three), make_tuple (one, four)),
        rime::false_);

    RIME_CHECK_EQUAL (equal (
        make_tuple (one), make_tuple (one, 3)), rime::false_);
    RIME_CHECK_EQUAL (equal (
        make_tuple (1), make_tuple (one)), true);
    RIME_CHECK_EQUAL (equal (
        make_tuple (2), make_tuple (one)), false);
    RIME_CHECK_EQUAL (equal (
        make_tuple (four, 1), make_tuple (four, one)), true);
    RIME_CHECK_EQUAL (equal (
        make_tuple (four, 2), make_tuple (four, one)), false);
}

BOOST_AUTO_TEST_SUITE_END()
