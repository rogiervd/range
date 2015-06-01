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

template <class ...> struct show_types;

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
