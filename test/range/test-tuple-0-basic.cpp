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

#define BOOST_TEST_MODULE test_range_tuple_basic
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"

#include <tuple>
#include "range/std/tuple.hpp"

#include "check_equal.hpp"

#include "tuple-basic_tests.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_basic)

BOOST_AUTO_TEST_CASE (tuple_basic) {
    check_empty (tuple<>());
    check_empty (view (tuple<>()));
    check_empty (view_once (tuple<>()));

    check_one_element (tuple <int> (5), 5);
    check_one_element (view (tuple <int> (5)), 5);
    check_one_element (view_once (tuple <int> (5)), 5);

    check_two_elements (tuple <int, char> (5, 'A'), 5, 'A');
    check_two_elements (view (tuple <int, char> (5, 'A')), 5, 'A');
    check_two_elements (view_once (tuple <int, char> (5, 'A')), 5, 'A');

    // Checks on three element is in the other file.
    // Otherwise, compilers start using more than 1GB of memory.
}

BOOST_AUTO_TEST_SUITE_END()
