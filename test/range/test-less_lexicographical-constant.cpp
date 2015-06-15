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

/*
These tests should be in test-less_lexicographical-heterogeneous.cpp but
by splitting it up, the compiler requires less memory.
*/

#define BOOST_TEST_MODULE range_test_less_lexicographical_constant
#include "utility/test/boost_unit_test.hpp"

#include "range/less_lexicographical.hpp"

#include <functional>
#include <vector>
#include <tuple>
#include <string>

#include "range/std.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"

#include "rime/core.hpp"
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

BOOST_AUTO_TEST_SUITE(range_test_less_lexicographical_constant)

using range::less_lexicographical;

using range::transform;
using range::reverse;

#define CHECK_range_less_lexicographical(r1, r2, value) \
    RIME_CHECK_EQUAL (less_lexicographical (r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        r1, r2, range::front), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        reverse (r1), reverse (r2), range::back), value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical (r1, r2, less()), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        r1, r2, range::front, less()), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        reverse (r1), reverse (r2), range::back, less()), value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        transform (r1, negate()), transform (r2, negate()), greater()), \
        value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        transform (r1, negate()), transform (r2, negate()), \
        range::front, greater()), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        transform (reverse (r1), negate()), \
        transform (reverse (r2), negate()), range::back, greater()), value)

BOOST_AUTO_TEST_CASE (test_range_less_lexicographical_constant) {
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <5>()), std::make_tuple (rime::int_ <5>()),
        rime::false_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <3>()), std::make_tuple (rime::int_ <7>()),
        rime::true_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <7>()), std::make_tuple (rime::int_ <3>()),
        rime::false_);

    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>()), rime::false_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <3>()), rime::true_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <3>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>()), rime::false_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <5>(), rime::int_ <2>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>()), rime::false_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>()),
        std::make_tuple (rime::int_ <5>(), rime::int_ <2>()), rime::true_);

    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::false_);

    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <6>(), rime::int_ <2>(), rime::int_ <3>()),
        std::make_tuple (rime::int_ <5>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::false_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <5>(), rime::int_ <2>(), rime::int_ <3>()),
        std::make_tuple (rime::int_ <6>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::true_);

    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <7>(), rime::int_ <3>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <8>(), rime::int_ <3>()),
        rime::true_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <8>(), rime::int_ <3>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <7>(), rime::int_ <3>()),
        rime::false_);

    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <4>()),
        rime::true_);
    CHECK_range_less_lexicographical (
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <4>()),
        std::make_tuple (rime::int_ <1>(), rime::int_ <2>(), rime::int_ <3>()),
        rime::false_);
}

BOOST_AUTO_TEST_SUITE_END()
