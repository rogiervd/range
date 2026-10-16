// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_tuple_basic_2
#include <boost/test/unit_test.hpp>

#include "range/tuple.hpp"

#include <tuple>
#include "range/std/tuple.hpp"

#include "utility/unique_ptr.hpp"

#include "rime/check/check_equal.hpp"

#include "tuple-basic_tests.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_basic_2)

BOOST_AUTO_TEST_CASE(tuple_basic)
{
    check_three_elements(tuple<int, char, double>(5, 'A', 7.25), 5, 'A', 7.25);
    check_three_elements(
        view(tuple<int, char, double>(5, 'A', 7.25)), 5, 'A', 7.25);
    check_three_elements(
        view_once(tuple<int, char, double>(5, 'A', 7.25)), 5, 'A', 7.25);
}

// The same type twice.
// This could conceivably confuse implementations because of how extractors
// work.
BOOST_AUTO_TEST_CASE(tuple_multiple_elements)
{
    using range::back;
    using range::drop;
    using range::first;
    {
        tuple<int, float, float, int> t(4, 5.5f, 6.75f, 7);

        RIME_CHECK_EQUAL(first(t), 4);
        RIME_CHECK_EQUAL(first(drop(t)), 5.5f);
        RIME_CHECK_EQUAL(first(drop(drop(t))), 6.75f);
        RIME_CHECK_EQUAL(first(t, back), 7);
    }
    {
        std::tuple<int, int> source(6, 7);
        tuple<int, int> t(source);
        BOOST_CHECK_EQUAL(first(t), 6);
        BOOST_CHECK_EQUAL(first(t, back), 7);
    }
}

double convert_to_double(int i) { return double(i); }
double convert_to_half_double(int i) { return double(i) / 2; }

BOOST_AUTO_TEST_CASE(tuple_funny_types)
{
    // Noncopyable.
    {
        tuple<std::unique_ptr<int>> ten(std::make_unique<int>(10));
        BOOST_CHECK_EQUAL(*first(ten), 10);
    }
    // Arrays.
    {
        int a[3] = {7, 77, 777};
        tuple<int[3]> sevens(a);
        BOOST_CHECK_EQUAL(first(sevens)[0], 7);
        BOOST_CHECK_EQUAL(first(sevens)[1], 77);
        BOOST_CHECK_EQUAL(first(sevens)[2], 777);

        tuple<int[3]> copy(sevens);
        BOOST_CHECK_EQUAL(first(copy)[0], 7);
        BOOST_CHECK_EQUAL(first(copy)[1], 77);
        BOOST_CHECK_EQUAL(first(copy)[2], 777);

        tuple<int (&)[3]> reference(sevens);
        BOOST_CHECK_EQUAL(first(reference)[0], 7);
        BOOST_CHECK_EQUAL(first(reference)[1], 77);
        BOOST_CHECK_EQUAL(first(reference)[2], 777);

        first(reference)[1] = 55;
        BOOST_CHECK_EQUAL(first(sevens)[1], 55);

        int b[3] = {4, 44, 444};
        tuple<int[3]> fours(b);
        BOOST_CHECK_EQUAL(first(fours)[0], 4);
        BOOST_CHECK_EQUAL(first(fours)[1], 44);
        BOOST_CHECK_EQUAL(first(fours)[2], 444);

        BOOST_MPL_ASSERT((std::is_assignable<tuple<int> &, tuple<int> &>) );
        BOOST_MPL_ASSERT_NOT(
            (std::is_assignable<tuple<int[3]> &, tuple<int[3]> &>) );
    }

    // Function.
    {
        tuple<double(int)> function(convert_to_double);
        RIME_CHECK_EQUAL(first(function)(5), 5.0);

        tuple<double(int)> function2(convert_to_half_double);
        RIME_CHECK_EQUAL(first(function2)(5), 2.5);

        tuple<double(int)> copy(function2);
        RIME_CHECK_EQUAL(first(copy)(5), 2.5);
    }
    // Function reference.
    {
        tuple<double (&)(int)> function(convert_to_double);
        RIME_CHECK_EQUAL(first(function)(5), 5.0);

        tuple<double (&)(int)> function2(convert_to_half_double);
        RIME_CHECK_EQUAL(first(function2)(5), 2.5);

        tuple<double (&)(int)> copy(function2);
        RIME_CHECK_EQUAL(first(copy)(5), 2.5);
    }
    // Function pointer.
    {
        tuple<double (*)(int)> function(convert_to_double);
        RIME_CHECK_EQUAL(first(function)(5), 5.0);

        tuple<double (*)(int)> function2(convert_to_half_double);
        RIME_CHECK_EQUAL(first(function2)(5), 2.5);

        tuple<double (*)(int)> copy(function2);
        RIME_CHECK_EQUAL(first(copy)(5), 2.5);

        copy = function;
        RIME_CHECK_EQUAL(first(copy)(5), 5.0);
    }
}

BOOST_AUTO_TEST_SUITE_END()
