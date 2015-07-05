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

#define BOOST_TEST_MODULE test_range_subrange
#include "utility/test/boost_unit_test.hpp"

#include "range/subrange.hpp"

#include <vector>

#include "range/count.hpp"
#include "range/for_each_macro.hpp"
#include "range/std/container.hpp"

using range::subrange;
using range::count;

using range::empty;
using range::first;
using range::drop;
using range::size;

using range::front;
using range::back;

template <class Type> struct show_type;

BOOST_AUTO_TEST_SUITE(test_range_subrange)

template <class Count> void check_with_count (Count const & original) {
    {
        auto sub = subrange (original);
        BOOST_CHECK (!empty (sub));
        BOOST_CHECK (!empty (front, sub));
        BOOST_CHECK (!empty (back, sub));

        BOOST_CHECK_EQUAL (size (sub), 10);
        BOOST_CHECK_EQUAL (size (front, sub), 10);
        BOOST_CHECK_EQUAL (size (back, sub), 10);

        BOOST_CHECK_EQUAL (first (sub), 0);
        BOOST_CHECK_EQUAL (first (front, sub), 0);
        BOOST_CHECK_EQUAL (first (back, sub), 9);

        auto sub2 = drop (3, sub);
        BOOST_CHECK_EQUAL (first (sub2), 3);

        {
            auto first_half = sub2.before();
            BOOST_CHECK_EQUAL (first (first_half), 0);
            BOOST_CHECK_EQUAL (first (back, first_half), 2);

            auto second_half = sub2.underlying();
            BOOST_CHECK_EQUAL (first (second_half), 3);
            BOOST_CHECK_EQUAL (first (back, second_half), 9);
        }

        auto sub3 = drop (back, 3, sub2);
        {
            BOOST_CHECK_EQUAL (first (sub3), 3);
            BOOST_CHECK_EQUAL (first (back, sub3), 6);

            auto first_half = sub3.before();
            BOOST_CHECK_EQUAL (first (first_half), 0);
            BOOST_CHECK_EQUAL (first (back, first_half), 2);

            auto second_half = sub3.underlying();
            BOOST_CHECK_EQUAL (first (second_half), 3);
            BOOST_CHECK_EQUAL (first (back, second_half), 6);
        }

        auto sub4 = subrange (back, sub);

        {
            auto middle = drop (front, 4, drop (back, 2, sub4));
            BOOST_CHECK_EQUAL (first (middle), 4);
            BOOST_CHECK_EQUAL (first (back, middle), 7);

            // show_type <decltype (middle)> s;

            auto before = middle.underlying().before();
            BOOST_CHECK_EQUAL (first (before), 0);
            BOOST_CHECK_EQUAL (first (back, before), 3);

            auto after = middle.before();
            BOOST_CHECK_EQUAL (first (after), 8);
            BOOST_CHECK_EQUAL (first (back, after), 9);
        }
    }
    {
        auto sub = subrange (subrange (back, original));
        auto middle =  drop (front, 4, drop (back, 2, sub));
        BOOST_CHECK_EQUAL (first (middle), 4);
        BOOST_CHECK_EQUAL (first (back, middle), 7);

        // show_type <decltype (middle)> s;

        auto before = middle.before();
        BOOST_CHECK_EQUAL (first (before), 0);
        BOOST_CHECK_EQUAL (first (back, before), 3);

        auto after = middle.underlying().before();
        BOOST_CHECK_EQUAL (first (after), 8);
        BOOST_CHECK_EQUAL (first (back, after), 9);
    }
    // \todo Nested subranges in various directions.
    {
        
    }
}

BOOST_AUTO_TEST_CASE (test_range_subrange_basic) {
    check_with_count (count (10));

    std::vector <int> v;
    RANGE_FOR_EACH (i, count (10))
        v.push_back (i);
    check_with_count (v);
}

BOOST_AUTO_TEST_SUITE_END()
