/*
Copyright 2013 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_walk_size
#include "utility/test/boost_unit_test.hpp"

#include "range/walk_size.hpp"

#include "range/core.hpp"

#include "range/std.hpp"
#include <vector>
#include <list>

BOOST_AUTO_TEST_SUITE(test_range_walk_size)

void check (std::vector <int> const & v) {
    // Sanity.
    BOOST_CHECK_EQUAL (range::size (v), v.size());
    // Test implementation that forwards to size.
    BOOST_CHECK_EQUAL (range::walk_size (v), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::front, v), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::back, v), range::size (v));

    std::list <int> l (v.begin(), v.end());
    // Test slow implementation.
    BOOST_CHECK_EQUAL (range::walk_size (l), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::front, l), range::size (v));
    BOOST_CHECK_EQUAL (range::walk_size (range::back, l), range::size (v));
}

BOOST_AUTO_TEST_CASE (test_range_walk_size) {
    using range::size;
    using range::walk_size;

    std::vector <int> v;
    check (v);

    v.push_back (2);
    check (v);

    v.push_back (7);
    check (v);

    v.push_back (27);
    check (v);
}

BOOST_AUTO_TEST_SUITE_END()

