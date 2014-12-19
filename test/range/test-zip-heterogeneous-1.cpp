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

BOOST_AUTO_TEST_CASE (heterogeneous) {
    {
        std::tuple<> t;
        auto z1 = zip (t);
        RIME_CHECK_EQUAL (empty (z1), rime::true_);
        auto z2 = zip (t, t);
        RIME_CHECK_EQUAL (empty (z2), rime::true_);
        auto z3 = zip (t, t, t);
        RIME_CHECK_EQUAL (empty (z3), rime::true_);

        RIME_CHECK_EQUAL (size (z3), rime::size_t <0>());
    }
    {
        std::tuple <int> ti (4);
        std::tuple <double> td (7.5);
        auto z = zip (ti, td);
        RIME_CHECK_EQUAL (empty (z), rime::false_);
        RIME_CHECK_EQUAL (first (first (z)), 4);
        RIME_CHECK_EQUAL (second (first (z)), 7.5);

        RIME_CHECK_EQUAL (size (z), rime::size_t <1>());

        auto tuple_tuple = std::make_tuple (ti, td);
        auto z2 = zip_from (tuple_tuple);
        static_assert (std::is_same <decltype (z), decltype (z2)>::value, "");
        RIME_CHECK_EQUAL (first (first (z2)), 4);
        RIME_CHECK_EQUAL (second (first (z2)), 7.5);

        auto z3 = zip (ti, unique_view (td));

        auto chopped = chop (std::move (z3));
        RIME_CHECK_EQUAL (first (chopped.first()), 4);
        RIME_CHECK_EQUAL (second (chopped.first()), 7.5);
        RIME_CHECK_EQUAL (empty (chopped.rest()), rime::true_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
