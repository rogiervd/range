// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test less_lexicographical on homogeneous ranges, heterogeneous ranges, mixes
of them, and ranges with compile-time constants.
*/

#define BOOST_TEST_MODULE range_test_less_lexicographical
#include "boost/test/unit_test.hpp"

#include "range/less_lexicographical.hpp"

#include <functional>
#include <string>
#include <tuple>
#include <vector>

#include "utility/returns.hpp"

#include "range/core.hpp"
#include "range/reverse.hpp"
#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"
#include "range/transform.hpp"

#include "rime/check/check_equal.hpp"
#include "rime/core.hpp"

struct negate
{
    int operator()(int i) const { return -i; }
};

BOOST_AUTO_TEST_SUITE(range_test_less_lexicographical)

#define CHECK_range_less_lexicographical(r1, r2, value)                      \
    RIME_CHECK_EQUAL(range::less_lexicographical(r1, r2), value);            \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(r1, r2, range::front), value);           \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(                                         \
            range::reverse(r1), range::reverse(r2), range::back),            \
        value);                                                              \
                                                                             \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(r1, r2, std::less<int>()), value);       \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(r1, r2, range::front, std::less<int>()), \
        value);                                                              \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(                                         \
            range::reverse(r1), range::reverse(r2), range::back,             \
            std::less<int>()),                                               \
        value);                                                              \
                                                                             \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(                                         \
            range::transform(r1, negate()), range::transform(r2, negate()),  \
            std::greater<int>()),                                            \
        value);                                                              \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(                                         \
            range::transform(r1, negate()), range::transform(r2, negate()),  \
            range::front, std::greater<int>()),                              \
        value);                                                              \
    RIME_CHECK_EQUAL(                                                        \
        range::less_lexicographical(                                         \
            range::transform(range::reverse(r1), negate()),                  \
            range::transform(range::reverse(r2), negate()), range::back,     \
            std::greater<int>()),                                            \
        value)

BOOST_AUTO_TEST_CASE(test_range_less_lexicographical_homogeneous)
{
    std::vector<int> v1, v2;

    CHECK_range_less_lexicographical(v1, v2, false);

    v1.push_back(1);
    CHECK_range_less_lexicographical(v1, v2, false);
    CHECK_range_less_lexicographical(v2, v1, true);

    v2.push_back(2);
    CHECK_range_less_lexicographical(v1, v2, true);
    CHECK_range_less_lexicographical(v2, v1, false);

    v2.pop_back();
    v2.push_back(1);
    CHECK_range_less_lexicographical(v1, v2, false);

    v2.push_back(3);
    CHECK_range_less_lexicographical(v1, v2, true);
    CHECK_range_less_lexicographical(v2, v1, false);

    v1.push_back(4);
    CHECK_range_less_lexicographical(v1, v2, false);
    CHECK_range_less_lexicographical(v2, v1, true);

    v1.pop_back();
    v1.push_back(3);
    CHECK_range_less_lexicographical(v1, v2, false);
}

BOOST_AUTO_TEST_CASE(test_range_less_lexicographical_heterogeneous)
{
    CHECK_range_less_lexicographical(
        std::make_tuple(), std::make_tuple(), rime::false_);

    // Different lengths: known at compile-time.
    CHECK_range_less_lexicographical(
        std::make_tuple(), std::make_tuple(1), rime::true_);
    CHECK_range_less_lexicographical(
        std::make_tuple(1), std::make_tuple(), rime::false_);
    CHECK_range_less_lexicographical(
        std::make_tuple(), std::make_tuple(1, 2), rime::true_);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2), std::make_tuple(), rime::false_);

    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2), std::make_tuple(1), false);
    CHECK_range_less_lexicographical(
        std::make_tuple(1), std::make_tuple(1, 2), true);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2, 3), std::make_tuple(1, 2), false);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2), std::make_tuple(1, 2, 3), true);

    // The same lengths: the result depends on the actual values.
    CHECK_range_less_lexicographical(
        std::make_tuple(5), std::make_tuple(5), false);
    CHECK_range_less_lexicographical(
        std::make_tuple(3), std::make_tuple(7), true);
    CHECK_range_less_lexicographical(
        std::make_tuple(7), std::make_tuple(3), false);

    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2), std::make_tuple(1, 2), false);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2), std::make_tuple(1, 3), true);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 3), std::make_tuple(1, 2), false);
    CHECK_range_less_lexicographical(
        std::make_tuple(5, 2), std::make_tuple(1, 2), false);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2), std::make_tuple(5, 2), true);

    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2, 3), std::make_tuple(1, 2, 3), false);

    CHECK_range_less_lexicographical(
        std::make_tuple(6, 2, 3), std::make_tuple(5, 2, 3), false);
    CHECK_range_less_lexicographical(
        std::make_tuple(5, 2, 3), std::make_tuple(6, 2, 3), true);

    CHECK_range_less_lexicographical(
        std::make_tuple(1, 7, 3), std::make_tuple(1, 8, 3), true);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 8, 3), std::make_tuple(1, 7, 3), false);

    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2, 3), std::make_tuple(1, 2, 4), true);
    CHECK_range_less_lexicographical(
        std::make_tuple(1, 2, 4), std::make_tuple(1, 2, 3), false);
}

