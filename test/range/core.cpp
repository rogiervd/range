/*
Copyright 2012, 2013 Rogier van Dalen.

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

/*
This tests some aspects of range/core.hpp.
Other aspects are tested on different types of ranges by
- member_view.cpp
- std_adaptor.cpp
- std_tuple.cpp
*/

#define BOOST_TEST_MODULE test_range_core
#include "../boost_unit_test.hpp"

#include "range/core.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "weird_direction.hpp"
#include "weird_count.hpp"

BOOST_AUTO_TEST_SUITE(test_range_core)

BOOST_AUTO_TEST_CASE (test_range_core) {
    {
        BOOST_MPL_ASSERT_NOT ((range::has::default_direction <int>));

        BOOST_MPL_ASSERT_NOT ((range::has::view <int>));
        BOOST_MPL_ASSERT_NOT ((range::has::view <int, char>));
        BOOST_MPL_ASSERT_NOT ((range::has::view <double, int, char>));

        BOOST_MPL_ASSERT_NOT ((range::has::empty <int>));
        BOOST_MPL_ASSERT_NOT ((range::has::empty <int, char>));

        BOOST_MPL_ASSERT_NOT ((range::has::size <int>));
        BOOST_MPL_ASSERT_NOT ((range::has::size <int, char>));

        BOOST_MPL_ASSERT_NOT ((range::has::first <int>));
        BOOST_MPL_ASSERT_NOT ((range::has::first <int, char>));

        BOOST_MPL_ASSERT_NOT ((range::has::drop <int>));
        BOOST_MPL_ASSERT_NOT ((range::has::drop <int, char>));
        BOOST_MPL_ASSERT_NOT ((range::has::drop <double, int, char>));
    }

    weird_count c;

    BOOST_MPL_ASSERT ((std::is_same <range::result_of::default_direction <
        weird_count>::type, forgotten_to_define_direction>));
    BOOST_MPL_ASSERT ((std::is_same <decltype (range::default_direction (c)),
        forgotten_to_define_direction>));

    BOOST_MPL_ASSERT ((range::is_homogeneous <
        weird_direction, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <
        weird_reverse_direction, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <
        weird_direction, weird_reverse_direction, weird_count>));

    BOOST_MPL_ASSERT ((range::is_view <weird_direction, weird_count>));
    BOOST_MPL_ASSERT ((range::is_view <weird_reverse_direction, weird_count>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_direction, weird_reverse_direction, weird_count>));

    BOOST_MPL_ASSERT ((range::is_view <weird_direction, weird_count &>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_reverse_direction, weird_count const>));
    BOOST_MPL_ASSERT ((range::is_view <
        weird_direction, weird_reverse_direction, weird_count const &>));

    BOOST_MPL_ASSERT_NOT ((range::has::view <weird_count>));
    BOOST_MPL_ASSERT_NOT ((
        range::has::view <direction::front, weird_count>));
    BOOST_MPL_ASSERT ((range::has::view <weird_direction, weird_count>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of::view <
        weird_direction, weird_count>::type, weird_count &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (range::view (weird_direction (7), c)), weird_count &>));

    BOOST_MPL_ASSERT_NOT ((range::has::empty <direction::front, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::has::empty <weird_count>));
    BOOST_MPL_ASSERT ((range::has::empty <weird_direction, weird_count>));
    BOOST_MPL_ASSERT ((
        range::has::empty <weird_reverse_direction, weird_count>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of::empty <weird_direction, weird_count>::type,
        rime::false_type>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of::empty <weird_reverse_direction, weird_count>::type,
        rime::false_type>));

    BOOST_MPL_ASSERT_NOT ((range::has::size <direction::front, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::has::size <weird_count>));
    // Has no size.
    BOOST_MPL_ASSERT_NOT ((range::has::size <weird_direction, weird_count>));
    BOOST_MPL_ASSERT_NOT ((
        range::has::size <weird_reverse_direction, weird_count>));

    BOOST_MPL_ASSERT_NOT ((range::has::first <direction::front, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::has::first <weird_count>));
    BOOST_MPL_ASSERT ((range::has::first <weird_direction, weird_count>));
    BOOST_MPL_ASSERT_NOT ((
        range::has::first <weird_reverse_direction, weird_count>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of::first <weird_direction, weird_count>::type, int>));

    BOOST_MPL_ASSERT_NOT ((range::has::drop <direction::front &, weird_count>));
    BOOST_MPL_ASSERT_NOT ((range::has::drop <weird_count>));
    BOOST_MPL_ASSERT ((range::has::drop <weird_direction, weird_count>));
    BOOST_MPL_ASSERT_NOT ((
        range::has::drop <weird_reverse_direction, weird_count>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::result_of::drop <weird_direction, weird_count>::type,
        weird_count>));

    BOOST_MPL_ASSERT ((range::has::drop <weird_direction, rime::int_<1>,
        weird_count const>));
    BOOST_MPL_ASSERT ((range::has::drop <weird_direction, rime::int_<5>,
        weird_count const &>));
    BOOST_MPL_ASSERT ((range::has::drop <weird_direction, int, weird_count>));

    // Run-time behaviour.
    weird_direction direction (7);
    BOOST_CHECK_EQUAL (range::first (direction, c), 0);

    c = range::drop (direction, c);
    BOOST_CHECK_EQUAL (range::first (direction, c), 1);

    c = range::drop (direction, rime::constant <std::size_t, 1u>(), c);
    BOOST_CHECK_EQUAL (range::first (direction, c), 2);

    // Also apply view (..., c). Should just return c.
    c = range::drop (direction, rime::constant <std::size_t, 5u>(),
        range::view (direction, c));
    BOOST_CHECK_EQUAL (range::first (direction, c), 7);

    c = range::drop (direction, 4,
        range::view (weird_reverse_direction (direction), c));
    BOOST_CHECK_EQUAL (range::first (direction, c), 11);

    c = range::drop (direction, 8,
        range::view (direction, weird_reverse_direction (direction), c));
    BOOST_CHECK_EQUAL (range::first (direction, c), 19);
}

BOOST_AUTO_TEST_SUITE_END()

