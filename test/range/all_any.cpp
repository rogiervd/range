/*
Copyright 2013 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_all_any
#include "utility/test/boost_unit_test.hpp"

#include "range/all.hpp"
#include "range/any.hpp"

#include "check_equal.hpp"

#include <vector>
#include <tuple>
#include "range/std.hpp"

BOOST_AUTO_TEST_SUITE(test_range_all_any)

BOOST_AUTO_TEST_CASE (test_range_all_any_homogeneous) {
    using range::all;
    using range::any;
    using range::front;
    using range::back;

    {
        std::vector <bool> v;
        RIME_CHECK_EQUAL (all (front, v), true);
        RIME_CHECK_EQUAL (all (back, v), true);
        RIME_CHECK_EQUAL (all (v), true);

        RIME_CHECK_EQUAL (any (front, v), false);
        RIME_CHECK_EQUAL (any (back, v), false);
        RIME_CHECK_EQUAL (any (v), false);
    }

    {
        std::vector <bool> v;
        v.push_back (true);
        RIME_CHECK_EQUAL (all (front, v), true);
        RIME_CHECK_EQUAL (all (back, v), true);
        RIME_CHECK_EQUAL (all (v), true);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);

        v.push_back (true);
        RIME_CHECK_EQUAL (all (front, v), true);
        RIME_CHECK_EQUAL (all (back, v), true);
        RIME_CHECK_EQUAL (all (v), true);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);

        v.push_back (true);
        RIME_CHECK_EQUAL (all (front, v), true);
        RIME_CHECK_EQUAL (all (back, v), true);
        RIME_CHECK_EQUAL (all (v), true);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);

    }

    {
        std::vector <bool> v;
        v.push_back (false);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), false);
        RIME_CHECK_EQUAL (any (back, v), false);
        RIME_CHECK_EQUAL (any (v), false);

        v.push_back (true);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);

        v.push_back (false);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);
    }

    {
        std::vector <bool> v;
        v.push_back (true);
        v.push_back (false);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);

        v.push_back (true);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);
    }

    {
        std::vector <bool> v;
        v.push_back (false);
        v.push_back (false);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), false);
        RIME_CHECK_EQUAL (any (back, v), false);
        RIME_CHECK_EQUAL (any (v), false);
    }
}

BOOST_AUTO_TEST_CASE (test_range_all_any_homogeneous_constant) {
    using range::all;
    using range::any;
    using range::front;
    using range::back;

    {
        std::vector <rime::true_type> v;
        RIME_CHECK_EQUAL (all (front, v), rime::true_);
        RIME_CHECK_EQUAL (all (back, v), rime::true_);
        RIME_CHECK_EQUAL (all (v), rime::true_);

        // v is empty, and therefore any (v) is false.
        RIME_CHECK_EQUAL (any (front, v), false);
        RIME_CHECK_EQUAL (any (back, v), false);
        RIME_CHECK_EQUAL (any (v), false);

        v.push_back (rime::true_);
        RIME_CHECK_EQUAL (all (front, v), rime::true_);
        RIME_CHECK_EQUAL (all (back, v), rime::true_);
        RIME_CHECK_EQUAL (all (v), rime::true_);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);

        v.push_back (rime::true_);
        RIME_CHECK_EQUAL (all (front, v), rime::true_);
        RIME_CHECK_EQUAL (all (back, v), rime::true_);
        RIME_CHECK_EQUAL (all (v), rime::true_);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);
    }

    {
        std::vector <rime::false_type> v;
        // v is empty, and therefore all (v) is true.
        RIME_CHECK_EQUAL (all (front, v), true);
        RIME_CHECK_EQUAL (all (back, v), true);
        RIME_CHECK_EQUAL (all (v), true);

        RIME_CHECK_EQUAL (any (front, v), rime::false_);
        RIME_CHECK_EQUAL (any (back, v), rime::false_);
        RIME_CHECK_EQUAL (any (v), rime::false_);

        v.push_back (rime::false_);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), rime::false_);
        RIME_CHECK_EQUAL (any (back, v), rime::false_);
        RIME_CHECK_EQUAL (any (v), rime::false_);

        v.push_back (rime::false_);
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), rime::false_);
        RIME_CHECK_EQUAL (any (back, v), rime::false_);
        RIME_CHECK_EQUAL (any (v), rime::false_);
    }

    // std::true_type and std::false_type
    {
        std::vector <std::true_type> v;
        RIME_CHECK_EQUAL (all (front, v), rime::true_);
        RIME_CHECK_EQUAL (all (back, v), rime::true_);
        RIME_CHECK_EQUAL (all (v), rime::true_);

        // v is empty, and therefore any (v) is false.
        RIME_CHECK_EQUAL (any (front, v), false);
        RIME_CHECK_EQUAL (any (back, v), false);
        RIME_CHECK_EQUAL (any (v), false);

        v.push_back (std::true_type());
        RIME_CHECK_EQUAL (all (front, v), rime::true_);
        RIME_CHECK_EQUAL (all (back, v), rime::true_);
        RIME_CHECK_EQUAL (all (v), rime::true_);

        RIME_CHECK_EQUAL (any (front, v), true);
        RIME_CHECK_EQUAL (any (back, v), true);
        RIME_CHECK_EQUAL (any (v), true);
    }

    {
        std::vector <std::false_type> v;
        // v is empty, and therefore all (v) is true.
        RIME_CHECK_EQUAL (all (front, v), true);
        RIME_CHECK_EQUAL (all (back, v), true);
        RIME_CHECK_EQUAL (all (v), true);

        RIME_CHECK_EQUAL (any (front, v), rime::false_);
        RIME_CHECK_EQUAL (any (back, v), rime::false_);
        RIME_CHECK_EQUAL (any (v), rime::false_);

        v.push_back (std::false_type());
        RIME_CHECK_EQUAL (all (front, v), false);
        RIME_CHECK_EQUAL (all (back, v), false);
        RIME_CHECK_EQUAL (all (v), false);

        RIME_CHECK_EQUAL (any (front, v), rime::false_);
        RIME_CHECK_EQUAL (any (back, v), rime::false_);
        RIME_CHECK_EQUAL (any (v), rime::false_);
    }
}

BOOST_AUTO_TEST_CASE (test_range_all_any_heterogeneous) {
    using range::all;
    using range::any;
    using range::front;
    using range::back;

    {
        std::tuple <bool> t (false);
        RIME_CHECK_EQUAL (all (front, t), false);
        RIME_CHECK_EQUAL (all (back, t), false);
        RIME_CHECK_EQUAL (all (t), false);

        RIME_CHECK_EQUAL (any (front, t), false);
        RIME_CHECK_EQUAL (any (back, t), false);
        RIME_CHECK_EQUAL (any (t), false);
    }

    {
        std::tuple <bool> t (true);
        RIME_CHECK_EQUAL (all (front, t), true);
        RIME_CHECK_EQUAL (all (back, t), true);
        RIME_CHECK_EQUAL (all (t), true);

        RIME_CHECK_EQUAL (any (front, t), true);
        RIME_CHECK_EQUAL (any (back, t), true);
        RIME_CHECK_EQUAL (any (t), true);
    }

    {
        std::tuple <bool, bool> t (false, false);
        RIME_CHECK_EQUAL (all (front, t), false);
        RIME_CHECK_EQUAL (all (back, t), false);
        RIME_CHECK_EQUAL (all (t), false);

        RIME_CHECK_EQUAL (any (front, t), false);
        RIME_CHECK_EQUAL (any (back, t), false);
        RIME_CHECK_EQUAL (any (t), false);
    }

    {
        std::tuple <bool, bool> t (false, true);
        RIME_CHECK_EQUAL (all (front, t), false);
        RIME_CHECK_EQUAL (all (back, t), false);
        RIME_CHECK_EQUAL (all (t), false);

        RIME_CHECK_EQUAL (any (front, t), true);
        RIME_CHECK_EQUAL (any (back, t), true);
        RIME_CHECK_EQUAL (any (t), true);
    }

    {
        std::tuple <bool, bool> t (true, false);
        RIME_CHECK_EQUAL (all (front, t), false);
        RIME_CHECK_EQUAL (all (back, t), false);
        RIME_CHECK_EQUAL (all (t), false);

        RIME_CHECK_EQUAL (any (front, t), true);
        RIME_CHECK_EQUAL (any (back, t), true);
        RIME_CHECK_EQUAL (any (t), true);
    }

    {
        std::tuple <bool, bool> t (true, true);
        RIME_CHECK_EQUAL (all (front, t), true);
        RIME_CHECK_EQUAL (all (back, t), true);
        RIME_CHECK_EQUAL (all (t), true);

        RIME_CHECK_EQUAL (any (front, t), true);
        RIME_CHECK_EQUAL (any (back, t), true);
        RIME_CHECK_EQUAL (any (t), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_all_any_constant) {
    using range::all;
    using range::any;
    using range::front;
    using range::back;

    {
        std::tuple <> t;
        RIME_CHECK_EQUAL (all (front, t), rime::true_);
        RIME_CHECK_EQUAL (all (back, t), rime::true_);
        RIME_CHECK_EQUAL (all (t), rime::true_);

        RIME_CHECK_EQUAL (any (front, t), rime::false_);
        RIME_CHECK_EQUAL (any (back, t), rime::false_);
        RIME_CHECK_EQUAL (any (t), rime::false_);
    }

    {
        std::tuple <rime::false_type> t (rime::false_);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), rime::false_);
        RIME_CHECK_EQUAL (any (back, t), rime::false_);
        RIME_CHECK_EQUAL (any (t), rime::false_);
    }

    {
        std::tuple <rime::true_type> t (rime::true_);
        RIME_CHECK_EQUAL (all (front, t), rime::true_);
        RIME_CHECK_EQUAL (all (back, t), rime::true_);
        RIME_CHECK_EQUAL (all (t), rime::true_);

        RIME_CHECK_EQUAL (any (front, t), rime::true_);
        RIME_CHECK_EQUAL (any (back, t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }

    {
        std::tuple <rime::false_type, rime::false_type> t (
            rime::false_, rime::false_);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), rime::false_);
        RIME_CHECK_EQUAL (any (back, t), rime::false_);
        RIME_CHECK_EQUAL (any (t), rime::false_);
    }

    {
        std::tuple <rime::false_type, rime::true_type> t (
            rime::false_, rime::true_);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), rime::true_);
        RIME_CHECK_EQUAL (any (back, t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }

    {
        std::tuple <rime::true_type, rime::false_type> t (
            rime::true_, rime::false_);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), rime::true_);
        RIME_CHECK_EQUAL (any (back, t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }

    {
        std::tuple <rime::true_type, rime::true_type> t (
            rime::true_, rime::true_);
        RIME_CHECK_EQUAL (all (front, t), rime::true_);
        RIME_CHECK_EQUAL (all (back, t), rime::true_);
        RIME_CHECK_EQUAL (all (t), rime::true_);

        RIME_CHECK_EQUAL (any (front, t), rime::true_);
        RIME_CHECK_EQUAL (any (back, t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }

    // Mix of constant and run-time.
    {
        std::tuple <rime::false_type, bool> t (rime::false_, false);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), false);
        RIME_CHECK_EQUAL (any (back, t), false);
        RIME_CHECK_EQUAL (any (t), false);
    }
    {
        std::tuple <bool, rime::false_type> t (false, rime::false_);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), false);
        RIME_CHECK_EQUAL (any (back, t), false);
        RIME_CHECK_EQUAL (any (t), false);
    }

    {
        std::tuple <rime::false_type, bool> t (rime::false_, true);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), true);
        RIME_CHECK_EQUAL (any (back, t), true);
        RIME_CHECK_EQUAL (any (t), true);
    }
    {
        std::tuple <bool, rime::false_type> t (true, rime::false_);
        RIME_CHECK_EQUAL (all (front, t), rime::false_);
        RIME_CHECK_EQUAL (all (back, t), rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);

        RIME_CHECK_EQUAL (any (front, t), true);
        RIME_CHECK_EQUAL (any (back, t), true);
        RIME_CHECK_EQUAL (any (t), true);
    }

    {
        std::tuple <rime::true_type, bool> t (rime::true_, false);
        RIME_CHECK_EQUAL (all (front, t), false);
        RIME_CHECK_EQUAL (all (back, t), false);
        RIME_CHECK_EQUAL (all (t), false);

        RIME_CHECK_EQUAL (any (front, t), rime::true_);
        RIME_CHECK_EQUAL (any (back, t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }
    {
        std::tuple <bool, rime::true_type> t (false, rime::true_);
        RIME_CHECK_EQUAL (all (front, t), false);
        RIME_CHECK_EQUAL (all (back, t), false);
        RIME_CHECK_EQUAL (all (t), false);
    }

    {
        std::tuple <rime::true_type, bool> t (rime::true_, true);
        RIME_CHECK_EQUAL (all (front, t), true);
        RIME_CHECK_EQUAL (all (back, t), true);
        RIME_CHECK_EQUAL (all (t), true);

        RIME_CHECK_EQUAL (any (front, t), rime::true_);
        RIME_CHECK_EQUAL (any (back, t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }
    {
        std::tuple <bool, rime::true_type> t (true, rime::true_);
        RIME_CHECK_EQUAL (all (front, t), true);
        RIME_CHECK_EQUAL (all (back, t), true);
        RIME_CHECK_EQUAL (all (t), true);

        RIME_CHECK_EQUAL (any (front, t), rime::true_);
        RIME_CHECK_EQUAL (any (back, t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }
}

/**
Spot checks on types that are not bool.
*/
BOOST_AUTO_TEST_CASE (test_range_all_any_non_bool) {
    using range::all;
    using range::any;

    {
        std::vector <int> v;
        RIME_CHECK_EQUAL (all (v), true);
        RIME_CHECK_EQUAL (any (v), false);

        v.push_back (0);
        RIME_CHECK_EQUAL (all (v), false);
        RIME_CHECK_EQUAL (any (v), false);

        // Note 6 == 0b0110.
        // The last 0 could make an incorrect implementation fall over.
        v.push_back (6);
        RIME_CHECK_EQUAL (all (v), false);
        RIME_CHECK_EQUAL (any (v), true);
    }

    {
        std::vector <int> v;
        v.push_back (6);
        RIME_CHECK_EQUAL (all (v), true);
        RIME_CHECK_EQUAL (any (v), true);

        v.push_back (-1);
        RIME_CHECK_EQUAL (all (v), true);
        RIME_CHECK_EQUAL (any (v), true);

        v.push_back (0);
        RIME_CHECK_EQUAL (all (v), false);
        RIME_CHECK_EQUAL (any (v), true);
    }

    {
        std::tuple <int, rime::false_type> t (1, rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);
        RIME_CHECK_EQUAL (any (t), true);
    }

    {
        std::tuple <int, rime::false_type> t (0, rime::false_);
        RIME_CHECK_EQUAL (all (t), rime::false_);
        RIME_CHECK_EQUAL (any (t), false);
    }

    {
        std::tuple <int, rime::true_type> t (0, rime::true_);
        RIME_CHECK_EQUAL (all (t), false);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }

    {
        std::tuple <rime::int_<0>> t ((rime::int_<0>()));
        RIME_CHECK_EQUAL (all (t), rime::false_);
        RIME_CHECK_EQUAL (any (t), rime::false_);
    }
    {
        std::tuple <rime::int_<0>, rime::int_<6>> t (
            (rime::int_<0>()), (rime::int_<6>()));
        RIME_CHECK_EQUAL (all (t), rime::false_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }
    {
        std::tuple <rime::int_<9>, rime::int_<6>> t (
            (rime::int_<9>()), (rime::int_<6>()));
        RIME_CHECK_EQUAL (all (t), rime::true_);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }

    {
        std::tuple <rime::int_<9>, float> t ((rime::int_<9>()), 0.f);
        RIME_CHECK_EQUAL (all (t), false);
        RIME_CHECK_EQUAL (any (t), rime::true_);
    }
}

BOOST_AUTO_TEST_SUITE_END()

