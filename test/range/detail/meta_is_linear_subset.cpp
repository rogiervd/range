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

#define BOOST_TEST_MODULE test_range_detail_linear_subset
#include "utility/test/boost_unit_test.hpp"

#include "range/detail/meta_is_linear_subset.hpp"

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_range_detail_linear_subset)

BOOST_AUTO_TEST_CASE (test_range_detail_linear_subset) {
    using range::detail::is_linear_subset;

    BOOST_MPL_ASSERT ((is_linear_subset <meta::vector<>, meta::vector<>>));
    BOOST_MPL_ASSERT ((is_linear_subset <meta::vector<>, meta::vector <int>>));
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector<>, meta::vector <int, bool>>));

    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <int>, meta::vector <>>));
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <int, bool>, meta::vector <>>));


    // Subset of one.
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int>, meta::vector <int>>));
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int>, meta::vector <int, bool>>));
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int>, meta::vector <bool, int>>));

    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int>, meta::vector <int, bool, long>>));
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int>, meta::vector <long, int, bool>>));
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int>, meta::vector <long, bool, int>>));

    // Subset of multiple.
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int, long>, meta::vector <int, long>>));

    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <int, long>, meta::vector <int, bool, long>>));
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <long, int>, meta::vector <long, int, bool>>));
    BOOST_MPL_ASSERT ((is_linear_subset <
            meta::vector <long, int>, meta::vector <long, bool, int>>));

    // Not a subset.
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <char>, meta::vector <int>>));
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <char>, meta::vector <int, bool>>));
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <char>, meta::vector <int, bool, long>>));

    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <int, char>, meta::vector <int, long>>));

    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <char, long>, meta::vector <int, bool, long>>));
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <char, int>, meta::vector <long, int, bool>>));
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <long, char>, meta::vector <long, bool, int>>));

    // Not a linear subset because of the order.
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <long, int>, meta::vector <int, bool, long>>));
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <int, long>, meta::vector <long, int, bool>>));
    BOOST_MPL_ASSERT_NOT ((is_linear_subset <
            meta::vector <int, long>, meta::vector <long, bool, int>>));
}

BOOST_AUTO_TEST_SUITE_END()
