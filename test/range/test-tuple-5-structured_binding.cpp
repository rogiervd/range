// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test structured binding for range::tuple.
*/

#define BOOST_TEST_MODULE range_test_tuple_less
#include <boost/test/unit_test.hpp>

#include "range/tuple.hpp"

#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include "range/std/vector.hpp"

BOOST_AUTO_TEST_SUITE(range_test_tuple_structured_binding)


BOOST_AUTO_TEST_CASE(compile_time)
{
    using range::tuple;
    static_assert(std::tuple_size<tuple<char>>::value == 1);
    static_assert(std::tuple_size<tuple<char, int>>::value == 2);
    static_assert(std::tuple_size<tuple<char, long, bool>>::value == 3);

    static_assert(
        std::is_same_v<std::tuple_element<0, tuple<char>>::type, char>);

    static_assert(
        std::is_same_v<std::tuple_element<0, tuple<char, int>>::type, char>);
    static_assert(
        std::is_same_v<std::tuple_element<1, tuple<char, int>>::type, int>);

    using tuple_3 = tuple<char, long &, bool const &>;
    static_assert(std::is_same_v<std::tuple_element<0, tuple_3>::type, char>);
    static_assert(std::is_same_v<std::tuple_element<1, tuple_3>::type, long &>);
    static_assert(
        std::is_same_v<std::tuple_element<2, tuple_3>::type, bool const &>);
}


BOOST_AUTO_TEST_CASE(run_time)
{
    using range::tuple;
    tuple<int> t1(27);
    tuple<char, int> t2('c', 28);
    tuple<char, int, bool> t3('c', 29, true);

    auto [i1] = t1;
    BOOST_CHECK_EQUAL(i1, 27);

    auto [c2, i2] = t2;
    BOOST_CHECK_EQUAL(c2, 'c');
    BOOST_CHECK_EQUAL(i2, 28);

    auto [c3, i3, b3] = t3;
    BOOST_CHECK_EQUAL(c3, 'c');
    BOOST_CHECK_EQUAL(i3, 29);
    BOOST_CHECK_EQUAL(b3, true);
}

BOOST_AUTO_TEST_CASE(return_types)
{
    long l = 34;
    std::string s = "Hello";
    int i = 27;
    bool b = true;

    using type = range::tuple<long, std::string const, int &, bool const &>;
    using range::get;

    // The following checks pass with std::tuple and std::get, so presumably
    // this is the correct behaviour.
    {
        type t(l, s, i, b);

        BOOST_CHECK_EQUAL(get<0>(t), 34);
        BOOST_CHECK_EQUAL(get<1>(t), "Hello");
        BOOST_CHECK_EQUAL(get<2>(t), 27);
        BOOST_CHECK_EQUAL(get<3>(t), true);

        // tuple passed in as const reference.
        static_assert(std::is_same_v<decltype(get<0>(t)), long &>);
        static_assert(std::is_same_v<decltype(get<1>(t)), std::string const &>);
        static_assert(std::is_same_v<decltype(get<2>(t)), int &>);
        static_assert(std::is_same_v<decltype(get<3>(t)), bool const &>);

        // tuple passed in as rvalue reference.
        static_assert(std::is_same_v<decltype(get<0>(std::move(t))), long &&>);
        static_assert(std::is_same_v<
                      decltype(get<1>(std::move(t))), std::string const &&>);
        static_assert(std::is_same_v<decltype(get<2>(std::move(t))), int &>);
        static_assert(
            std::is_same_v<decltype(get<3>(std::move(t))), bool const &>);
    }

    {
        type const t(l, s, i, b);

        BOOST_CHECK_EQUAL(get<0>(t), 34);
        BOOST_CHECK_EQUAL(get<1>(t), "Hello");
        BOOST_CHECK_EQUAL(get<2>(t), 27);
        BOOST_CHECK_EQUAL(get<3>(t), true);

        // tuple passed in as const reference.
        static_assert(std::is_same_v<decltype(get<0>(t)), long const &>);
        static_assert(std::is_same_v<decltype(get<1>(t)), std::string const &>);
        static_assert(std::is_same_v<decltype(get<2>(t)), int &>);
        static_assert(std::is_same_v<decltype(get<3>(t)), bool const &>);
    }
}

BOOST_AUTO_TEST_CASE(example)
{
    std::vector<range::tuple<int, std::string>> v {
        {0, "zero"}, {1, "one"}, {2, "two"}};

    for (auto [number, name] : v) {
        if (number == 0) {
            BOOST_CHECK_EQUAL(name, "zero");
        } else if (number == 1) {
            BOOST_CHECK_EQUAL(name, "one");
        } else {
            BOOST_CHECK_EQUAL(number, 2);
            BOOST_CHECK_EQUAL(name, "two");
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
