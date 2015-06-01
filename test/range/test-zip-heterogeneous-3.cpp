/*
Copyright 2014 Rogier van Dalen.

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

    v.push_back (5);
    {
        std::tuple <double, std::string> t (4.5, "");
        auto z = zip (v, t);
        BOOST_CHECK (!empty (z));
        BOOST_CHECK_EQUAL (size (z), 1);
        RIME_CHECK_EQUAL (first (first (z)), 5);
        RIME_CHECK_EQUAL (second (first (z)), 4.5);
        // Only known at run-time that this is empty.
        RIME_CHECK_EQUAL (empty (drop (z)), true);
    }

    v.push_back (7);
    {
        std::tuple <double, std::string> t (27.5, "Test");
        auto z = zip (t, v);

        BOOST_CHECK_EQUAL (size (z), 2);

        RIME_CHECK_EQUAL (first (first (z)), 27.5);
        RIME_CHECK_EQUAL (first (second (z)), std::string ("Test"));
        RIME_CHECK_EQUAL (second (first (z)), 5);
        RIME_CHECK_EQUAL (second (second (z)), 7);

        RIME_CHECK_EQUAL (empty (drop (rime::size_t <2>(), z)), rime::true_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
