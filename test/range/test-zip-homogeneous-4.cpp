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

BOOST_AUTO_TEST_CASE (unique_range) {
    std::vector <int> vi;
    vi.push_back (3);
    vi.push_back (7);

    std::vector <double> vd;
    vd.push_back (3.5);
    vd.push_back (7.5);

    {
        auto z = zip (unique_view (vi), vd);

        RIME_CHECK_EQUAL (first (first (z)), 3);
        RIME_CHECK_EQUAL (second (first (z)), 3.5);

        z = drop (std::move (z));

        RIME_CHECK_EQUAL (first (first (z)), 7);
        RIME_CHECK_EQUAL (second (first (z)), 7.5);

        z = drop (std::move (z));
        BOOST_CHECK (empty (z));
    }
    {
        auto z = zip (one_time_view (vi), vd);

        auto chopped = chop (std::move (z));
        RIME_CHECK_EQUAL (first (chopped.first()), 3);
        RIME_CHECK_EQUAL (second (chopped.first()), 3.5);

        // Can call "first" with rvalue reference.
        auto element = first (chopped.move_rest());
        RIME_CHECK_EQUAL (first (element), 7);
        RIME_CHECK_EQUAL (second (element), 7.5);
    }
}

BOOST_AUTO_TEST_SUITE_END()
