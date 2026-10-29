// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_tuple_basic
#include <boost/test/unit_test.hpp>

#include "range/tuple.hpp"

#include <tuple>
#include "range/std/tuple.hpp"

#include "rime/check/check_equal.hpp"

#include "tuple-basic_tests.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_basic)

BOOST_AUTO_TEST_CASE(tuple_basic)
{
    check_empty(tuple<>());
    check_empty(view(tuple<>()));
    check_empty(view_once(tuple<>()));

    check_one_element(tuple<int>(5), 5);
    check_one_element(view(tuple<int>(5)), 5);
    check_one_element(view_once(tuple<int>(5)), 5);

    check_two_elements(tuple<int, char>(5, 'A'), 5, 'A');
    check_two_elements(view(tuple<int, char>(5, 'A')), 5, 'A');
    check_two_elements(view_once(tuple<int, char>(5, 'A')), 5, 'A');

    // Checks on three element are in the other file.
    // Otherwise, compilers start using more than 1GB of memory.
}

BOOST_AUTO_TEST_SUITE_END()
