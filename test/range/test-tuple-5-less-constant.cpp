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

#define BOOST_TEST_MODULE range_test_tuple_less_constant
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"
#include "range/less_lexicographical.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"

#include "rime/check/check_equal.hpp"

struct less {
    template <class Left, class Right>
        auto operator() (Left left, Right right) const RETURNS (left < right);
};

struct greater {
    template <class Left, class Right>
        auto operator() (Left left, Right right) const RETURNS (left > right);
};

struct negate {
    template <class Type> auto operator() (Type i) const RETURNS (-i);
};

BOOST_AUTO_TEST_SUITE(range_test_tuple_less_constant)

using range::make_tuple;
using range::less_lexicographical;

using range::drop;
using range::back;
using range::front;

using range::reverse;
using range::transform;

#define CHECK_tuple_view_less(r1, r2, value) \
    RIME_CHECK_EQUAL (less_lexicographical (r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical (front, r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        back, make_tuple_from (reverse (r1)), make_tuple_from (reverse (r2))), \
        value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical (less(), r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical (front, less(), r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical (back, less(), \
        make_tuple_from (reverse (r1)), make_tuple_from (reverse (r2))), \
        value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical (greater(), \
        make_tuple_from (range::transform (negate(), r1)), \
        make_tuple_from (range::transform (negate(), r2))), value); \
    RIME_CHECK_EQUAL (less_lexicographical (front, greater(), \
        make_tuple_from (range::transform (negate(), r1)), \
        make_tuple_from (range::transform (negate(), r2))), value); \
    RIME_CHECK_EQUAL (less_lexicographical (back, greater(), \
        make_tuple_from (range::transform (negate(), reverse (r1))), \
        make_tuple_from (range::transform (negate(), reverse (r2)))), value)

#define CHECK_tuple_less(r1, r2, value) \
    CHECK_tuple_view_less (r1, r2, value); \
    RIME_CHECK_EQUAL ((r1 < r2), value); \
    RIME_CHECK_EQUAL (!(r1 >= r2), value); \
    RIME_CHECK_EQUAL ((r2 > r1), value); \
    RIME_CHECK_EQUAL (!(r2 <= r1), value)

BOOST_AUTO_TEST_CASE (constants) {
    // Just check a few examples or compilers use too much memory.

    CHECK_tuple_less (
        make_tuple (rime::int_ <5>()), make_tuple (rime::int_ <5>()),
        rime::false_);
    CHECK_tuple_less (
        make_tuple (rime::int_ <3>()), make_tuple (rime::int_ <7>()),
        rime::true_);
    CHECK_tuple_less (
        make_tuple (rime::int_ <7>()), make_tuple (rime::int_ <3>()),
        rime::false_);

    CHECK_tuple_less (make_tuple (rime::int_ <1>(), rime::int_ <2>()),
        make_tuple (rime::int_ <1>(), rime::int_ <2>()), rime::false_);
    /*CHECK_tuple_less (make_tuple (rime::int_ <1>(), rime::int_ <2>()),
        make_tuple (rime::int_ <1>(), rime::int_ <3>()), rime::true_);
    CHECK_tuple_less (make_tuple (rime::int_ <1>(), rime::int_ <3>()),
        make_tuple (rime::int_ <1>(), rime::int_ <2>()), rime::false_);
    CHECK_tuple_less (make_tuple (rime::int_ <5>(), rime::int_ <2>()),
        make_tuple (rime::int_ <1>(), rime::int_ <2>()), rime::false_);*/
    CHECK_tuple_less (make_tuple (rime::int_ <1>(), rime::int_ <2>()),
        make_tuple (rime::int_ <5>(), rime::int_ <2>()), rime::true_);

    /*CHECK_tuple_less (
        make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::false_);

    CHECK_tuple_less (
        make_tuple (rime::int_ <6>(), rime::int_ <2>(), rime::int_ <3>()),
        make_tuple (rime::int_ <5>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::false_);
    CHECK_tuple_less (
        make_tuple (rime::int_ <5>(), rime::int_ <2>(), rime::int_ <3>()),
        make_tuple (rime::int_ <6>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::true_);

    CHECK_tuple_less (
        make_tuple (rime::int_ <1>(), rime::int_ <7>(), rime::int_ <3>()),
        make_tuple (rime::int_ <1>(), rime::int_ <8>(), rime::int_ <3>()),
        rime::true_);
    CHECK_tuple_less (
        make_tuple (rime::int_ <1>(), rime::int_ <8>(), rime::int_ <3>()),
        make_tuple (rime::int_ <1>(), rime::int_ <7>(), rime::int_ <3>()),
        rime::false_);*/

    CHECK_tuple_less (
        make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <4>()),
        rime::true_);
    /*CHECK_tuple_less (
        make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <4>()),
        make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::false_);*/
}

BOOST_AUTO_TEST_SUITE_END()
