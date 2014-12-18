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
