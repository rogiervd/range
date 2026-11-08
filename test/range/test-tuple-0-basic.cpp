// SPDX-FileCopyrightText: Copyright 2013-2015, 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_tuple_basic
#include "boost/test/unit_test.hpp"

#include "range/tuple.hpp"

#include <tuple>
#include "range/std/tuple.hpp"

#include "rime/check/check_equal.hpp"

#include "tuple-basic_tests.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_basic)

BOOST_AUTO_TEST_CASE(tuple_basic)
{
    // view() does not accept a temporary tuple, since the view would refer to
    // the destructed tuple.
    // view_once() does.
    static_assert(!range::has<range::callable::view(tuple<>)>::value);
    static_assert(!range::has<range::callable::view(tuple<int>)>::value);
    static_assert(range::has<range::callable::view_once(tuple<int>)>::value);

    tuple<> t0;
    check_empty(tuple<>());
    check_empty(view(t0));
    check_empty(view_once(tuple<>()));

    tuple<int> t1(5);
    check_one_element(tuple<int>(5), 5);
    check_one_element(view(t1), 5);
    check_one_element(view_once(tuple<int>(5)), 5);

    tuple<int, char> t2(5, 'A');
    check_two_elements(tuple<int, char>(5, 'A'), 5, 'A');
    check_two_elements(view(t2), 5, 'A');
    check_two_elements(view_once(tuple<int, char>(5, 'A')), 5, 'A');

    // Checks on three element are in the other file.
    // Otherwise, compilers start using more than 1GB of memory.
}

BOOST_AUTO_TEST_CASE(test_tuple_does_not_have_begin_end)
{
    static_assert(
        !range::has<range::callable::begin(range::tuple<int, double>)>::value);
    static_assert(
        !range::has<range::callable::end(range::tuple<int, double>)>::value);
}


BOOST_AUTO_TEST_SUITE_END()
