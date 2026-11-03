// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_detail_linear_subset
#include <boost/test/unit_test.hpp>

#include "range/detail/meta_is_linear_subset.hpp"

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_range_detail_linear_subset)

BOOST_AUTO_TEST_CASE(test_range_detail_linear_subset)
{
    using range::detail::is_linear_subset;

    static_assert(is_linear_subset<meta::vector<>, meta::vector<>>::value);
    static_assert(is_linear_subset<meta::vector<>, meta::vector<int>>::value);
    static_assert(
        is_linear_subset<meta::vector<>, meta::vector<int, bool>>::value);

    static_assert(!is_linear_subset<meta::vector<int>, meta::vector<>>::value);
    static_assert(
        !is_linear_subset<meta::vector<int, bool>, meta::vector<>>::value);

    // Subset of one.
    static_assert(
        is_linear_subset<meta::vector<int>, meta::vector<int>>::value);
    static_assert(
        is_linear_subset<meta::vector<int>, meta::vector<int, bool>>::value);
    static_assert(
        is_linear_subset<meta::vector<int>, meta::vector<bool, int>>::value);

    static_assert(is_linear_subset<
                  meta::vector<int>, meta::vector<int, bool, long>>::value);
    static_assert(is_linear_subset<
                  meta::vector<int>, meta::vector<long, int, bool>>::value);
    static_assert(is_linear_subset<
                  meta::vector<int>, meta::vector<long, bool, int>>::value);

    // Subset of multiple.
    static_assert(is_linear_subset<
                  meta::vector<int, long>, meta::vector<int, long>>::value);

    static_assert(
        is_linear_subset<
            meta::vector<int, long>, meta::vector<int, bool, long>>::value);
    static_assert(
        is_linear_subset<
            meta::vector<long, int>, meta::vector<long, int, bool>>::value);
    static_assert(
        is_linear_subset<
            meta::vector<long, int>, meta::vector<long, bool, int>>::value);

    // Not a subset.
    static_assert(
        !is_linear_subset<meta::vector<char>, meta::vector<int>>::value);
    static_assert(
        !is_linear_subset<meta::vector<char>, meta::vector<int, bool>>::value);
    static_assert(!is_linear_subset<
                  meta::vector<char>, meta::vector<int, bool, long>>::value);

    static_assert(!is_linear_subset<
                  meta::vector<int, char>, meta::vector<int, long>>::value);

    static_assert(
        !is_linear_subset<
            meta::vector<char, long>, meta::vector<int, bool, long>>::value);
    static_assert(
        !is_linear_subset<
            meta::vector<char, int>, meta::vector<long, int, bool>>::value);
    static_assert(
        !is_linear_subset<
            meta::vector<long, char>, meta::vector<long, bool, int>>::value);

    // Not a linear subset because of the order.
    static_assert(
        !is_linear_subset<
            meta::vector<long, int>, meta::vector<int, bool, long>>::value);
    static_assert(
        !is_linear_subset<
            meta::vector<int, long>, meta::vector<long, int, bool>>::value);
    static_assert(
        !is_linear_subset<
            meta::vector<int, long>, meta::vector<long, bool, int>>::value);
}

BOOST_AUTO_TEST_SUITE_END()
