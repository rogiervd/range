/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_tuple_types
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"

#include <type_traits>
#include <string>
#include <tuple>
#include <vector>

#include <boost/mpl/assert.hpp>

#include "utility/test/tracked.hpp"

#include "range/std.hpp"

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_types)

using range::tuple;
using range::element_type;

using range::size;
using range::first;
using range::at;
using range::at_c;
using range::view;
using range::view_once;

using range::always_empty;
using range::never_empty;

rime::size_t <0> zero;
rime::size_t <1> one;
rime::size_t <2> two;

BOOST_AUTO_TEST_CASE (tuple_element_type) {
    BOOST_MPL_ASSERT ((std::is_same <
        range::element_type <0, tuple <int>>::type, int>));

    BOOST_MPL_ASSERT ((std::is_same <
        range::element_type <0, tuple <int, bool &> &>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::element_type <1, tuple <int, bool &> const>::type, bool &>));

    BOOST_MPL_ASSERT ((std::is_same <
        range::element_type <0, tuple <double const, bool, float> &&
         >::type, double const>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::element_type <1, tuple <double const, bool, float> const &
        >::type, bool>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::element_type <2, tuple <double const, bool, float> const
        >::type, float>));
}

BOOST_AUTO_TEST_CASE (tuple_types) {
    static_assert (always_empty <direction::front, tuple<>>::value, "");
    static_assert (always_empty <direction::back, tuple<>>::value, "");
    static_assert (never_empty <direction::front, tuple <int>>::value, "");
    static_assert (never_empty <direction::back, tuple <int>>::value, "");
    static_assert (
        never_empty <direction::front, tuple <int, bool>>::value, "");
    static_assert (
        never_empty <direction::back, tuple <int, bool>>::value, "");

    RIME_CHECK_EQUAL (size (tuple<>()), zero);
    RIME_CHECK_EQUAL (size (tuple <int> (1)), one);
    RIME_CHECK_EQUAL (size (tuple <int, bool> (2, false)), two);

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (tuple<>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (tuple<>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::at (rime::size_t <0>, tuple<>)>));

    BOOST_MPL_ASSERT ((range::has <range::callable::first (tuple <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::drop (tuple <int>)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::at (rime::size_t <0>, tuple <int>)>));

    // Test reference, const reference, and
    // rvalue reference without and with view_once.
    {
        tuple <int> t;
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (at_c <0> (t)), int &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (at (zero, t)), int &>));
    }
    {
        tuple <int> const t;
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, t)), int const &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (view (tuple <int>()))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (view (tuple <int>()))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, view (tuple <int>()))), int const &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (view_once (tuple <int>()))), int &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (view_once (tuple <int>()))), int &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, view_once (tuple <int>()))), int &&>));
    }
    {
        // first uses view_once.
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (tuple <int>())), int &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (tuple <int>())), int &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, tuple <int>())), int &&>));
    }

    // The same on a const contained type.
    {
        tuple <int const> t;
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, t)), int const &>));
    }
    {
        tuple <int const> const t;
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, t)), int const &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            first (view (tuple <int const>()))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at_c <0> (view (tuple <int const>()))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at (zero, view (tuple <int const>()))), int const &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            first (view_once (tuple <int const>()))), int const &&>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at_c <0> (view_once (tuple <int const>()))), int const &&>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at (zero, view_once (tuple <int const>()))), int const &&>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            first (tuple <int const>())), int const &&>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at_c <0> (tuple <int const>())), int const &&>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at (zero, tuple <int const>())), int const &&>));
    }

    int i = 5;
    // The same on a reference contained type.
    {
        tuple <int &> t (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (at_c <0> (t)), int &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (at (zero, t)), int &>));
    }
    {
        tuple <int &> const t (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (at_c <0> (t)), int &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (at (zero, t)), int &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (view (tuple <int &> (i)))), int &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (view (tuple <int &> (i)))), int &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, view (tuple <int &> (i)))), int &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (view_once (tuple <int &> (i)))), int &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (view_once (tuple <int &> (i)))), int &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, view_once (tuple <int &> (i)))), int &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (tuple <int &> (i))), int &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (tuple <int &> (i))), int &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, tuple <int &> (i))), int &>));
    }

    // The same on a const reference contained type.
    {
        tuple <int const &> t (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, t)), int const &>));
    }
    {
        tuple <int const &> const t (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <0> (t)), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at (zero, t)), int const &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            first (view (tuple <int const &> (i)))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at_c <0> (view (tuple <int const &> (i)))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at (zero, view (tuple <int const &> (i)))), int const &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            first (view_once (tuple <int const &> (i)))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at_c <0> (view_once (tuple <int const &> (i)))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at (zero, view_once (tuple <int const &> (i)))), int const &>));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            first (tuple <int const &> (i))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at_c <0> (tuple <int const &> (i))), int const &>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (
            at (zero, tuple <int const &> (i))), int const &>));
    }
}

BOOST_AUTO_TEST_SUITE_END()