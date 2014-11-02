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

#define BOOST_TEST_MODULE test_range_empty_view
#include "utility/test/boost_unit_test.hpp"

#include "range/empty_view.hpp"

BOOST_AUTO_TEST_SUITE(test_range_empty_view)

BOOST_AUTO_TEST_CASE (test_range_empty_view) {
    range::empty_view v;

    typedef decltype (range::empty (v)) is_empty;
    static_assert (is_empty::value, "");

    typedef decltype (range::empty (range::back, v)) is_empty_back;
    static_assert (is_empty_back::value, "");

    typedef decltype (range::size (v)) size;
    static_assert (size::value == 0u, "");

    typedef decltype (range::size (range::back, v)) size_back;
    static_assert (size_back::value == 0u, "");
}

BOOST_AUTO_TEST_SUITE_END()
