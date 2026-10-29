// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_empty_view
#include <boost/test/unit_test.hpp>

#include "range/empty_view.hpp"

BOOST_AUTO_TEST_SUITE(test_range_empty_view)

BOOST_AUTO_TEST_CASE(test_range_empty_view)
{
    range::empty_view v;

    typedef decltype(range::empty(v)) is_empty;
    static_assert(is_empty::value, "");

    typedef decltype(range::empty(v, range::back)) is_empty_back;
    static_assert(is_empty_back::value, "");

    typedef decltype(range::size(v)) size;
    static_assert(size::value == 0u, "");

    typedef decltype(range::size(v, range::back)) size_back;
    static_assert(size_back::value == 0u, "");
}

BOOST_AUTO_TEST_SUITE_END()
