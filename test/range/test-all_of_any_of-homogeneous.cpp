/*
Copyright 2013, 2015 Rogier van Dalen.

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

/** \file
This tests both range::all and range::any, since they are very similar.
This file is long but its structure is exceedingly simple.

\todo Test half-heterogeneous sequences, with concatenate().
\todo Test that after finding compile-time "false" the remaining elements are
not instantiated, and none are evaluated.
\todo Test that after finding run-time "false" the remaining elements are not
evaluated.
*/

#define BOOST_TEST_MODULE test_range_all_of_any_of
#include "utility/test/boost_unit_test.hpp"

#include "range/all_of.hpp"
#include "range/any_of.hpp"

#include "rime/check/check_equal.hpp"

#include <vector>
#include <tuple>
#include "range/std.hpp"

BOOST_AUTO_TEST_SUITE(test_range_all_of_any_of)

BOOST_AUTO_TEST_CASE (test_range_all_of_any_of_homogeneous) {
    using range::all_of;
    using range::any_of;
    using range::front;
    using range::back;

    {
        std::vector <bool> v;
        RIME_CHECK_EQUAL (all_of (front, v), true);
        RIME_CHECK_EQUAL (all_of (back, v), true);
        RIME_CHECK_EQUAL (all_of (v), true);

        RIME_CHECK_EQUAL (any_of (front, v), false);
        RIME_CHECK_EQUAL (any_of (back, v), false);
        RIME_CHECK_EQUAL (any_of (v), false);
    }

    {
        std::vector <bool> v;
        v.push_back (true);
        RIME_CHECK_EQUAL (all_of (front, v), true);
        RIME_CHECK_EQUAL (all_of (back, v), true);
        RIME_CHECK_EQUAL (all_of (v), true);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

        v.push_back (true);
        RIME_CHECK_EQUAL (all_of (front, v), true);
        RIME_CHECK_EQUAL (all_of (back, v), true);
        RIME_CHECK_EQUAL (all_of (v), true);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

        v.push_back (true);
        RIME_CHECK_EQUAL (all_of (front, v), true);
        RIME_CHECK_EQUAL (all_of (back, v), true);
        RIME_CHECK_EQUAL (all_of (v), true);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

    }

    {
        std::vector <bool> v;
        v.push_back (false);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), false);
        RIME_CHECK_EQUAL (any_of (back, v), false);
        RIME_CHECK_EQUAL (any_of (v), false);

        v.push_back (true);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

        v.push_back (false);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);
    }

    {
        std::vector <bool> v;
        v.push_back (true);
        v.push_back (false);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

        v.push_back (true);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);
    }

    {
        std::vector <bool> v;
        v.push_back (false);
        v.push_back (false);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), false);
        RIME_CHECK_EQUAL (any_of (back, v), false);
        RIME_CHECK_EQUAL (any_of (v), false);
    }
}

