// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE range_test_tuple_less_meta
#include "boost/test/unit_test.hpp"

#include "range/detail/tuple-compare.hpp"

#include "rime/core.hpp"

/// Get the predicate results with rime::less and as a vector.
/// This saves typing in the tests.
template <class LeftTypes, class RightTypes> struct predicate_results
: range::tuple_detail::less_lexicographical_detail::predicate_results<
      LeftTypes, RightTypes, rime::callable::less> {};

BOOST_AUTO_TEST_SUITE(range_test_tuple_less_meta)

BOOST_AUTO_TEST_CASE(test_meta)
{
    using boost::mpl::pair;
    using meta::vector;

    static_assert(
        std::is_same<
            predicate_results<vector<>, vector<>>::type,
            vector<pair<rime::false_type, rime::true_type>>>::value,
        "");
    static_assert(
        std::is_same<
            predicate_results<vector<int>, vector<>>::type,
            vector<
                pair<rime::false_type, rime::true_type>,
                pair<rime::false_type, rime::true_type>>>::value,
        "");
    static_assert(
        std::is_same<
            predicate_results<vector<>, vector<int>>::type,
            vector<
                pair<rime::true_type, rime::false_type>,
                pair<rime::false_type, rime::true_type>>>::value,
        "");

    static_assert(
        std::is_same<
            predicate_results<vector<int>, vector<int>>::type,
            vector<pair<bool, bool>, pair<rime::false_type, rime::true_type>>>::
            value,
        "");

    static_assert(
        std::is_same<
            predicate_results<
                vector<rime::int_<0>>, vector<rime::int_<0>>>::type,
            vector<
                pair<rime::constant<bool, false>, rime::constant<bool, false>>,
                pair<rime::false_type, rime::true_type>>>::value,
        "");
    static_assert(
        std::is_same<
            predicate_results<
                vector<rime::int_<0>>, vector<rime::int_<1>>>::type,
            vector<
                pair<rime::constant<bool, true>, rime::constant<bool, false>>,
                pair<rime::false_type, rime::true_type>>>::value,
        "");
    static_assert(
        std::is_same<
            predicate_results<
                vector<rime::int_<5>>, vector<rime::int_<1>>>::type,
            vector<
                pair<rime::constant<bool, false>, rime::constant<bool, true>>,
                pair<rime::false_type, rime::true_type>>>::value,
        "");

    // Longer.
    static_assert(
        std::is_same<
            predicate_results<
                vector<rime::int_<5>, int>, vector<rime::int_<1>, int>>::type,
            vector<
                pair<rime::constant<bool, false>, rime::constant<bool, true>>,
                pair<bool, bool>,
                pair<rime::false_type, rime::true_type>>>::value,
        "");
    // Right one longer than left.
    static_assert(
        std::is_same<
            predicate_results<
                vector<rime::int_<5>, int>,
                vector<rime::int_<1>, int, bool>>::type,
            vector<
                pair<rime::constant<bool, false>, rime::constant<bool, true>>,
                pair<bool, bool>, pair<rime::true_type, rime::false_type>,
                pair<rime::false_type, rime::true_type>>>::value,
        "");
}

BOOST_AUTO_TEST_SUITE_END()
