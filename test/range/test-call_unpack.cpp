// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test call_unpack.
call_unpack is particularly useful in combination with "zip".
Therefore, this file also tests this directly.
*/

#define BOOST_TEST_MODULE test_range_call_unpack
#include "boost/test/unit_test.hpp"

#include "range/call_unpack.hpp"

#include <tuple>
#include <vector>

#include "utility/returns.hpp"

#include "rime/check/check_equal.hpp"

#include "range/core.hpp"
#include "range/for_each.hpp"
#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"
#include "range/transform.hpp"
#include "range/tuple.hpp"
#include "range/zip.hpp"

using range::call_unpack;
using range::transform;
using range::tuple;
using range::zip;

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

/* Integration with zip. */

BOOST_AUTO_TEST_CASE(test_call_unpack_add)
{
    std::vector<double> v1;
    v1.push_back(7.5);
    v1.push_back(17);
    v1.push_back(99);
    std::vector<double> v2;
    v2.push_back(8.5);
    v2.push_back(18);

    range::for_each(zip(v1, v2), call_unpack(plus_assign));

    BOOST_CHECK_EQUAL(v1[0], 7.5 + 8.5);
    BOOST_CHECK_EQUAL(v1[1], 17. + 18);
    // zip stops iff one of the ranges is empty, so v1 [2] is never touched.
    BOOST_CHECK_EQUAL(v1[2], 99.);
}

struct difference
{
    template <class Left, class Right>
    auto operator()(Left left, Right right) const RETURNS(right - left);
};

BOOST_AUTO_TEST_CASE(test_call_unpack_difference)
{
    tuple<int, double> t1(1, 5.5);
    tuple<float, double> t2(2.5f, 7.5);

    auto differences = transform(zip(t1, t2), call_unpack(difference()));
    auto e1 = range::first(differences);
    auto e2 = range::second(differences);

    RIME_CHECK_EQUAL(e1, 1.5f);
    RIME_CHECK_EQUAL(e2, 2.);
}

BOOST_AUTO_TEST_SUITE_END()
