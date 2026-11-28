// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test order comparison on tuples.
less_lexicographical and <, >, <=, >= are tested.
These tests are very similar to the ones in test-less-lexicographical-*.cpp.
This file tests heterogeneous tuples, tuples with different types, tuples with
compile-time constants, and the metafunctions used to implement comparison.
*/

#define BOOST_TEST_MODULE range_test_tuple_less
#include "boost/test/unit_test.hpp"

#include <functional>
#include <string>
#include <type_traits>

#include "boost/mpl/pair.hpp"

#include "meta/vector.hpp"

#include "utility/returns.hpp"

#include "range/core.hpp"
#include "range/detail/tuple-compare.hpp"
#include "range/less_lexicographical.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"
#include "range/tuple.hpp"

#include "rime/check/check_equal.hpp"
#include "rime/core.hpp"

struct negate
{
    double operator()(double i) const { return -i; }
};

BOOST_AUTO_TEST_SUITE(range_test_tuple_less)

using range::less_lexicographical;
using range::make_tuple;
using range::make_tuple_from;

using range::back;
using range::drop;
using range::front;

using range::reverse;
using range::transform;

#define CHECK_tuple_view_less(r1, r2, value)                                   \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2), value);                     \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2, front), value);              \
    RIME_CHECK_EQUAL(                                                          \
        less_lexicographical(                                                  \
            make_tuple_from(reverse(r1)), make_tuple_from(reverse(r2)), back), \
        value);                                                                \
                                                                               \
    RIME_CHECK_EQUAL(                                                          \
        less_lexicographical(r1, r2, std::less<double>()), value);             \
    RIME_CHECK_EQUAL(                                                          \
        less_lexicographical(r1, r2, front, std::less<double>()), value);      \
    RIME_CHECK_EQUAL(                                                          \
        less_lexicographical(                                                  \
            make_tuple_from(reverse(r1)), make_tuple_from(reverse(r2)), back,  \
            std::less<double>()),                                              \
        value);                                                                \
                                                                               \
    RIME_CHECK_EQUAL(                                                          \
        less_lexicographical(                                                  \
            make_tuple_from(range::transform(r1, negate())),                   \
            make_tuple_from(range::transform(r2, negate())),                   \
            std::greater<double>()),                                           \
        value);                                                                \
    RIME_CHECK_EQUAL(                                                          \
        less_lexicographical(                                                  \
            make_tuple_from(range::transform(r1, negate())),                   \
            make_tuple_from(range::transform(r2, negate())), front,            \
            std::greater<double>()),                                           \
        value);                                                                \
    RIME_CHECK_EQUAL(                                                          \
        less_lexicographical(                                                  \
            make_tuple_from(range::transform(reverse(r1), negate())),          \
            make_tuple_from(range::transform(reverse(r2), negate())), back,    \
            std::greater<double>()),                                           \
        value)

#define CHECK_tuple_less(r1, r2, value)   \
    CHECK_tuple_view_less(r1, r2, value); \
    RIME_CHECK_EQUAL((r1 < r2), value);   \
    RIME_CHECK_EQUAL(!(r1 >= r2), value); \
    RIME_CHECK_EQUAL((r2 > r1), value);   \
    RIME_CHECK_EQUAL(!(r2 <= r1), value)