BOOST_AUTO_TEST_CASE (test_range_all_of_any_of_homogeneous_constant) {
    using range::all_of;
    using range::any_of;
    using range::front;
    using range::back;

    {
        std::vector <rime::true_type> v;
        RIME_CHECK_EQUAL (all_of (front, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (back, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (v), rime::true_);

        // v is empty, and therefore any_of (v) is false.
        RIME_CHECK_EQUAL (any_of (front, v), false);
        RIME_CHECK_EQUAL (any_of (back, v), false);
        RIME_CHECK_EQUAL (any_of (v), false);

        v.push_back (rime::true_);
        RIME_CHECK_EQUAL (all_of (front, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (back, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (v), rime::true_);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

        v.push_back (rime::true_);
        RIME_CHECK_EQUAL (all_of (front, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (back, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (v), rime::true_);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);
    }

    {
        std::vector <rime::false_type> v;
        // v is empty, and therefore all_of (v) is true.
        RIME_CHECK_EQUAL (all_of (front, v), true);
        RIME_CHECK_EQUAL (all_of (back, v), true);
        RIME_CHECK_EQUAL (all_of (v), true);

        RIME_CHECK_EQUAL (any_of (front, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (back, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (v), rime::false_);

        v.push_back (rime::false_);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (back, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (v), rime::false_);

        v.push_back (rime::false_);
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (back, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (v), rime::false_);
    }

    // std::true_type and std::false_type
    {
        std::vector <std::true_type> v;
        RIME_CHECK_EQUAL (all_of (front, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (back, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (v), rime::true_);

        // v is empty, and therefore any_of (v) is false.
        RIME_CHECK_EQUAL (any_of (front, v), false);
        RIME_CHECK_EQUAL (any_of (back, v), false);
        RIME_CHECK_EQUAL (any_of (v), false);

        v.push_back (std::true_type());
        RIME_CHECK_EQUAL (all_of (front, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (back, v), rime::true_);
        RIME_CHECK_EQUAL (all_of (v), rime::true_);

        RIME_CHECK_EQUAL (any_of (front, v), true);
        RIME_CHECK_EQUAL (any_of (back, v), true);
        RIME_CHECK_EQUAL (any_of (v), true);
    }

    {
        std::vector <std::false_type> v;
        // v is empty, and therefore all_of (v) is true.
        RIME_CHECK_EQUAL (all_of (front, v), true);
        RIME_CHECK_EQUAL (all_of (back, v), true);
        RIME_CHECK_EQUAL (all_of (v), true);

        RIME_CHECK_EQUAL (any_of (front, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (back, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (v), rime::false_);

        v.push_back (std::false_type());
        RIME_CHECK_EQUAL (all_of (front, v), false);
        RIME_CHECK_EQUAL (all_of (back, v), false);
        RIME_CHECK_EQUAL (all_of (v), false);

        RIME_CHECK_EQUAL (any_of (front, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (back, v), rime::false_);
        RIME_CHECK_EQUAL (any_of (v), rime::false_);
    }
}

/**
Spot checks on types that are not bool.
*/
BOOST_AUTO_TEST_CASE (test_range_all_of_any_of_non_bool) {
    using range::all_of;
    using range::any_of;

    {
        std::vector <int> v;
        RIME_CHECK_EQUAL (all_of (v), true);
        RIME_CHECK_EQUAL (any_of (v), false);

        v.push_back (0);
        RIME_CHECK_EQUAL (all_of (v), false);
        RIME_CHECK_EQUAL (any_of (v), false);

        // Note 6 == 0b0110.
        // The last 0 could make an incorrect implementation fall over.
        v.push_back (6);
        RIME_CHECK_EQUAL (all_of (v), false);
        RIME_CHECK_EQUAL (any_of (v), true);
    }

    {
        std::vector <int> v;
        v.push_back (6);
        RIME_CHECK_EQUAL (all_of (v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

        v.push_back (-1);
        RIME_CHECK_EQUAL (all_of (v), true);
        RIME_CHECK_EQUAL (any_of (v), true);

        v.push_back (0);
        RIME_CHECK_EQUAL (all_of (v), false);
        RIME_CHECK_EQUAL (any_of (v), true);
    }

    {
        std::tuple <int, rime::false_type> t (1, rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);
        RIME_CHECK_EQUAL (any_of (t), true);
    }

    {
        std::tuple <int, rime::false_type> t (0, rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);
        RIME_CHECK_EQUAL (any_of (t), false);
    }

    {
        std::tuple <int, rime::true_type> t (0, rime::true_);
        RIME_CHECK_EQUAL (all_of (t), false);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }

    {
        std::tuple <rime::int_<0>> t ((rime::int_<0>()));
        RIME_CHECK_EQUAL (all_of (t), rime::false_);
        RIME_CHECK_EQUAL (any_of (t), rime::false_);
    }
    {
        std::tuple <rime::int_<0>, rime::int_<6>> t (
            (rime::int_<0>()), (rime::int_<6>()));
        RIME_CHECK_EQUAL (all_of (t), rime::false_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }
    {
        std::tuple <rime::int_<9>, rime::int_<6>> t (
            (rime::int_<9>()), (rime::int_<6>()));
        RIME_CHECK_EQUAL (all_of (t), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }

    {
        std::tuple <rime::int_<9>, float> t ((rime::int_<9>()), 0.f);
        RIME_CHECK_EQUAL (all_of (t), false);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
