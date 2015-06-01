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
