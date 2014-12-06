/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_tuple_basic_2
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"

#include <tuple>
#include "range/std/tuple.hpp"

#include "check_equal.hpp"

#include "tuple-basic_tests.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_basic_2)

BOOST_AUTO_TEST_CASE (tuple_basic) {
    check_three_elements (tuple <int, char, double> (5, 'A', 7.25),
        5, 'A', 7.25);
    check_three_elements (view (tuple <int, char, double> (5, 'A', 7.25)),
        5, 'A', 7.25);
    check_three_elements (view_once (tuple <int, char, double> (5, 'A', 7.25)),
        5, 'A', 7.25);
}

// The same type twice.
// This could conceivably confuse implementations because of how extractors
// work.
BOOST_AUTO_TEST_CASE (tuple_multiple_elements) {
    using range::first;
    using range::drop;
    using range::back;
    {
        tuple <int, float, float, int> t (4, 5.5f, 6.75f, 7);

        RIME_CHECK_EQUAL (first (t), 4);
        RIME_CHECK_EQUAL (first (drop (t)), 5.5f);
        RIME_CHECK_EQUAL (first (drop (drop (t))), 6.75f);
        RIME_CHECK_EQUAL (first (back, t), 7);
    }
    {
        std::tuple <int, int> source (6, 7);
        tuple <int, int> t (source);
        BOOST_CHECK_EQUAL (first (t), 6);
        BOOST_CHECK_EQUAL (first (back, t), 7);
    }
}

double convert_to_double (int i) { return double (i); }
double convert_to_half_double (int i) { return double (i) / 2; }

BOOST_AUTO_TEST_CASE (tuple_funny_types) {
    // Arrays.
    {
        int a [3] = {7, 77, 777};
        tuple <int [3]> sevens (a);
        BOOST_CHECK_EQUAL (first (sevens) [0], 7);
        BOOST_CHECK_EQUAL (first (sevens) [1], 77);
        BOOST_CHECK_EQUAL (first (sevens) [2], 777);

        tuple <int [3]> copy (sevens);
        BOOST_CHECK_EQUAL (first (copy) [0], 7);
        BOOST_CHECK_EQUAL (first (copy) [1], 77);
        BOOST_CHECK_EQUAL (first (copy) [2], 777);

        tuple <int (&) [3]> reference (sevens);
        BOOST_CHECK_EQUAL (first (reference) [0], 7);
        BOOST_CHECK_EQUAL (first (reference) [1], 77);
        BOOST_CHECK_EQUAL (first (reference) [2], 777);

        first (reference) [1] = 55;
        BOOST_CHECK_EQUAL (first (sevens) [1], 55);

        int b [3] = {4, 44, 444};
        tuple <int [3]> fours (b);
        BOOST_CHECK_EQUAL (first (fours) [0], 4);
        BOOST_CHECK_EQUAL (first (fours) [1], 44);
        BOOST_CHECK_EQUAL (first (fours) [2], 444);

        BOOST_MPL_ASSERT ((utility::is_assignable <
            tuple <int> &, tuple <int> &>));
        // GCC 4.6 does not agree so never mind.
        // BOOST_MPL_ASSERT_NOT ((utility::is_assignable <
        //     tuple <int [3]> &, tuple <int [3]> &>));
    }

    // Function.
    {
        tuple <double (int)> function (convert_to_double);
        RIME_CHECK_EQUAL (first (function) (5), 5.0);

        tuple <double (int)> function2 (convert_to_half_double);
        RIME_CHECK_EQUAL (first (function2) (5), 2.5);

        tuple <double (int)> copy (function2);
        RIME_CHECK_EQUAL (first (copy) (5), 2.5);
    }
    // Function reference.
    {
        tuple <double (&) (int)> function (convert_to_double);
        RIME_CHECK_EQUAL (first (function) (5), 5.0);

        tuple <double (&) (int)> function2 (convert_to_half_double);
        RIME_CHECK_EQUAL (first (function2) (5), 2.5);

        tuple <double (&) (int)> copy (function2);
        RIME_CHECK_EQUAL (first (copy) (5), 2.5);
    }
    // Function pointer.
    {
        tuple <double (*) (int)> function (convert_to_double);
        RIME_CHECK_EQUAL (first (function) (5), 5.0);

        tuple <double (*) (int)> function2 (convert_to_half_double);
        RIME_CHECK_EQUAL (first (function2) (5), 2.5);

        tuple <double (*) (int)> copy (function2);
        RIME_CHECK_EQUAL (first (copy) (5), 2.5);

        copy = function;
        RIME_CHECK_EQUAL (first (copy) (5), 5.0);
    }
}

BOOST_AUTO_TEST_SUITE_END()
