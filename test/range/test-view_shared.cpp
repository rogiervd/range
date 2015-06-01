/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_view_shared
#include "utility/test/boost_unit_test.hpp"

#include "range/view_shared.hpp"

#include <list>

#include "range/std/container.hpp"
#include "range/reverse.hpp"
#include "range/transform.hpp"
#include "range/curry.hpp"

#include "unique_range.hpp"

static auto const curried_transform =
    range::callable::curried <range::callable::transform>();

using range::back;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

using range::second;
using range::third;

using range::has;
namespace callable = range::callable;

BOOST_AUTO_TEST_SUITE(test_range_view_shared)

/* Plain range, no transformation function. */

inline range::view_of_shared <std::list <int>> return_4_7_10() {
    std::list <int> l;
    l.push_back (4);
    l.push_back (7);
    l.push_back (10);

    return range::view_shared (std::move (l));
}

BOOST_AUTO_TEST_CASE (test_range_view_shared) {
    auto v = return_4_7_10();

    static_assert (!has <callable::size (decltype (v))>::value,
        "Operation 'size' not available when underlying is a list.");

    BOOST_CHECK_EQUAL (first (v), 4);
    BOOST_CHECK_EQUAL (chop_in_place (v), 4);
    BOOST_CHECK_EQUAL (chop_in_place (v), 7);
    BOOST_CHECK_EQUAL (chop_in_place (v), 10);
    BOOST_CHECK (range::empty (v));
}

/* shared_ptr range, no transformation function. */

inline range::view_of_shared <std::vector <int>> return_2_6_3() {
    auto l = std::make_shared <std::vector <int>>();
    l->push_back (2);
    l->push_back (6);
    l->push_back (3);

    return range::view_shared (std::move (l));
}

BOOST_AUTO_TEST_CASE (test_range_view_shared_2) {
    auto v = return_2_6_3();

    BOOST_CHECK_EQUAL (size (v), 3);
    BOOST_CHECK_EQUAL (first (v), 2);
    v = drop (v);
    BOOST_CHECK_EQUAL (size (v), 2);
    BOOST_CHECK_EQUAL (chop_in_place (v), 6);
    BOOST_CHECK_EQUAL (chop_in_place (v), 3);
    BOOST_CHECK (empty (v));
}

/* shared_ptr range, transformation function. */

inline
typename std::result_of <range::callable::view_shared (
        std::list <int>, range::callable::reverse)>::type
    return_4_7_10_11()
{
    std::list <int> l;
    l.push_back (11);
    l.push_back (10);
    l.push_back (7);
    l.push_back (4);

    return range::view_shared (
        std::make_shared <std::list <int>> (std::move (l)), range::reverse);
}

BOOST_AUTO_TEST_CASE (test_range_view_shared_reverse) {
    {
        auto v = return_4_7_10_11();

        BOOST_CHECK_EQUAL (first (v), 4);
        BOOST_CHECK_EQUAL (chop_in_place (v), 4);
        BOOST_CHECK_EQUAL (chop_in_place (v), 7);
        BOOST_CHECK_EQUAL (chop_in_place (v), 10);
        BOOST_CHECK_EQUAL (chop_in_place (v), 11);
        BOOST_CHECK (range::empty (v));
    }
    {
        auto v = return_4_7_10_11();

        BOOST_CHECK_EQUAL (first (back, v), 11);
        BOOST_CHECK_EQUAL (second (back, v), 10);
        BOOST_CHECK (empty (
            drop (back, drop (back, drop (back, drop (back, v))))));
    }
}

/* Plain range, transformation function. */

std::list <int> get_7_10_55() {
    std::list <int> l;
    l.push_back (7);
    l.push_back (10);
    l.push_back (55);
    return l;
}

struct add_one {
    int operator() (int i) const { return i + 1; }
};

inline auto return_8_11_56()
RETURNS (range::view_shared (get_7_10_55(), curried_transform (add_one())));

BOOST_AUTO_TEST_CASE (test_range_view_shared_transform) {
    auto v = return_8_11_56();

    BOOST_CHECK_EQUAL (first (v), 8);
    BOOST_CHECK_EQUAL (chop_in_place (v), 8);
    BOOST_CHECK_EQUAL (chop_in_place (v), 11);
    BOOST_CHECK_EQUAL (chop_in_place (v), 56);
    BOOST_CHECK (range::empty (v));
}

std::vector <int> get_5_16_18() {
    std::vector <int> v;
    v.push_back (5);
    v.push_back (16);
    v.push_back (18);
    return v;
}

struct make_one_time_view {
    template <class Range> auto operator() (Range && range) const
    RETURNS (one_time_view (std::forward <Range> (range)));
};

auto return_5_16_18_one_time()
RETURNS (range::view_shared (get_5_16_18(), make_one_time_view()));

BOOST_AUTO_TEST_CASE (test_range_view_shared_unique) {
    auto v = return_5_16_18_one_time();

    static_assert (!has <callable::drop (decltype (v) const &)>::value, "");
    static_assert (!has <callable::chop (decltype (v) const &)>::value, "");

    auto chopped1 = chop (std::move (v));
    BOOST_CHECK_EQUAL (chopped1.first(), 5);
    auto chopped2 = chop (chopped1.move_rest());
    BOOST_CHECK_EQUAL (chopped2.first(), 16);
    auto chopped3 = chop (chopped2.move_rest());
    BOOST_CHECK_EQUAL (chopped3.first(), 18);
    BOOST_CHECK (empty (chopped3.rest()));
}

auto return_2_a_hello()
RETURNS (range::view_shared (
    range::make_tuple (2, 'a', std::string ("hello"))));

BOOST_AUTO_TEST_CASE (test_range_view_shared_heterogeneous) {
    auto v = return_2_a_hello();

    BOOST_CHECK_EQUAL (first (v), 2);
    BOOST_CHECK_EQUAL (second (v), 'a');
    BOOST_CHECK_EQUAL (third (v), "hello");
}

BOOST_AUTO_TEST_SUITE_END()
