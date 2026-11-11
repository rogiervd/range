// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_call_unpack
#include "boost/test/unit_test.hpp"

#include "range/call_unpack.hpp"

#include <tuple>

#include "range/std/tuple.hpp"
#include "range/tuple.hpp"

using range::call_unpack;

BOOST_AUTO_TEST_SUITE(test_range_call_unpack)

int check_nullary() { return -7; }

char check_5u(unsigned five)
{
    BOOST_CHECK_EQUAL(five, 5u);
    return 'u';
}

double & plus_assign(double & target, double addendum)
{
    return target += addendum;
}

BOOST_AUTO_TEST_CASE(test_call_unpack)
{
    int minus_7 = call_unpack(check_nullary, range::tuple<>());
    BOOST_CHECK_EQUAL(minus_7, -7);

    auto u = call_unpack(check_5u, std::make_tuple(5u));
    BOOST_CHECK_EQUAL(u, 'u');

    double current = 5.5;
    range::tuple<double &, double> pair(current, 2.25);
    double & current2 = call_unpack(plus_assign, pair);
    BOOST_CHECK_EQUAL(current, 7.75);
    BOOST_CHECK_EQUAL(&current2, &current);
}

BOOST_AUTO_TEST_SUITE_END()