BOOST_AUTO_TEST_CASE(heterogeneous)
{
    CHECK_tuple_less(make_tuple(), make_tuple(), rime::false_);

    // Different lengths: known at compile-time.
    CHECK_tuple_less(make_tuple(), make_tuple(1), rime::true_);
    CHECK_tuple_less(make_tuple(1), make_tuple(), rime::false_);
    CHECK_tuple_less(make_tuple(), make_tuple(1, 2), rime::true_);
    CHECK_tuple_less(make_tuple(1, 2), make_tuple(), rime::false_);

    CHECK_tuple_less(make_tuple(1, 2), make_tuple(1), false);
    CHECK_tuple_less(make_tuple(1), make_tuple(1, 2), true);
    CHECK_tuple_less(make_tuple(1, 2, 3), make_tuple(1, 2), false);
    CHECK_tuple_less(make_tuple(1, 2), make_tuple(1, 2, 3), true);

    // The same lengths: the result depends on the actual values.
    CHECK_tuple_less(make_tuple(5), make_tuple(5), false);
    CHECK_tuple_less(make_tuple(3), make_tuple(7), true);
    CHECK_tuple_less(make_tuple(7), make_tuple(3), false);

    CHECK_tuple_less(make_tuple(1, 2), make_tuple(1, 2), false);
    CHECK_tuple_less(make_tuple(1, 2), make_tuple(1, 3), true);
    CHECK_tuple_less(make_tuple(1, 3), make_tuple(1, 2), false);
    CHECK_tuple_less(make_tuple(5, 2), make_tuple(1, 2), false);
    CHECK_tuple_less(make_tuple(1, 2), make_tuple(5, 2), true);

    CHECK_tuple_view_less(make_tuple(1, 2), drop(make_tuple(7., 1, 2)), false);
    CHECK_tuple_view_less(drop(make_tuple(7., 1, 2)), make_tuple(1, 3), true);
    CHECK_tuple_view_less(
        drop(make_tuple(7., 1, 3)), drop(make_tuple(7., 1, 2)), false);
    CHECK_tuple_view_less(make_tuple(5, 2), drop(make_tuple(7., 1, 2)), false);
    CHECK_tuple_view_less(
        drop(make_tuple(7., 1, 2)), drop(make_tuple(7., 5, 2)), true);

    CHECK_tuple_less(make_tuple(1, 2, 3), make_tuple(1, 2, 3), false);

    CHECK_tuple_less(make_tuple(6, 2, 3), make_tuple(5, 2, 3), false);
    CHECK_tuple_less(make_tuple(5, 2, 3), make_tuple(6, 2, 3), true);

    CHECK_tuple_less(make_tuple(1, 7, 3), make_tuple(1, 8, 3), true);
    CHECK_tuple_less(make_tuple(1, 8, 3), make_tuple(1, 7, 3), false);

    CHECK_tuple_less(make_tuple(1, 2, 3), make_tuple(1, 2, 4), true);
    CHECK_tuple_less(make_tuple(1, 2, 4), make_tuple(1, 2, 3), false);
}

BOOST_AUTO_TEST_CASE(types)
{
    range::tuple<char> t1('a');
    range::tuple<char, std::string> t2('a', "bye");
    range::tuple<char, char const *> t3('a', "hello");
    range::tuple<char, std::string> t4('r', "hello");

    RIME_CHECK_EQUAL(less_lexicographical(t1, t1), false);
    RIME_CHECK_EQUAL(less_lexicographical(t1, t2), true);
    RIME_CHECK_EQUAL(less_lexicographical(t1, t3), true);
    RIME_CHECK_EQUAL(less_lexicographical(t1, t4), true);

    RIME_CHECK_EQUAL(less_lexicographical(t2, t1), false);
    RIME_CHECK_EQUAL(less_lexicographical(t2, t2), false);
    RIME_CHECK_EQUAL(less_lexicographical(t2, t3), true);
    RIME_CHECK_EQUAL(less_lexicographical(t2, t4), true);

    RIME_CHECK_EQUAL(less_lexicographical(t3, t1), false);
    RIME_CHECK_EQUAL(less_lexicographical(t3, t2), false);
    RIME_CHECK_EQUAL(less_lexicographical(t3, t3), false);
    RIME_CHECK_EQUAL(less_lexicographical(t3, t4), true);

    RIME_CHECK_EQUAL(less_lexicographical(t4, t1), false);
    RIME_CHECK_EQUAL(less_lexicographical(t4, t2), false);
    RIME_CHECK_EQUAL(less_lexicographical(t4, t3), false);
    RIME_CHECK_EQUAL(less_lexicographical(t4, t4), false);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(range_test_tuple_less_constant)

// Comparison of tuples with compile-time constants is implemented specially for
// comparison from the front only, so that is all that is tested.

struct less
{
    template <class Left, class Right>
    auto operator()(Left left, Right right) const RETURNS(left < right);
};

struct greater
{
    template <class Left, class Right>
    auto operator()(Left left, Right right) const RETURNS(left > right);
};

struct negate
{
    template <class Type> auto operator()(Type i) const RETURNS(-i);
};

using range::less_lexicographical;
using range::make_tuple;
using range::make_tuple_from;

using range::drop;
using range::front;

using range::transform;

#undef CHECK_tuple_view_less
#define CHECK_tuple_view_less(r1, r2, value)                              \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2), value);                \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2, front), value);         \
                                                                          \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2, less()), value);        \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2, front, less()), value); \
                                                                          \
    RIME_CHECK_EQUAL(                                                     \
        less_lexicographical(                                             \
            make_tuple_from(range::transform(r1, negate())),              \
            make_tuple_from(range::transform(r2, negate())), greater()),  \
        value);                                                           \
    RIME_CHECK_EQUAL(                                                     \
        less_lexicographical(                                             \
            make_tuple_from(range::transform(r1, negate())),              \
            make_tuple_from(range::transform(r2, negate())), front,       \
            greater()),                                                   \
        value)

