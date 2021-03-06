/*
Copyright 2014, 2015 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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
