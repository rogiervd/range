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

#define BOOST_TEST_MODULE test_range_shared_list
#include "utility/test/boost_unit_test.hpp"

#include "container/shared_list.hpp"

#include "boost/mpl/assert.hpp"

// #include "utility/test/tracked.hpp"

#include <vector>
#include "range/std/container.hpp"
#include "range/tuple.hpp"
#include "range/function_range.hpp"
#include "range/take.hpp"

using range::view;

using range::empty;
using range::first;
using range::drop;

BOOST_AUTO_TEST_SUITE(test_suite_range_shared_list)

typedef range::shared_list <int> shared_list;

int count() {
    static int c = 0;
    return c++;
}

BOOST_AUTO_TEST_CASE (test_range_shared_list_construct) {
    shared_list l1;
    BOOST_CHECK (l1.empty());

    {
        shared_list copy (l1);
        BOOST_CHECK (copy.empty());
    }
    {
        shared_list copy (std::move (l1));
        BOOST_CHECK (copy.empty());
        BOOST_CHECK (l1.empty());
    }
    {
        shared_list copy (view (l1));
        BOOST_CHECK (copy.empty());
    }

    l1.push (range::front, 345);
    {
        shared_list copy (l1);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 345);
        BOOST_CHECK (range::empty (drop (copy)));
        // The copy and l1 should be sharing their structure.
        BOOST_CHECK (&first (copy) == &first (l1));
    }
    {
        shared_list copy (view (l1));
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 345);
        BOOST_CHECK (range::empty (drop (copy)));
        // The copy and l1 should be sharing their structure.
        BOOST_CHECK (&first (copy) == &first (l1));
    }
    l1.push (range::front, 789);
    {
        shared_list copy (l1);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 789);
        BOOST_CHECK_EQUAL (first (drop (copy)), 345);
        BOOST_CHECK (range::empty (drop (drop (copy))));
        // The copy and l1 should be sharing their structure.
        BOOST_CHECK (&first (copy) == &first (l1));
    }
    {
        shared_list copy (view (l1));
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 789);
        BOOST_CHECK_EQUAL (first (drop (copy)), 345);
        BOOST_CHECK (range::empty (drop (drop (copy))));
        // The copy and l1 should be sharing their structure.
        BOOST_CHECK (&first (copy) == &first (l1));
    }
    {
        shared_list copy (std::move (l1));
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 789);
        BOOST_CHECK_EQUAL (first (drop (copy)), 345);
        BOOST_CHECK (range::empty (drop (drop (copy))));
    }
    // l1 has been pilfered.
    BOOST_CHECK (range::empty (l1));

    // Construct from other range: forward and backward.
    std::vector <int> v;
    {
        shared_list copy (v);
        BOOST_CHECK (copy.empty());
    }
    {
        // With direction "back".
        range::shared_list <int, direction::back> copy (v);
        BOOST_CHECK (copy.empty());
    }
    v.push_back (9);
    {
        shared_list copy (v);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 9);
        BOOST_CHECK (range::empty (drop (copy)));
    }
    {
        // With direction "back".
        range::shared_list <int, direction::back> copy (v);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 9);
        BOOST_CHECK (range::empty (drop (copy)));
    }
    v.push_back (27);
    {
        shared_list copy (v);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 9);
        BOOST_CHECK_EQUAL (first (drop (copy)), 27);
        BOOST_CHECK (range::empty (drop (drop (copy))));
    }
    {
        // With direction "back".
        range::shared_list <int, direction::back> copy (v);
        BOOST_CHECK (!copy.empty());
        // Here, the order of the elements is reversed.
        BOOST_CHECK_EQUAL (first (copy), 27);
        BOOST_CHECK_EQUAL (first (drop (copy)), 9);
        BOOST_CHECK (range::empty (drop (drop (copy))));
    }
    // \todo Test (using tracked) that the elements get moved if the range is
    // moved.

    // Construct from heterogeneous containers: forward and backward.
    {
        range::tuple <> t;
        shared_list copy (t);
        BOOST_CHECK (copy.empty());
    }
    {
        range::tuple <> t;
        // With direction "back".
        range::shared_list <int, direction::back> copy (t);
        BOOST_CHECK (copy.empty());
    }
    {
        range::tuple <int> t (9);
        shared_list copy (t);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 9);
        BOOST_CHECK (range::empty (drop (copy)));
    }
    {
        // With direction "back".
        range::tuple <int> t (9);
        range::shared_list <int, direction::back> copy (t);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 9);
        BOOST_CHECK (range::empty (drop (copy)));
    }
    {
        range::tuple <int, short> t (9, 27);
        shared_list copy (t);
        BOOST_CHECK (!copy.empty());
        BOOST_CHECK_EQUAL (first (copy), 9);
        BOOST_CHECK_EQUAL (first (drop (copy)), 27);
        BOOST_CHECK (range::empty (drop (drop (copy))));
    }
    {
        // With direction "back".
        range::tuple <int, short> t (9, 27);
        range::shared_list <int, direction::back> copy (t);
        BOOST_CHECK (!copy.empty());
        // Here, the order of the elements is reversed.
        BOOST_CHECK_EQUAL (first (copy), 27);
        BOOST_CHECK_EQUAL (first (drop (copy)), 9);
        BOOST_CHECK (range::empty (drop (drop (copy))));
    }

    // Construct with function_range, which has no "first" and "drop", as input.
    {
        shared_list copy (range::take (3, range::make_function_range (&count)));
        BOOST_CHECK_EQUAL (first (copy), 0);
        BOOST_CHECK_EQUAL (first (drop (copy)), 1);
        BOOST_CHECK_EQUAL (first (drop (drop (copy))), 2);
        BOOST_CHECK (range::empty (
            drop (drop (drop (copy)))));
    }
}

