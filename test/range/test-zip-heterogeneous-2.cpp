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

#define BOOST_TEST_MODULE test_range_zip_heterogeneous
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

BOOST_AUTO_TEST_CASE (homogeneous_and_heterogeneous) {
    std::vector <int> v;
    {
        std::tuple<> t;
        auto z = zip (v, t);
        RIME_CHECK_EQUAL (empty (z), rime::true_);
        // In theory this could be known to be 0 at compile time.
        // It is not currently, and it is not clear that that is overly
        // valuable anyway.
        BOOST_CHECK_EQUAL (size (z), 0);
    }
    {
        std::tuple <int> t;
        auto z = zip (v, t);
        RIME_CHECK_EQUAL (empty (z), true);
        RIME_CHECK_EQUAL (size (z), std::size_t (0));
    }

    v.push_back (5);
    {
        std::tuple <double> t (4.5);
        auto z = zip (v, t);
        BOOST_CHECK (!empty (z));
        BOOST_CHECK_EQUAL (size (z), 1);
        RIME_CHECK_EQUAL (first (first (z)), 5);
        RIME_CHECK_EQUAL (second (first (z)), 4.5);
        // It is known at compile-time that t is empty.
        RIME_CHECK_EQUAL (empty (drop (z)), rime::true_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
