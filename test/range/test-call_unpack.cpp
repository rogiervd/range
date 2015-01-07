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
