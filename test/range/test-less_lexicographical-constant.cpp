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
        range::front, r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        range::back, reverse (r1), reverse (r2)), value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical (less(), r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        range::front, less(), r1, r2), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        range::back, less(), reverse (r1), reverse (r2)), value); \
    \
    RIME_CHECK_EQUAL (less_lexicographical (greater(), \
        transform (negate(), r1), transform (negate(), r2)), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        range::front, greater(), \
        transform (negate(), r1), transform (negate(), r2)), value); \
    RIME_CHECK_EQUAL (less_lexicographical ( \
        range::back, greater(), \
        transform (negate(), reverse (r1)), \
        transform (negate(), reverse (r2))), value)

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
