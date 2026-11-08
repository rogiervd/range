// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_tuple_types
#include "boost/test/unit_test.hpp"

#include "range/tuple.hpp"

#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "boost/mpl/assert.hpp"

#include "utility/test/tracked.hpp"

#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_types)

using range::element_type;
using range::tuple;

using range::at;
using range::at_c;
using range::drop;
using range::first;
using range::second;
using range::size;
using range::view;
using range::view_once;

using range::always_empty;
using range::never_empty;

rime::size_t<0> zero;
rime::size_t<1> one;
rime::size_t<2> two;

BOOST_AUTO_TEST_CASE(tuple_element_type)
{
    static_assert(
        std::is_same_v<range::element_type<0, tuple<int>>::type, int>);

    static_assert(std::is_same_v<
                  range::element_type<0, tuple<int, bool &> &>::type, int>);
    static_assert(
        std::is_same_v<
            range::element_type<1, tuple<int, bool &> const>::type, bool &>);

    static_assert(
        std::is_same_v<
            range::element_type<0, tuple<double const, bool, float> &&>::type,
            double const>);
    static_assert(std::is_same_v<
                  range::element_type<
                      1, tuple<double const, bool, float> const &>::type,
                  bool>);
    static_assert(std::is_same_v<
                  range::element_type<
                      2, tuple<double const, bool, float> const>::type,
                  float>);
}

