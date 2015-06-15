/*
Copyright 2012-2015 Rogier van Dalen.

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

/** \file
This tests some aspects of range/core.hpp.
Other aspects are tested on different types of ranges by
- member_view.cpp
- std_container.cpp
- std_tuple.cpp
- function_range.cpp
*/

#define BOOST_TEST_MODULE test_range_core
#include "utility/test/boost_unit_test.hpp"

#include "range/core.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "weird_direction.hpp"
#include "weird_count.hpp"

struct fake_range_1 {};
struct fake_range_1_tag {};

namespace range {

    template <> struct tag_of_qualified <fake_range_1>
    { typedef fake_range_1_tag type; };

} // namespace range

BOOST_AUTO_TEST_SUITE(test_range_core)

BOOST_AUTO_TEST_CASE (test_range_tag) {
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_1>::type, fake_range_1_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_1 const>::type, fake_range_1_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_1 &>::type, fake_range_1_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_1 const &>::type, fake_range_1_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_1 &&>::type, fake_range_1_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_1 const &&>::type, fake_range_1_tag>));
}

BOOST_AUTO_TEST_CASE (test_range_core) {
    {
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::default_direction (
            int)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (
            double, int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view_once (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view_once (
            int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view_once (
            double, int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::empty (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::empty (
            int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            double, int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            double, int, char)>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::chop_in_place (int)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::chop_in_place (int, char)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::chop_in_place (double, int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (
            double, int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at_c <2> (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at_c <0> (
            int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at_c <1> (
            double, int, char)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::second (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::third (
            int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::seventh (
            double, int, char)>));
    }

    weird_count c;

    // default_direction.
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::default_direction (weird_count)>::type,
        forgotten_to_define_direction>));
    BOOST_MPL_ASSERT ((std::is_same <decltype (range::default_direction (c)),
        forgotten_to_define_direction>));

    // is_homogeneous.
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        weird_count, weird_direction>));
    BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <
        weird_count, weird_opposite_direction>));

    // is_view.
    BOOST_MPL_ASSERT ((range::is_view <weird_count, weird_direction>));
    BOOST_MPL_ASSERT ((range::is_view <weird_count, weird_opposite_direction>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_count, weird_direction, weird_opposite_direction>));

    BOOST_MPL_ASSERT ((range::is_view <weird_count &, weird_direction>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_count const, weird_opposite_direction>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_count const &, weird_direction, weird_opposite_direction>));

    // view.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (weird_count)>));
    BOOST_MPL_ASSERT_NOT ((
        range::has <range::callable::view (weird_count, direction::front)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::view (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::view (
        weird_count, weird_direction)>::type, weird_count>));
    BOOST_MPL_ASSERT ((std::is_same <
        weird_count &, decltype (range::view (c, weird_direction (7)))>));

    // forward_view.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::forward_view (
        weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::forward_view (
        weird_count, direction::front)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::forward_view (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::forward_view (weird_count, weird_direction)>::type,
        weird_count &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (range::forward_view (c, weird_direction (7))),
        weird_count &>));

    // view_once.
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::view_once (weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::view_once (weird_count, direction::front)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::view_once (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::view_once (weird_count, weird_direction)>::type,
        weird_count>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (range::view_once (c, weird_direction (7))), weird_count &>));

    // empty.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::empty (
        weird_count, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::empty (weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::empty (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::empty (
        weird_count, weird_opposite_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of <range::callable::empty (weird_count, weird_direction)
            >::type,
        rime::false_type>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::empty (weird_count, weird_opposite_direction)>::type,
        rime::false_type>));
    BOOST_MPL_ASSERT_NOT ((range::always_empty <weird_count, weird_direction>));
    BOOST_MPL_ASSERT ((range::never_empty <weird_count, weird_direction>));

    // size.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        weird_count, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (weird_count)>));
    // Has no size.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        weird_count, weird_opposite_direction)>));

    // first.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
        weird_count, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
        weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::first (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
        weird_count, weird_opposite_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::first (
        weird_count, weird_direction)>::type, int>));

    // drop.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
        weird_count, direction::front &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
        weird_count, weird_opposite_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::drop (
        weird_count, weird_direction)>::type, weird_count>));

    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_count const, rime::int_<1>, weird_direction)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_count const &, rime::int_<5>, weird_direction)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_count, int, weird_direction)>));

    // chop.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
        weird_count, direction::front &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::chop (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
        weird_count, weird_opposite_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::chop (
        weird_count, weird_direction)>::type,
        range::chopped <int, weird_count>>));

    // chop_in_place.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_count, direction::front &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_count, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_count const &, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_count &&, weird_direction)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
        weird_count &, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_opposite_direction, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of <range::callable::chop_in_place (
            weird_count &, weird_direction)>::type,
        int>));

    // at.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (
        weird_count, int, direction::front &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (
        weird_count, int)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_count, int, weird_direction)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (
        weird_count, int, weird_opposite_direction)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::at (weird_count, int, weird_direction)>::type, int>));

    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_count const, rime::int_<1>, weird_direction)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_count const &, rime::int_<5>, weird_direction)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_count, int, weird_direction)>));

    // Run-time behaviour.
    weird_direction direction (7);
    BOOST_CHECK_EQUAL (range::first (c, direction), 0);
    BOOST_CHECK_EQUAL (range::second (c, direction), 1);
    BOOST_CHECK_EQUAL (range::third (c, direction), 2);
    BOOST_CHECK_EQUAL (range::fourth (c, direction), 3);
    BOOST_CHECK_EQUAL (range::fifth (c, direction), 4);
    BOOST_CHECK_EQUAL (range::sixth (c, direction), 5);
    BOOST_CHECK_EQUAL (range::seventh (c, direction), 6);
    BOOST_CHECK_EQUAL (range::eighth (c, direction), 7);
    BOOST_CHECK_EQUAL (range::ninth (c, direction), 8);
    BOOST_CHECK_EQUAL (range::tenth (c, direction), 9);

    BOOST_CHECK_EQUAL (
        range::at (c, rime::constant <std::size_t, 0u>(), direction), 0);
    BOOST_CHECK_EQUAL (range::at (c, 2, direction), 2);
    BOOST_CHECK_EQUAL (range::at (c, 21, direction), 21);

    BOOST_CHECK_EQUAL (range::at_c <2> (c, direction), 2);
    BOOST_CHECK_EQUAL (range::at_c <5> (c, direction), 5);

    c = range::drop (c, direction);
    BOOST_CHECK_EQUAL (range::first (c, direction), 1);

    range::chopped <int, weird_count> next = range::chop (c, direction);
    BOOST_CHECK_EQUAL (next.first(), 1);
    BOOST_CHECK_EQUAL (range::first (next.rest(), direction), 2);

    c = range::drop (c, rime::constant <std::size_t, 1u>(), direction);
    BOOST_CHECK_EQUAL (range::first (c, direction), 2);

    // Also apply view (c, ...). Should just return c.
    c = range::drop (range::view (c, direction),
        rime::constant <std::size_t, 5u>(), direction);
    BOOST_CHECK_EQUAL (range::first (c, direction), 7);

    BOOST_CHECK_EQUAL (range::chop_in_place (c, direction), 7);
    BOOST_CHECK_EQUAL (range::chop_in_place (c, direction), 8);

    c = range::drop (range::view_once (c, weird_opposite_direction (direction)),
        2, direction);
    BOOST_CHECK_EQUAL (range::first (c, direction), 11);

    c = range::drop (
        range::view (c, direction, weird_opposite_direction (direction)),
        8, direction);
    BOOST_CHECK_EQUAL (range::first (c, direction), 19);

    BOOST_CHECK_EQUAL (
        range::at (c, rime::constant <std::size_t, 5u>(), direction), 24);
}

BOOST_AUTO_TEST_SUITE_END()
