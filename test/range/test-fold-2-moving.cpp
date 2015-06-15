/*
Copyright 2014, 2015 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#define BOOST_TEST_MODULE test_range_fold_moving
#include "utility/test/boost_unit_test.hpp"

#include "range/fold.hpp"

#include <tuple>
#include <vector>

#include "utility/unique_ptr.hpp"

#include "range/std.hpp"
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

    auto result = fold (0, range::take (std::move (count), 10), add());
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
        // range::helper::get_underlying (view);
        int result = fold (0, std::move (view), add());
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

        int result = fold (0, std::move (view), add());
        BOOST_CHECK_EQUAL (result, 5+6+7);
    }
    {
        std::tuple <int, short, int> v (5, 6, 7);
        // Note: one_time_view.
        auto view = one_time_view (v);

        int result = fold (0, std::move (view), add());
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
            = fold (make_unique <int> (0), v, add_to_unique_ptr());
        BOOST_CHECK_EQUAL (*result, 555);
    }
    {
        std::tuple <int, short, int> v (456, -123, 222);

        std::unique_ptr <int> result
            = fold (make_unique <int> (0), v, add_to_unique_ptr());
        BOOST_CHECK_EQUAL (*result, 555);
    }
    {
        std::tuple <int, short, int> v (456, -123, 222);

        std::unique_ptr <int> result
            = fold (make_unique <int> (0), v, add_to_unique_ptr());
        BOOST_CHECK_EQUAL (*result, 555);
    }
}

BOOST_AUTO_TEST_SUITE_END()
