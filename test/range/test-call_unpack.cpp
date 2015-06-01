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

#define BOOST_TEST_MODULE test_range_call_unpack
#include "utility/test/boost_unit_test.hpp"

#include "range/call_unpack.hpp"

#include <tuple>

#include "range/tuple.hpp"
#include "range/std/tuple.hpp"

using range::call_unpack;

BOOST_AUTO_TEST_SUITE(test_range_call_unpack)

int check_nullary() {
    return -7;
}

char check_5u (unsigned five) {
    BOOST_CHECK_EQUAL (five, 5u);
    return 'u';
}

double & plus_assign (double & target, double addendum) {
    return target += addendum;
}

BOOST_AUTO_TEST_CASE (test_call_unpack) {
    int minus_7 = call_unpack (check_nullary, range::tuple<>());
    BOOST_CHECK_EQUAL (minus_7, -7);

    auto u = call_unpack (check_5u, std::make_tuple (5u));
    BOOST_CHECK_EQUAL (u, 'u');

    double current = 5.5;
    range::tuple <double &, double> pair (current, 2.25);
    double & current2 = call_unpack (plus_assign, pair);
    BOOST_CHECK_EQUAL (current, 7.75);
    BOOST_CHECK_EQUAL (&current2, &current);
}

BOOST_AUTO_TEST_CASE (test_curry_call_unpack) {
    int minus_7 = range::curry::call_unpack (check_nullary, range::tuple<>());
    BOOST_CHECK_EQUAL (minus_7, -7);

    auto u = range::curry::call_unpack (check_5u) (std::make_tuple (5u));
    BOOST_CHECK_EQUAL (u, 'u');

    double current = 5.5;
    range::tuple <double &, double> pair (current, 2.25);
    auto plus_assign_with = range::curry::call_unpack (plus_assign);
    double & current2 = plus_assign_with (pair);
    BOOST_CHECK_EQUAL (current, 7.75);
    BOOST_CHECK_EQUAL (&current2, &current);
}

BOOST_AUTO_TEST_SUITE_END()
