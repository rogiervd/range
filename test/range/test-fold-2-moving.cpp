/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_fold_moving
#include "utility/test/boost_unit_test.hpp"

#include "range/fold.hpp"

#include <tuple>
#include <vector>

#include "utility/make_unique.hpp"

#include "range/std.hpp"
#include "range/tuple.hpp"
#include "range/function_range.hpp"

#include "range/take.hpp"

#include "unique_range.hpp"

using utility::make_unique;
using range::fold;

BOOST_AUTO_TEST_SUITE(test_range_fold_moving)

int get_next() {
    static int current = 0;
    return current ++;
}

struct add {
    int operator() (int a, int b) const { return a + b; }
};

BOOST_AUTO_TEST_CASE (function_range) {
    auto count = range::make_function_range (&get_next);

    auto result = fold (add(), 0, range::take (10, std::move (count)));
    BOOST_CHECK_EQUAL (result, 45);
}

// More structured test than using function_range.
// Use unique_view.
BOOST_AUTO_TEST_CASE (test_fold_moving) {
    {
        std::vector <int> v;
        v.push_back (5);
        v.push_back (6);
        v.push_back (7);
        auto view = unique_view (v);
        // range::detail::get_underlying (view);
        int result = fold (add(), 0, std::move (view));
        BOOST_CHECK_EQUAL (result, 5+6+7);
    }
}

// Use one_time_view.
BOOST_AUTO_TEST_CASE (test_fold_chop) {
    {
        std::vector <int> v;
        v.push_back (5);
        v.push_back (6);
        v.push_back (7);
        // Note: one_time_view.
        auto view = one_time_view (v);

        int result = fold (add(), 0, std::move (view));
        BOOST_CHECK_EQUAL (result, 5+6+7);
    }
    {
        std::tuple <int, short, int> v (5, 6, 7);
        // Note: one_time_view.
        auto view = one_time_view (v);

        int result = fold (add(), 0, std::move (view));
        BOOST_CHECK_EQUAL (result, 5+6+7);
    }
}

/* Test that states are only moved and never copied. */

struct add_to_unique_ptr {
    int value;

    std::unique_ptr <int> operator() (std::unique_ptr <int> && current, int v)
        const
    {
        // Pilfer "current".
        *current += v;
        return std::move (current);
    }
};

BOOST_AUTO_TEST_CASE (test_fold_move_state) {
    {
        std::vector <int> v;
        v.push_back (456);
        v.push_back (-123);
        v.push_back (222);

        std::unique_ptr <int> result
            = fold (add_to_unique_ptr(), make_unique <int> (0), v);
        BOOST_CHECK_EQUAL (*result, 555);
    }
    {
        std::tuple <int, short, int> v (456, -123, 222);

        std::unique_ptr <int> result
            = fold (add_to_unique_ptr(), make_unique <int> (0), v);
        BOOST_CHECK_EQUAL (*result, 555);
    }
    {
        range::tuple <int, short, int> v (456, -123, 222);

        std::unique_ptr <int> result
            = fold (add_to_unique_ptr(), make_unique <int> (0), v);
        BOOST_CHECK_EQUAL (*result, 555);
    }
}

BOOST_AUTO_TEST_SUITE_END()
