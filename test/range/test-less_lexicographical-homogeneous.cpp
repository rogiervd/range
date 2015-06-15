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

#define BOOST_TEST_MODULE range_test_less_lexicographical_homogeneous
#include "utility/test/boost_unit_test.hpp"

#include "range/less_lexicographical.hpp"

#include <functional>
#include <vector>
#include <tuple>
#include <string>

#include "range/std.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"

#include "rime/check/check_equal.hpp"

struct negate { int operator() (int i) const { return -i; } };

BOOST_AUTO_TEST_SUITE(range_less_lexicographical_homogeneous)

#define CHECK_range_less_lexicographical(r1, r2, value) \
    RIME_CHECK_EQUAL (range::less_lexicographical (r1, r2), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        r1, r2, range::front), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::reverse (r1), range::reverse (r2), range::back), value); \
    \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        r1, r2, std::less <int>()), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        r1, r2, range::front, std::less <int>()), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::reverse (r1), range::reverse (r2), \
        range::back, std::less <int>()), value); \
    \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::transform (r1, negate()), range::transform (r2, negate()), \
        std::greater <int>()), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::transform (r1, negate()), \
        range::transform (r2, negate()), \
        range::front, std::greater <int>()), value); \
    RIME_CHECK_EQUAL (range::less_lexicographical ( \
        range::transform (range::reverse (r1), negate()), \
        range::transform (range::reverse (r2), negate()), \
        range::back, std::greater <int>()), value)

BOOST_AUTO_TEST_CASE (test_range_less_lexicographical_homogeneous) {
    std::vector <int> v1, v2;

    CHECK_range_less_lexicographical (v1, v2, false);

    v1.push_back (1);
    CHECK_range_less_lexicographical (v1, v2, false);
    CHECK_range_less_lexicographical (v2, v1, true);

    v2.push_back (2);
    CHECK_range_less_lexicographical (v1, v2, true);
    CHECK_range_less_lexicographical (v2, v1, false);

    v2.pop_back();
    v2.push_back (1);
    CHECK_range_less_lexicographical (v1, v2, false);

    v2.push_back (3);
    CHECK_range_less_lexicographical (v1, v2, true);
    CHECK_range_less_lexicographical (v2, v1, false);

    v1.push_back (4);
    CHECK_range_less_lexicographical (v1, v2, false);
    CHECK_range_less_lexicographical (v2, v1, true);

    v1.pop_back();
    v1.push_back(3);
    CHECK_range_less_lexicographical (v1, v2, false);
}

BOOST_AUTO_TEST_SUITE_END()