BOOST_AUTO_TEST_CASE(test_range_less_lexicographical_types)
{
    std::tuple<char> t1('a');
    std::tuple<char, std::string> t2('a', "bye");
    std::tuple<char, char const *> t3('a', "hello");
    std::tuple<char, std::string> t4('r', "hello");

    RIME_CHECK_EQUAL(range::less_lexicographical(t1, t1), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t1, t2), true);
    RIME_CHECK_EQUAL(range::less_lexicographical(t1, t3), true);
    RIME_CHECK_EQUAL(range::less_lexicographical(t1, t4), true);

    RIME_CHECK_EQUAL(range::less_lexicographical(t2, t1), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t2, t2), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t2, t3), true);
    RIME_CHECK_EQUAL(range::less_lexicographical(t2, t4), true);

    RIME_CHECK_EQUAL(range::less_lexicographical(t3, t1), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t3, t2), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t3, t3), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t3, t4), true);

    RIME_CHECK_EQUAL(range::less_lexicographical(t4, t1), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t4, t2), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t4, t3), false);
    RIME_CHECK_EQUAL(range::less_lexicographical(t4, t4), false);
}

// Mix homogeneous and heterogeneous ranges.
BOOST_AUTO_TEST_CASE(test_range_less_lexicographical_mixed)
{
    std::vector<int> v1;

    CHECK_range_less_lexicographical(v1, std::make_tuple(), rime::false_);
    CHECK_range_less_lexicographical(std::make_tuple(), v1, false);
    CHECK_range_less_lexicographical(v1, std::make_tuple(1), true);
    CHECK_range_less_lexicographical(std::make_tuple(1), v1, false);
    CHECK_range_less_lexicographical(v1, std::make_tuple(1, 2), true);
    CHECK_range_less_lexicographical(std::make_tuple(1, 2), v1, false);

    v1.push_back(1);
    // The following works around strange behaviour of CLang with optimisations:
    // It seems to perform a 64-bit comparison instead of 32-bit.
    // Maybe this is somehow correct, but it surprises Valgrind too much.
    v1.push_back(4);
    v1.pop_back();
    // End workaround.

    CHECK_range_less_lexicographical(v1, std::make_tuple(1), false);
    CHECK_range_less_lexicographical(std::make_tuple(1), v1, false);
    CHECK_range_less_lexicographical(v1, std::make_tuple(), rime::false_);
    CHECK_range_less_lexicographical(std::make_tuple(), v1, true);
    CHECK_range_less_lexicographical(v1, std::make_tuple(2), true);
    CHECK_range_less_lexicographical(std::make_tuple(2), v1, false);
    CHECK_range_less_lexicographical(v1, std::make_tuple(1, 3), true);
    CHECK_range_less_lexicographical(std::make_tuple(1, 3), v1, false);

    v1.push_back(4);

    v1.push_back(8);
    v1.pop_back();

    CHECK_range_less_lexicographical(v1, std::make_tuple(1, 4), false);
    CHECK_range_less_lexicographical(std::make_tuple(1, 4), v1, false);
    CHECK_range_less_lexicographical(v1, std::make_tuple(), rime::false_);
    CHECK_range_less_lexicographical(std::make_tuple(), v1, true);
    CHECK_range_less_lexicographical(v1, std::make_tuple(1), false);
    CHECK_range_less_lexicographical(std::make_tuple(1), v1, true);
    CHECK_range_less_lexicographical(v1, std::make_tuple(1, 3), false);
    CHECK_range_less_lexicographical(std::make_tuple(1, 3), v1, true);
    CHECK_range_less_lexicographical(v1, std::make_tuple(1, 4, 5), true);
    CHECK_range_less_lexicographical(std::make_tuple(1, 4, 5), v1, false);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(range_test_less_lexicographical_constant)

// Different from the helpers above: these work on compile-time constants.
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

using range::reverse;
using range::transform;

#undef CHECK_range_less_lexicographical
#define CHECK_range_less_lexicographical(r1, r2, value)                      \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2), value);                   \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2, range::front), value);     \
    RIME_CHECK_EQUAL(                                                        \
        less_lexicographical(reverse(r1), reverse(r2), range::back), value); \
                                                                             \
    RIME_CHECK_EQUAL(less_lexicographical(r1, r2, less()), value);           \
    RIME_CHECK_EQUAL(                                                        \
        less_lexicographical(r1, r2, range::front, less()), value);          \
    RIME_CHECK_EQUAL(                                                        \
        less_lexicographical(reverse(r1), reverse(r2), range::back, less()), \
        value);                                                              \
                                                                             \
    RIME_CHECK_EQUAL(                                                        \
        less_lexicographical(                                                \
            transform(r1, negate()), transform(r2, negate()), greater()),    \
        value);                                                              \
    RIME_CHECK_EQUAL(                                                        \
        less_lexicographical(                                                \
            transform(r1, negate()), transform(r2, negate()), range::front,  \
            greater()),                                                      \
        value);                                                              \
    RIME_CHECK_EQUAL(                                                        \
        less_lexicographical(                                                \
            transform(reverse(r1), negate()),                                \
            transform(reverse(r2), negate()), range::back, greater()),       \
        value)

BOOST_AUTO_TEST_CASE(test_range_less_lexicographical_constant)
{
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<5>()), std::make_tuple(rime::int_<5>()),
        rime::false_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<3>()), std::make_tuple(rime::int_<7>()),
        rime::true_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<7>()), std::make_tuple(rime::int_<3>()),
        rime::false_);

    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<2>()),
        std::make_tuple(rime::int_<1>(), rime::int_<2>()), rime::false_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<2>()),
        std::make_tuple(rime::int_<1>(), rime::int_<3>()), rime::true_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<3>()),
        std::make_tuple(rime::int_<1>(), rime::int_<2>()), rime::false_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<5>(), rime::int_<2>()),
        std::make_tuple(rime::int_<1>(), rime::int_<2>()), rime::false_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<2>()),
        std::make_tuple(rime::int_<5>(), rime::int_<2>()), rime::true_);

    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        std::make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        rime::false_);

    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<6>(), rime::int_<2>(), rime::int_<3>()),
        std::make_tuple(rime::int_<5>(), rime::int_<2>(), rime::int_<3>()),
        rime::false_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<5>(), rime::int_<2>(), rime::int_<3>()),
        std::make_tuple(rime::int_<6>(), rime::int_<2>(), rime::int_<3>()),
        rime::true_);

    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<7>(), rime::int_<3>()),
        std::make_tuple(rime::int_<1>(), rime::int_<8>(), rime::int_<3>()),
        rime::true_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<8>(), rime::int_<3>()),
        std::make_tuple(rime::int_<1>(), rime::int_<7>(), rime::int_<3>()),
        rime::false_);

    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        std::make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<4>()),
        rime::true_);
    CHECK_range_less_lexicographical(
        std::make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<4>()),
        std::make_tuple(rime::int_<1>(), rime::int_<2>(), rime::int_<3>()),
        rime::false_);
}

BOOST_AUTO_TEST_SUITE_END()
