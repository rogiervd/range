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

#define BOOST_TEST_MODULE range_test_less_lexicographical_mixed
#include "utility/test/boost_unit_test.hpp"

#include "range/less_lexicographical.hpp"

#include <functional>
#include <vector>
#include <tuple>
#include <string>

#include "utility/config.hpp"

#include "range/std.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"

#include "rime/check/check_equal.hpp"

struct negate { int operator() (int i) const { return -i; } };

BOOST_AUTO_TEST_SUITE(range_less_lexicographical_test_suite_mixed)

#define CHECK_range_less_lexicographical(r1, r2, value) \
    RIME_CHECK_EQUAL (range::less_lexicographical (r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::front, r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::back, range::reverse (r1), range::reverse (r2)), value); \
    \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        std::less <int>(), r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::front, std::less <int>(), r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::back, std::less <int>(), \
        range::reverse (r1), range::reverse (r2)), value); \
    \
    RIME_CHECK_EQUAL (range::less_lexicographical (std::greater <int>(), \
        range::transform (negate(), r1), \
        range::transform (negate(), r2)), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::front, std::greater <int>(), \
        range::transform (negate(), r1), \
        range::transform (negate(), r2)), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::back, std::greater <int>(), \
        range::transform (negate(), range::reverse (r1)), \
        range::transform (negate(), range::reverse (r2))), value)

// Mix homogeneous and heterogeneous ranges.
BOOST_AUTO_TEST_CASE (test_range_less_lexicographical_mixed) {
    std::vector <int> v1;

    CHECK_range_less_lexicographical (v1, std::make_tuple(), rime::false_);
    CHECK_range_less_lexicographical (std::make_tuple(), v1, false);
    CHECK_range_less_lexicographical (v1, std::make_tuple (1), true);
    CHECK_range_less_lexicographical (std::make_tuple (1), v1, false);
    CHECK_range_less_lexicographical (v1, std::make_tuple (1, 2), true);
    CHECK_range_less_lexicographical (std::make_tuple (1, 2), v1, false);

    v1.push_back (1);
    // The following works around strange behaviour of CLang with optimisations:
    // It seems to perform a 64-bit comparison instead of 32-bit.
    // Maybe this is somehow correct, but it surprises Valgrind too much.
    v1.push_back (4);
    v1.pop_back();
    // End workaround.

    CHECK_range_less_lexicographical (v1, std::make_tuple (1), false);
    CHECK_range_less_lexicographical (std::make_tuple (1), v1, false);
    CHECK_range_less_lexicographical (v1, std::make_tuple(), rime::false_);
    CHECK_range_less_lexicographical (std::make_tuple(), v1, true);
    CHECK_range_less_lexicographical (v1, std::make_tuple (2), true);
    CHECK_range_less_lexicographical (std::make_tuple (2), v1, false);
    CHECK_range_less_lexicographical (v1, std::make_tuple(1, 3), true);
    CHECK_range_less_lexicographical (std::make_tuple(1, 3), v1, false);

    v1.push_back (4);

    v1.push_back (8);
    v1.pop_back();

    CHECK_range_less_lexicographical (v1, std::make_tuple(1, 4), false);
    CHECK_range_less_lexicographical (std::make_tuple(1, 4), v1, false);
    CHECK_range_less_lexicographical (v1, std::make_tuple(), rime::false_);
    CHECK_range_less_lexicographical (std::make_tuple(), v1, true);
    CHECK_range_less_lexicographical (v1, std::make_tuple(1), false);
    CHECK_range_less_lexicographical (std::make_tuple(1), v1, true);
    CHECK_range_less_lexicographical (v1, std::make_tuple(1, 3), false);
    CHECK_range_less_lexicographical (std::make_tuple(1, 3), v1, true);
    CHECK_range_less_lexicographical (v1, std::make_tuple(1, 4, 5), true);
    CHECK_range_less_lexicographical (std::make_tuple(1, 4, 5), v1, false);
}

BOOST_AUTO_TEST_SUITE_END()
