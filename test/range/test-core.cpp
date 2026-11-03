// SPDX-FileCopyrightText: Copyright 2012-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
This tests some aspects of range/core.hpp.
Other aspects are tested on different types of ranges by
- member_view.cpp
- std_container.cpp
- std_tuple.cpp
- function_range.cpp
*/

#define BOOST_TEST_MODULE test_range_core
#include "boost/test/unit_test.hpp"

#include "range/core.hpp"

#include <type_traits>

#include "boost/mpl/assert.hpp"

#include "weird_count.hpp"
#include "weird_direction.hpp"

struct fake_range_1 {};
struct fake_range_1_tag {};

namespace range {

template <> struct tag_of_qualified<fake_range_1>
{
    typedef fake_range_1_tag type;
};

}  // namespace range

BOOST_AUTO_TEST_SUITE(test_range_core)

BOOST_AUTO_TEST_CASE(test_range_tag)
{
    static_assert(
        std::is_same_v<range::tag_of<fake_range_1>::type, fake_range_1_tag>);
    static_assert(std::is_same_v<
                  range::tag_of<fake_range_1 const>::type, fake_range_1_tag>);
    static_assert(
        std::is_same_v<range::tag_of<fake_range_1 &>::type, fake_range_1_tag>);
    static_assert(std::is_same_v<
                  range::tag_of<fake_range_1 const &>::type, fake_range_1_tag>);
    static_assert(
        std::is_same_v<range::tag_of<fake_range_1 &&>::type, fake_range_1_tag>);
    static_assert(
        std::is_same_v<
            range::tag_of<fake_range_1 const &&>::type, fake_range_1_tag>);
}

