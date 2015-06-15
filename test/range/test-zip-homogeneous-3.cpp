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
    static_assert (range::is_homogeneous <decltype (z), direction::front
        >::value, "");

    RIME_CHECK_EQUAL (empty (z), true);
}

BOOST_AUTO_TEST_CASE (with_weird_count) {
    weird_count w1 (8);
    weird_count w2 (17);
    auto z = zip_from (make_tuple (w1, w2), weird_direction (7));

    auto d = range::default_direction (z);
    static_assert (std::is_same <decltype (d), weird_direction>::value, "");

    BOOST_CHECK_EQUAL (first (first (z)), 8);
    BOOST_CHECK_EQUAL (second (first (z)), 17);

    static_assert (!has <callable::size (decltype (z))>::value, "");

    for (int count = 0; count != 10; ++ count) {
        BOOST_CHECK_EQUAL (first (first (z)), 8 + count);
        BOOST_CHECK_EQUAL (second (first (z)), 17 + count);

        if ((count % 2) == 0)
            z = drop (z, weird_direction (7));
        else
            z = chop (z, weird_direction (7)).rest();
    }
}

BOOST_AUTO_TEST_SUITE_END()