BOOST_AUTO_TEST_CASE (test_range_shared_list) {
    shared_list l;
    BOOST_CHECK (l.empty());

    l.push (4);
    BOOST_CHECK (!l.empty());
    BOOST_CHECK_EQUAL (l.first(), 4);

    l.push (5);
    BOOST_CHECK_EQUAL (l.first(), 5);

    auto l2 = l.tail();
    BOOST_CHECK (!l2.empty());
    BOOST_CHECK_EQUAL (l2.first(), 4);

    l2 = l2.tail();
    BOOST_CHECK (l2.empty());
}

BOOST_AUTO_TEST_CASE (test_range_shared_list_view) {
    shared_list l;
    l.push (4);
    l.push (range::front, 3);
    l.push (2);

    BOOST_MPL_ASSERT ((range::is_range <shared_list>));

    auto v = range::view (range::front, l);
    BOOST_MPL_ASSERT ((range::is_homogeneous <decltype (v)>));

    {
        // view_once does exactly the same as view, because the list may be
        // shared.
        auto v2 = range::view_once (std::move (l));
        BOOST_MPL_ASSERT ((std::is_same <decltype (v), decltype (v2)>));
        BOOST_CHECK (!range::empty (v2));
    }

    BOOST_CHECK (!range::empty (v));
    BOOST_CHECK (!range::empty (l));

    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of <range::callable::first (decltype (v))>::type,
        int const &>));

    BOOST_CHECK_EQUAL (first (v), 2);
    BOOST_CHECK_EQUAL (first (l), 2);

    BOOST_CHECK_EQUAL (first (drop (v)), 3);
    BOOST_CHECK_EQUAL (first (drop (l)), 3);

    BOOST_CHECK_EQUAL (first (drop (drop (v))), 4);
    BOOST_CHECK_EQUAL (first (drop (drop (l))), 4);

    // This should work with std::move (l).
    int sum = range::fold (std::plus <int>(), 0, std::move (l));
    BOOST_CHECK_EQUAL (sum, 2+3+4);
}

BOOST_AUTO_TEST_SUITE_END()
