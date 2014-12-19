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

#define BOOST_TEST_MODULE test_range_zip
#include "utility/test/boost_unit_test.hpp"

#include "range/zip.hpp"

#include <vector>
#include <string>

#include "rime/check/check_equal.hpp"

#include "range/tuple.hpp"
#include "range/std.hpp"
#include "range/for_each_macro.hpp"

#include "weird_direction.hpp"
#include "weird_count.hpp"

#include "unique_range.hpp"

using range::tuple;

template <class ... Types> struct show_types;

using range::zip;
using range::zip_from;
using range::view;

using range::empty;
using range::size;
using range::first;
using range::second;
using range::third;

using range::drop;
using range::chop;
using range::chop_in_place;

using range::make_tuple;

using range::has;
namespace callable = range::callable;

BOOST_AUTO_TEST_SUITE(test_range_zip)

BOOST_AUTO_TEST_CASE (single) {
    std::vector <int> v;
    auto z = zip (v);
    static_assert (range::is_homogeneous <direction::front, decltype (z)
        >::value, "");

    RIME_CHECK_EQUAL (empty (z), true);
}

BOOST_AUTO_TEST_CASE (with_weird_count) {
    weird_count w1 (8);
    weird_count w2 (17);
    auto z = zip (weird_direction (7), w1, w2);

    BOOST_CHECK_EQUAL (first (first (z)), 8);
    BOOST_CHECK_EQUAL (second (first (z)), 17);

    for (int count = 0; count != 10; ++ count) {
        BOOST_CHECK_EQUAL (first (first (z)), 8 + count);
        BOOST_CHECK_EQUAL (second (first (z)), 17 + count);

        if ((count % 2) == 0)
            z = drop (weird_direction (7), z);
        else
            z = chop (weird_direction (7), z).rest();
    }
}

BOOST_AUTO_TEST_SUITE_END()
