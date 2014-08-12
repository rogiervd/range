/*
Copyright 2012-2014 Rogier van Dalen.

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

struct fake_range_1;
struct fake_range_1_tag;

struct fake_range_2;
struct fake_range_2_normal_tag;
struct fake_range_2_rvalue_tag;
struct fake_range_2_lvalue_reference_tag;

namespace range {

    template <> struct tag_of_qualified <fake_range_1>
    { typedef fake_range_1_tag type; };

    template <> struct tag_of_qualified <fake_range_2, const_reference>
    { typedef fake_range_2_normal_tag type; };
    template <> struct tag_of_qualified <fake_range_2, temporary>
    { typedef fake_range_2_rvalue_tag type; };
    template <> struct tag_of_qualified <fake_range_2, reference>
    { typedef fake_range_2_lvalue_reference_tag type; };

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

    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_2>::type, fake_range_2_rvalue_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_2 const>::type, fake_range_2_normal_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_2 &>::type, fake_range_2_lvalue_reference_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_2 const &>::type, fake_range_2_normal_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_2 &&>::type, fake_range_2_rvalue_tag>));
    BOOST_MPL_ASSERT ((std::is_same <range::tag_of <
        fake_range_2 const &&>::type, fake_range_2_normal_tag>));
}

BOOST_AUTO_TEST_CASE (test_range_core) {
    {
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::default_direction (
            int)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (int, char)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (
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
        weird_direction, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <
        weird_reverse_direction, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <
        weird_direction, weird_reverse_direction, weird_count>));

    // is_view.
    BOOST_MPL_ASSERT ((range::is_view <weird_direction, weird_count>));
    BOOST_MPL_ASSERT ((range::is_view <weird_reverse_direction, weird_count>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_direction, weird_reverse_direction, weird_count>));

    BOOST_MPL_ASSERT ((range::is_view <weird_direction, weird_count &>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_reverse_direction, weird_count const>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_direction, weird_reverse_direction, weird_count const &>));

    // view.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::view (weird_count)>));
    BOOST_MPL_ASSERT_NOT ((
        range::has <range::callable::view (direction::front, weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::view (
        weird_direction, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::view (
        weird_direction, weird_count)>::type, weird_count &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (range::view (weird_direction (7), c)), weird_count &>));

    // empty.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::empty (
        direction::front, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::empty (weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::empty (
        weird_direction, weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::empty (
        weird_reverse_direction, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of <range::callable::empty (weird_direction, weird_count)
            >::type,
        rime::false_type>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::empty (weird_reverse_direction, weird_count)>::type,
        rime::false_type>));

    // size.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        direction::front, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (weird_count)>));
    // Has no size.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        weird_direction, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        weird_reverse_direction, weird_count)>));

    // first.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
        direction::front, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
        weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::first (
        weird_direction, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
        weird_reverse_direction, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::first (
        weird_direction, weird_count)>::type, int>));

    // drop.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
        direction::front &, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_direction, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
        weird_reverse_direction, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::drop (
        weird_direction, weird_count)>::type, weird_count>));

    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_direction, rime::int_<1>, weird_count const)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_direction, rime::int_<5>, weird_count const &)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::drop (
        weird_direction, int, weird_count)>));

    // chop.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
        direction::front &, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::chop (
        weird_direction, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
        weird_reverse_direction, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <range::callable::chop (
        weird_direction, weird_count)>::type,
        range::chopped <int, weird_count>>));

    // chop_in_place.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        direction::front &, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_direction, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_direction, weird_count const &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_direction, weird_count &&)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
        weird_direction, weird_count &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
        weird_reverse_direction, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of <range::callable::chop_in_place (
            weird_direction, weird_count &)>::type,
        int>));

    // at.
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (
        direction::front &, int, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (
        int, weird_count)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_direction, int, weird_count)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::at (
        weird_reverse_direction, int, weird_count)>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::at (weird_direction, int, weird_count)>::type, int>));

    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_direction, rime::int_<1>, weird_count const)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_direction, rime::int_<5>, weird_count const &)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::at (
        weird_direction, int, weird_count)>));

    // Run-time behaviour.
    weird_direction direction (7);
    BOOST_CHECK_EQUAL (range::first (direction, c), 0);
    BOOST_CHECK_EQUAL (range::at (direction, rime::constant <std::size_t, 0u>(),
        c), 0);
    BOOST_CHECK_EQUAL (range::at (direction, 2, c), 2);
    BOOST_CHECK_EQUAL (range::at (direction, 21, c), 21);

    c = range::drop (direction, c);
    BOOST_CHECK_EQUAL (range::first (direction, c), 1);

    range::chopped <int, weird_count> next = range::chop (direction, c);
    BOOST_CHECK_EQUAL (next.first(), 1);
    BOOST_CHECK_EQUAL (range::first (direction, next.rest()), 2);

    c = range::drop (direction, rime::constant <std::size_t, 1u>(), c);
    BOOST_CHECK_EQUAL (range::first (direction, c), 2);

    // Also apply view (..., c). Should just return c.
    c = range::drop (direction, rime::constant <std::size_t, 5u>(),
        range::view (direction, c));
    BOOST_CHECK_EQUAL (range::first (direction, c), 7);

    BOOST_CHECK_EQUAL (range::chop_in_place (direction, c), 7);
    BOOST_CHECK_EQUAL (range::chop_in_place (direction, c), 8);

    c = range::drop (direction, 2,
        range::view (weird_reverse_direction (direction), c));
    BOOST_CHECK_EQUAL (range::first (direction, c), 11);

    c = range::drop (direction, 8,
        range::view (direction, weird_reverse_direction (direction), c));
    BOOST_CHECK_EQUAL (range::first (direction, c), 19);

    BOOST_CHECK_EQUAL (range::at (direction, rime::constant <std::size_t, 5u>(),
        c), 24);
}

BOOST_AUTO_TEST_SUITE_END()

