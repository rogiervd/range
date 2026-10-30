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

/**
Check that handling large lists does not cause stack overflows.
Call this with "--full" on the command line (and without Valgrind, probably) to
run the test with enough elements to cause stack overflows.
*/

#define BOOST_TEST_MODULE test_range_shared_list_large
#include "utility/test/boost_unit_test.hpp"
#include "utility/test/boost_unit_test_command_line.hpp"

#include "container/shared_list.hpp"

#include <vector>
#include "range/std/container.hpp"

#include "range/for_each_macro.hpp"
#include "range/count.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_range_shared_list_large)

typedef range::shared_list <int> shared_list;

/**
Check that handling large lists does not cause stack overflows.

This can happen in various scenarios.
Destruction is one.
Construction from a large (homogeneous) container is another.
*/
BOOST_AUTO_TEST_CASE (test_range_shared_list_construct_large) {
    std::size_t size = command_line_contains ("--full") ? 1000000 : 1000;
    std::vector <int> v (size, -1);
    BOOST_CHECK_EQUAL (v.size(), size);

    // Construct a large shared_list.
    {
        shared_list l (v);
        BOOST_CHECK_EQUAL (l.first(), -1);

        // Destruct it.
    }

    // Construct a large shared_list that actually has a shared tail.
    {
        shared_list l2;
        {
            shared_list l1 (v);
            BOOST_CHECK_EQUAL (l1.first(), -1);

            l2 = l1;
            // for (std::size_t i = 0; i != size; ++ i)
            RANGE_FOR_EACH (i, range::count (size))
                l2.push (range::front, int (i));

            // Destruct l1.
        }
        BOOST_CHECK_EQUAL (l2.first(), size-1);
        // Destruct l2.
    }

    // Use push_range() from a range with many elements.
    {
        shared_list l;
        l.push (7);
        l.push (318);
        l.push_range (v);
    }
}

BOOST_AUTO_TEST_SUITE_END()