BOOST_AUTO_TEST_CASE(tuple_types)
{
    static_assert(always_empty<tuple<>, direction::front>::value);
    static_assert(always_empty<tuple<>, direction::back>::value);
    static_assert(never_empty<tuple<int>, direction::front>::value);
    static_assert(never_empty<tuple<int>, direction::back>::value);
    static_assert(never_empty<tuple<int, bool>, direction::front>::value);
    static_assert(never_empty<tuple<int, bool>, direction::back>::value);

    RIME_CHECK_EQUAL(size(tuple<>()), zero);
    RIME_CHECK_EQUAL(size(tuple<int>(1)), one);
    RIME_CHECK_EQUAL(size(tuple<int, bool>(2, false)), two);

    static_assert(!range::has<range::callable::first(tuple<>)>::value);
    static_assert(!range::has<range::callable::drop(tuple<>)>::value);
    static_assert(
        !range::has<range::callable::at(tuple<>, rime::size_t<0>)>::value);

    static_assert(range::has<range::callable::first(tuple<int>)>::value);
    static_assert(range::has<range::callable::drop(tuple<int>)>::value);
    static_assert(
        range::has<range::callable::at(tuple<int>, rime::size_t<0>)>::value);

    // Test reference, const reference, and
    // rvalue reference without and with view_once.
    {
        tuple<int, int> t;
        static_assert(std::is_same_v<decltype(first(t)), int &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int &>);

        static_assert(std::is_same_v<decltype(first(drop(t))), int &>);
        static_assert(std::is_same_v<decltype(second(t)), int &>);
        static_assert(std::is_same_v<decltype(at(t, one)), int &>);
    }
    {
        tuple<int, int> const t;
        static_assert(std::is_same_v<decltype(first(t)), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int const &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int const &>);

        static_assert(std::is_same_v<decltype(first(drop(t))), int const &>);
        static_assert(std::is_same_v<decltype(second(t)), int const &>);
        static_assert(std::is_same_v<decltype(at(t, one)), int const &>);
    }
    {
        // view() does not accept temporaries, so use const lvalues.
        static_assert(!range::has<range::callable::view(tuple<int>)>::value);
        tuple<int> const t1 {};
        tuple<int, int> const t2 {};
        static_assert(std::is_same_v<decltype(first(view(t1))), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(view(t1))), int const &>);
        static_assert(
            std::is_same_v<decltype(at(view(t1), zero)), int const &>);

        static_assert(
            std::is_same_v<decltype(first(drop(view(t2)))), int const &>);
        static_assert(std::is_same_v<decltype(second(view(t2))), int const &>);
        static_assert(std::is_same_v<decltype(at(view(t2), one)), int const &>);
    }
    {
        static_assert(
            std::is_same_v<decltype(first(view_once(tuple<int>()))), int &&>);
        static_assert(
            std::is_same_v<decltype(at_c<0>(view_once(tuple<int>()))), int &&>);
        static_assert(std::is_same_v<
                      decltype(at(view_once(tuple<int>()), zero)), int &&>);

        static_assert(
            std::is_same_v<
                decltype(first(drop(view_once(tuple<int, int>())))), int &&>);
        static_assert(std::is_same_v<
                      decltype(second(view_once(tuple<int, int>()))), int &&>);
        static_assert(std::is_same_v<
                      decltype(at(view_once(tuple<int, int>()), one)), int &&>);
    }
    {
        // first uses view_once.
        static_assert(std::is_same_v<decltype(first(tuple<int>())), int &&>);
        static_assert(std::is_same_v<decltype(at_c<0>(tuple<int>())), int &&>);
        static_assert(std::is_same_v<decltype(at(tuple<int>(), zero)), int &&>);

        // first (drop()) is not the same as second()!
        // first (drop()) uses view, second() uses view_once.
        static_assert(std::is_same_v<
                      decltype(first(drop(tuple<int, int>()))), int const &>);
        static_assert(
            std::is_same_v<decltype(second(tuple<int, int>())), int &&>);
        static_assert(
            std::is_same_v<decltype(at(tuple<int, int>(), one)), int &&>);
    }

    // The same on a const contained type.
    {
        tuple<int const> t;
        static_assert(std::is_same_v<decltype(first(t)), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int const &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int const &>);
    }
    {
        tuple<int const> const t;
        static_assert(std::is_same_v<decltype(first(t)), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int const &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int const &>);
    }
    {
        // view() does not accept temporaries.
        static_assert(
            !range::has<range::callable::view(tuple<int const>)>::value);
        tuple<int const> const t {};
        static_assert(std::is_same_v<decltype(first(view(t))), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(view(t))), int const &>);
        static_assert(std::is_same_v<decltype(at(view(t), zero)), int const &>);
    }
    {
        static_assert(
            std::is_same_v<
                decltype(first(view_once(tuple<int const>()))), int const &&>);
        static_assert(std::is_same_v<
                      decltype(at_c<0>(view_once(tuple<int const>()))),
                      int const &&>);
        static_assert(std::is_same_v<
                      decltype(at(view_once(tuple<int const>()), zero)),
                      int const &&>);
    }
    {
        static_assert(
            std::is_same_v<decltype(first(tuple<int const>())), int const &&>);
        static_assert(std::is_same_v<
                      decltype(at_c<0>(tuple<int const>())), int const &&>);
        static_assert(std::is_same_v<
                      decltype(at(tuple<int const>(), zero)), int const &&>);
    }

    int i = 5;
    // The same on a reference contained type.
    {
        tuple<int &> t(i);
        static_assert(std::is_same_v<decltype(first(t)), int &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int &>);
    }
    {
        tuple<int &> const t(i);
        static_assert(std::is_same_v<decltype(first(t)), int &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int &>);
    }
    {
        // view() does not accept temporaries.
        static_assert(!range::has<range::callable::view(tuple<int &>)>::value);
        tuple<int &> const t(i);
        static_assert(std::is_same_v<decltype(first(view(t))), int &>);
        static_assert(std::is_same_v<decltype(at_c<0>(view(t))), int &>);
        static_assert(std::is_same_v<decltype(at(view(t), zero)), int &>);
    }
    {
        static_assert(
            std::is_same_v<decltype(first(view_once(tuple<int &>(i)))), int &>);
        static_assert(std::is_same_v<
                      decltype(at_c<0>(view_once(tuple<int &>(i)))), int &>);
        static_assert(std::is_same_v<
                      decltype(at(view_once(tuple<int &>(i)), zero)), int &>);
    }
    {
        static_assert(std::is_same_v<decltype(first(tuple<int &>(i))), int &>);
        static_assert(
            std::is_same_v<decltype(at_c<0>(tuple<int &>(i))), int &>);
        static_assert(
            std::is_same_v<decltype(at(tuple<int &>(i), zero)), int &>);
    }

    // The same on a const reference contained type.
    {
        tuple<int const &> t(i);
        static_assert(std::is_same_v<decltype(first(t)), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int const &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int const &>);
    }
    {
        tuple<int const &> const t(i);
        static_assert(std::is_same_v<decltype(first(t)), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(t)), int const &>);
        static_assert(std::is_same_v<decltype(at(t, zero)), int const &>);
    }
    {
        // view() does not accept temporaries.
        static_assert(
            !range::has<range::callable::view(tuple<int const &>)>::value);
        tuple<int const &> const t(i);
        static_assert(std::is_same_v<decltype(first(view(t))), int const &>);
        static_assert(std::is_same_v<decltype(at_c<0>(view(t))), int const &>);
        static_assert(std::is_same_v<decltype(at(view(t), zero)), int const &>);
    }
    {
        static_assert(std::is_same_v<
                      decltype(first(view_once(tuple<int const &>(i)))),
                      int const &>);
        static_assert(std::is_same_v<
                      decltype(at_c<0>(view_once(tuple<int const &>(i)))),
                      int const &>);
        static_assert(std::is_same_v<
                      decltype(at(view_once(tuple<int const &>(i)), zero)),
                      int const &>);
    }
    {
        static_assert(std::is_same_v<
                      decltype(first(tuple<int const &>(i))), int const &>);
        static_assert(std::is_same_v<
                      decltype(at_c<0>(tuple<int const &>(i))), int const &>);
        static_assert(std::is_same_v<
                      decltype(at(tuple<int const &>(i), zero)), int const &>);
    }
}

BOOST_AUTO_TEST_SUITE_END()
