/*
Copyright 2014, 2015 Rogier van Dalen.

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