BOOST_AUTO_TEST_CASE(test_range_core)
{
    {
        static_assert(
            !range::has<range::callable::default_direction(int)>::value);

        static_assert(!range::has<range::callable::view(int)>::value);
        static_assert(!range::has<range::callable::view(int, char)>::value);
        static_assert(
            !range::has<range::callable::view(double, int, char)>::value);

        static_assert(!range::has<range::callable::view_once(int)>::value);
        static_assert(
            !range::has<range::callable::view_once(int, char)>::value);
        static_assert(
            !range::has<range::callable::view_once(double, int, char)>::value);

        static_assert(!range::has<range::callable::empty(int)>::value);
        static_assert(!range::has<range::callable::empty(int, char)>::value);

        static_assert(!range::has<range::callable::size(int)>::value);
        static_assert(!range::has<range::callable::size(int, char)>::value);

        static_assert(!range::has<range::callable::first(int)>::value);
        static_assert(!range::has<range::callable::first(int, char)>::value);

        static_assert(!range::has<range::callable::drop(int)>::value);
        static_assert(!range::has<range::callable::drop(int, char)>::value);
        static_assert(
            !range::has<range::callable::drop(double, int, char)>::value);
        static_assert(!range::has<range::callable::chop(int)>::value);
        static_assert(!range::has<range::callable::chop(int, char)>::value);
        static_assert(
            !range::has<range::callable::chop(double, int, char)>::value);

        static_assert(!range::has<range::callable::chop_in_place(int)>::value);
        static_assert(
            !range::has<range::callable::chop_in_place(int, char)>::value);
        static_assert(!range::has<range::callable::chop_in_place(
                          double, int, char)>::value);

        static_assert(!range::has<range::callable::at(int)>::value);
        static_assert(!range::has<range::callable::at(int, char)>::value);
        static_assert(
            !range::has<range::callable::at(double, int, char)>::value);

        static_assert(!range::has<range::callable::at_c<2>(int)>::value);
        static_assert(!range::has<range::callable::at_c<0>(int, char)>::value);
        static_assert(
            !range::has<range::callable::at_c<1>(double, int, char)>::value);

        static_assert(!range::has<range::callable::second(int)>::value);
        static_assert(!range::has<range::callable::third(int, char)>::value);
        static_assert(
            !range::has<range::callable::seventh(double, int, char)>::value);
    }

    weird_count c;

    // default_direction.
    static_assert(std::is_same_v<
                  range::result_of<range::callable::default_direction(
                      weird_count)>::type,
                  forgotten_to_define_direction>);
    static_assert(std::is_same_v<
                  decltype(range::default_direction(c)),
                  forgotten_to_define_direction>);

    // is_homogeneous.
    static_assert(range::is_homogeneous<weird_count, weird_direction>::value);
    static_assert(
        !range::is_homogeneous<weird_count, weird_opposite_direction>::value);

    // is_view.
    static_assert(range::is_view<weird_count, weird_direction>::value);
    static_assert(range::is_view<weird_count, weird_opposite_direction>::value);
    static_assert(
        range::is_view<
            weird_count, weird_direction, weird_opposite_direction>::value);

    static_assert(range::is_view<weird_count &, weird_direction>::value);
    static_assert(
        range::is_view<weird_count const, weird_opposite_direction>::value);
    static_assert(range::is_view<
                  weird_count const &, weird_direction,
                  weird_opposite_direction>::value);

    // view.
    static_assert(!range::has<range::callable::view(weird_count)>::value);
    static_assert(!range::has<range::callable::view(
                      weird_count, direction::front)>::value);
    static_assert(
        range::has<range::callable::view(weird_count, weird_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::view(
                      weird_count, weird_direction)>::type,
                  weird_count>);
    static_assert(std::is_same_v<
                  weird_count &, decltype(range::view(c, weird_direction(7)))>);

    // forward_view.
    static_assert(
        !range::has<range::callable::forward_view(weird_count)>::value);
    static_assert(!range::has<range::callable::forward_view(
                      weird_count, direction::front)>::value);
    static_assert(range::has<range::callable::forward_view(
                      weird_count, weird_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::forward_view(
                      weird_count, weird_direction)>::type,
                  weird_count &&>);
    static_assert(std::is_same_v<
                  decltype(range::forward_view(c, weird_direction(7))),
                  weird_count &>);

    // view_once.
    static_assert(!range::has<range::callable::view_once(weird_count)>::value);
    static_assert(!range::has<range::callable::view_once(
                      weird_count, direction::front)>::value);
    static_assert(range::has<range::callable::view_once(
                      weird_count, weird_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::view_once(
                      weird_count, weird_direction)>::type,
                  weird_count>);
    static_assert(
        std::is_same_v<
            decltype(range::view_once(c, weird_direction(7))), weird_count &>);

    // empty.
    static_assert(!range::has<range::callable::empty(
                      weird_count, direction::front)>::value);
    static_assert(!range::has<range::callable::empty(weird_count)>::value);
    static_assert(range::has<range::callable::empty(
                      weird_count, weird_direction)>::value);
    static_assert(range::has<range::callable::empty(
                      weird_count, weird_opposite_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::empty(
                      weird_count, weird_direction)>::type,
                  rime::false_type>);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::empty(
                      weird_count, weird_opposite_direction)>::type,
                  rime::false_type>);
    static_assert(!range::always_empty<weird_count, weird_direction>::value);
    static_assert(range::never_empty<weird_count, weird_direction>::value);

    // size.
    static_assert(!range::has<range::callable::size(
                      weird_count, direction::front)>::value);
    static_assert(!range::has<range::callable::size(weird_count)>::value);
    // Has no size.
    static_assert(!range::has<range::callable::size(
                      weird_count, weird_direction)>::value);
    static_assert(!range::has<range::callable::size(
                      weird_count, weird_opposite_direction)>::value);

    // first.
    static_assert(!range::has<range::callable::first(
                      weird_count, direction::front)>::value);
    static_assert(!range::has<range::callable::first(weird_count)>::value);
    static_assert(range::has<range::callable::first(
                      weird_count, weird_direction)>::value);
    static_assert(!range::has<range::callable::first(
                      weird_count, weird_opposite_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::first(
                      weird_count, weird_direction)>::type,
                  int>);

    // drop.
    static_assert(!range::has<range::callable::drop(
                      weird_count, direction::front &)>::value);
    static_assert(!range::has<range::callable::drop(weird_count)>::value);
    static_assert(
        range::has<range::callable::drop(weird_count, weird_direction)>::value);
    static_assert(!range::has<range::callable::drop(
                      weird_count, weird_opposite_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::drop(
                      weird_count, weird_direction)>::type,
                  weird_count>);

    static_assert(
        range::has<range::callable::drop(
            weird_count const, rime::int_<1>, weird_direction)>::value);
    static_assert(
        range::has<range::callable::drop(
            weird_count const &, rime::int_<5>, weird_direction)>::value);
    static_assert(range::has<range::callable::drop(
                      weird_count, int, weird_direction)>::value);

    // chop.
    static_assert(!range::has<range::callable::chop(
                      weird_count, direction::front &)>::value);
    static_assert(!range::has<range::callable::chop(weird_count)>::value);
    static_assert(
        range::has<range::callable::chop(weird_count, weird_direction)>::value);
    static_assert(!range::has<range::callable::chop(
                      weird_count, weird_opposite_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::chop(
                      weird_count, weird_direction)>::type,
                  range::chopped<int, weird_count>>);

    // chop_in_place.
    static_assert(!range::has<range::callable::chop_in_place(
                      weird_count, direction::front &)>::value);
    static_assert(
        !range::has<range::callable::chop_in_place(weird_count)>::value);
    static_assert(!range::has<range::callable::chop_in_place(
                      weird_count, weird_direction)>::value);
    static_assert(!range::has<range::callable::chop_in_place(
                      weird_count const &, weird_direction)>::value);
    static_assert(!range::has<range::callable::chop_in_place(
                      weird_count &&, weird_direction)>::value);
    static_assert(range::has<range::callable::chop_in_place(
                      weird_count &, weird_direction)>::value);
    static_assert(!range::has<range::callable::chop_in_place(
                      weird_opposite_direction, weird_count)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::chop_in_place(
                      weird_count &, weird_direction)>::type,
                  int>);

    // at.
    static_assert(!range::has<range::callable::at(
                      weird_count, int, direction::front &)>::value);
    static_assert(!range::has<range::callable::at(weird_count, int)>::value);
    static_assert(range::has<range::callable::at(
                      weird_count, int, weird_direction)>::value);
    static_assert(!range::has<range::callable::at(
                      weird_count, int, weird_opposite_direction)>::value);
    static_assert(std::is_same_v<
                  range::result_of<range::callable::at(
                      weird_count, int, weird_direction)>::type,
                  int>);

    static_assert(
        range::has<range::callable::at(
            weird_count const, rime::int_<1>, weird_direction)>::value);
    static_assert(
        range::has<range::callable::at(
            weird_count const &, rime::int_<5>, weird_direction)>::value);
    static_assert(range::has<range::callable::at(
                      weird_count, int, weird_direction)>::value);

    // Run-time behaviour.
    weird_direction direction(7);
    BOOST_CHECK_EQUAL(range::first(c, direction), 0);
    BOOST_CHECK_EQUAL(range::second(c, direction), 1);
    BOOST_CHECK_EQUAL(range::third(c, direction), 2);
    BOOST_CHECK_EQUAL(range::fourth(c, direction), 3);
    BOOST_CHECK_EQUAL(range::fifth(c, direction), 4);
    BOOST_CHECK_EQUAL(range::sixth(c, direction), 5);
    BOOST_CHECK_EQUAL(range::seventh(c, direction), 6);
    BOOST_CHECK_EQUAL(range::eighth(c, direction), 7);
    BOOST_CHECK_EQUAL(range::ninth(c, direction), 8);
    BOOST_CHECK_EQUAL(range::tenth(c, direction), 9);

    BOOST_CHECK_EQUAL(
        range::at(c, rime::constant<std::size_t, 0u>(), direction), 0);
    BOOST_CHECK_EQUAL(range::at(c, 2, direction), 2);
    BOOST_CHECK_EQUAL(range::at(c, 21, direction), 21);

    BOOST_CHECK_EQUAL(range::at_c<2>(c, direction), 2);
    BOOST_CHECK_EQUAL(range::at_c<5>(c, direction), 5);

    c = range::drop(c, direction);
    BOOST_CHECK_EQUAL(range::first(c, direction), 1);

    range::chopped<int, weird_count> next = range::chop(c, direction);
    BOOST_CHECK_EQUAL(next.first(), 1);
    BOOST_CHECK_EQUAL(range::first(next.rest(), direction), 2);

    c = range::drop(c, rime::constant<std::size_t, 1u>(), direction);
    BOOST_CHECK_EQUAL(range::first(c, direction), 2);

    // Also apply view (c, ...). Should just return c.
    c = range::drop(
        range::view(c, direction), rime::constant<std::size_t, 5u>(),
        direction);
    BOOST_CHECK_EQUAL(range::first(c, direction), 7);

    BOOST_CHECK_EQUAL(range::chop_in_place(c, direction), 7);
    BOOST_CHECK_EQUAL(range::chop_in_place(c, direction), 8);

    c = range::drop(
        range::view_once(c, weird_opposite_direction(direction)), 2, direction);
    BOOST_CHECK_EQUAL(range::first(c, direction), 11);

    c = range::drop(
        range::view(c, direction, weird_opposite_direction(direction)), 8,
        direction);
    BOOST_CHECK_EQUAL(range::first(c, direction), 19);

    BOOST_CHECK_EQUAL(
        range::at(c, rime::constant<std::size_t, 5u>(), direction), 24);
}

BOOST_AUTO_TEST_SUITE_END()
