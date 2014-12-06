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

#define BOOST_TEST_MODULE test_range_all_any_heterogeneous
#include "utility/test/boost_unit_test.hpp"

#include "range/all.hpp"
#include "range/any.hpp"

#include "check_equal.hpp"

#include <vector>
#include <tuple>
#include "range/std.hpp"

BOOST_AUTO_TEST_SUITE(test_range_all_any_heterogeneous)

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

BOOST_AUTO_TEST_SUITE_END()
