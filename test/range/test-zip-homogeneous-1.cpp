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

BOOST_AUTO_TEST_CASE (example) {
    std::vector <int> vi;
    vi.push_back (2);
    vi.push_back (7);

    std::vector <std::string> vs;
    vs.push_back ("Hello");
    vs.push_back ("there!");
    vs.push_back ("Never change");

    std::size_t count = 0;

    auto z = zip (vi, vs);
    RANGE_FOR_EACH (pair, z) {
        if (first (pair) == 2) {
            BOOST_CHECK_EQUAL (second (pair), "Hello");
        } else {
            BOOST_CHECK_EQUAL (first (pair), 7);
            BOOST_CHECK_EQUAL (second (pair), "there!");
        }

        // Set the first element to the length of the second element.
        first (pair) = size (second (pair));

        ++ count;
    }

    BOOST_CHECK_EQUAL (count, 2);

    BOOST_CHECK_EQUAL (first (vi), 5);
    BOOST_CHECK_EQUAL (second (vi), 6);

    // Mutate the whole pair, and thereby the underlying two vectors, at once.
    RANGE_FOR_EACH (pair, z)
        pair = make_tuple (77, "Something else");

    BOOST_CHECK_EQUAL (first (vi), 77);
    BOOST_CHECK_EQUAL (second (vi), 77);

    BOOST_CHECK_EQUAL (first (vs), "Something else");
    BOOST_CHECK_EQUAL (second (vs), "Something else");

    // The third element of vs is never seen since vi only has two elements.
    BOOST_CHECK_EQUAL (third (vs), "Never change");
}

BOOST_AUTO_TEST_SUITE_END()