// CHECK_tuple_less, defined above, uses this CHECK_tuple_view_less.

BOOST_AUTO_TEST_CASE(constants)
{
    CHECK_tuple_less(
        make_tuple(rime::int_<5>()), make_tuple(rime::int_<5>()), rime::false_);
    CHECK_tuple_less(
        make_tuple(rime::int_<3>()), make_tuple(rime::int_<7>()), rime::true_);
    CHECK_tuple_less(
        make_tuple(rime::int_<7>()), make_tuple(rime::int_<3>()), rime::false_);

    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>()),
        make_tuple(rime::int_<1>(), rime::int_<2>()), rime::false_);
    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>()),
        make_tuple(rime::int_<1>(), rime::int_<3>(), rime::int_<4>()),
        rime::true_);
    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<3>()),
        make_tuple(rime::int_<1>(), rime::int_<2>()), rime::false_);
    CHECK_tuple_less(
        make_tuple(rime::int_<5>(), rime::int_<2>()),
        make_tuple(rime::int_<1>(), rime::int_<2>()), rime::false_);
    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>()),
        make_tuple(rime::int_<5>(), rime::int_<2>()), rime::true_);

    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        rime::false_);

    CHECK_tuple_less(
        make_tuple(rime::int_<6>(), rime::int_<2>(), rime::int_<3>()),
        make_tuple(rime::int_<5>(), rime::int_<2>(), rime::int_<3>()),
        rime::false_);
    CHECK_tuple_less(
        make_tuple(rime::int_<5>(), rime::int_<2>(), rime::int_<3>()),
        make_tuple(rime::int_<6>(), rime::int_<2>(), rime::int_<3>()),
        rime::true_);

    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<7>(), rime::int_<3>()),
        make_tuple(rime::int_<1>(), rime::int_<8>(), rime::int_<3>()),
        rime::true_);
    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<8>(), rime::int_<3>()),
        make_tuple(rime::int_<1>(), rime::int_<7>(), rime::int_<3>()),
        rime::false_);

    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<4>()),
        rime::true_);
    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<4>()),
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        rime::false_);

    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<4>()),
        make_tuple(rime::int_<1>(), rime::int_<2>()), rime::false_);
    CHECK_tuple_less(
        make_tuple(rime::int_<1>(), rime::int_<2>()),
        make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        rime::true_);
}

BOOST_AUTO_TEST_SUITE_END()

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

    static_assert(std::is_same_v<
                  predicate_results<vector<>, vector<>>::type,
                  vector<pair<rime::false_type, rime::true_type>>>);
    static_assert(std::is_same_v<
                  predicate_results<vector<int>, vector<>>::type,
                  vector<
                      pair<rime::false_type, rime::true_type>,
                      pair<rime::false_type, rime::true_type>>>);
    static_assert(std::is_same_v<
                  predicate_results<vector<>, vector<int>>::type,
                  vector<
                      pair<rime::true_type, rime::false_type>,
                      pair<rime::false_type, rime::true_type>>>);

    static_assert(
        std::is_same_v<
            predicate_results<vector<int>, vector<int>>::type,
            vector<pair<bool, bool>, pair<rime::false_type, rime::true_type>>>);

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
