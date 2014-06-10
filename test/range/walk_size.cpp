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

#define BOOST_TEST_MODULE test_range_walk_size
#include "utility/test/boost_unit_test.hpp"

#include "range/walk_size.hpp"

#include "range/core.hpp"

#include "range/std.hpp"
#include <vector>
#include <list>

BOOST_AUTO_TEST_SUITE(test_range_walk_size)

void check (std::vector <int> const & v) {
    // Sanity.
    BOOST_CHECK_EQUAL (range::size (v), v.size());
    // Test implementation that forwards to size.
    BOOST_CHECK_EQUAL (range::walk_size (v), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::front, v), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::back, v), range::size (v));

    std::list <int> l (v.begin(), v.end());
    // Test slow implementation.
    BOOST_CHECK_EQUAL (range::walk_size (l), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::front, l), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::back, l), range::size (v));
}

BOOST_AUTO_TEST_CASE (test_range_walk_size) {
    using range::size;
    using range::walk_size;

    std::vector <int> v;
    check (v);

    v.push_back (2);
    check (v);

    v.push_back (7);
    check (v);

    v.push_back (27);
    check (v);
}

BOOST_AUTO_TEST_SUITE_END()

