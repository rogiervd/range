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

/** \file
call_unpack is particularly useful in combination with "zip".
Therefore, this file tests this directly.
*/

#define BOOST_TEST_MODULE test_range_call_unpack_integration
#include "utility/test/boost_unit_test.hpp"

#include "range/call_unpack.hpp"

#include <vector>

#include "utility/returns.hpp"

#include "rime/check/check_equal.hpp"

#include "range/std/container.hpp"
#include "range/tuple.hpp"
#include "range/transform.hpp"
#include "range/for_each.hpp"
#include "range/zip.hpp"

using range::tuple;
using range::transform;
using range::zip;
namespace curry = range::curry;

BOOST_AUTO_TEST_SUITE(test_range_call_unpack)

double & plus_assign (double & target, double addendum) {
    return target += addendum;
}

BOOST_AUTO_TEST_CASE (test_call_unpack_add) {
    std::vector <double> v1;
    v1.push_back (7.5);
    v1.push_back (17);
    v1.push_back (99);
    std::vector <double> v2;
    v2.push_back (8.5);
    v2.push_back (18);

    range::for_each (curry::call_unpack (plus_assign), zip (v1, v2));

    BOOST_CHECK_EQUAL (v1 [0], 7.5 + 8.5);
    BOOST_CHECK_EQUAL (v1 [1], 17. + 18);
    // zip stops iff one of the ranges is empty, so v1 [2] is never touched.
    BOOST_CHECK_EQUAL (v1 [2], 99.);
}

struct difference {
    template <class Left, class Right>
        auto operator() (Left left, Right right) const
    RETURNS (right - left);
};

BOOST_AUTO_TEST_CASE (test_call_unpack_difference) {
    tuple <int, double> t1 (1, 5.5);
    tuple <float, double> t2 (2.5, 7.5);

    auto differences = transform (
        curry::call_unpack (difference()), zip (t1, t2));
    auto e1 = range::first (differences);
    auto e2 = range::second (differences);

    RIME_CHECK_EQUAL (e1, 1.5f);
    RIME_CHECK_EQUAL (e2, 2.);
}

BOOST_AUTO_TEST_SUITE_END()
