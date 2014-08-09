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

#define BOOST_TEST_MODULE test_container_std_tuple
#include "utility/test/boost_unit_test.hpp"

#include "range/std/tuple.hpp"

#include <tuple>
#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "../range/check_equal_behaviour.hpp"

BOOST_AUTO_TEST_SUITE(test_container_std_tuple)

BOOST_AUTO_TEST_CASE (test_std_tuple_types) {
    {
        BOOST_MPL_ASSERT_NOT ((range::is_view <std::tuple<>>));
        BOOST_MPL_ASSERT_NOT ((range::is_view <std::tuple<>>));
        BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <std::tuple<>>));
        std::tuple<> t;
        BOOST_MPL_ASSERT ((range::is_view <decltype (range::view (t))>));
        BOOST_MPL_ASSERT_NOT ((
            range::is_homogeneous <decltype (range::view (t))>));
    }
    {
        BOOST_MPL_ASSERT_NOT ((range::is_view <std::tuple <int>>));
        BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <std::tuple <int>>));
        std::tuple <int> t (4);
        BOOST_MPL_ASSERT ((range::is_view <decltype (range::view (t)) &>));
        BOOST_MPL_ASSERT_NOT ((
            range::is_homogeneous <decltype (range::view (t))>));
    }
    {
        BOOST_MPL_ASSERT_NOT ((range::is_view <std::tuple <int, double> &>));
        BOOST_MPL_ASSERT_NOT ((
            range::is_homogeneous <std::tuple <int, double>>));
        std::tuple <int, double> t (4, 5.);
        BOOST_MPL_ASSERT ((range::is_view <decltype (range::view (t)) const>));
        BOOST_MPL_ASSERT_NOT ((
            range::is_homogeneous <decltype (range::view (t))>));
    }
}

BOOST_AUTO_TEST_CASE (test_std_tuple) {
    // Zero elements
    {
        std::tuple<> t;
        BOOST_CHECK (range::empty (t));
        check_equal_value (range::size (t), rime::size_t <0>());

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::first (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::at (rime::size_t <0>, decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::at (rime::size_t <1>, decltype (t))>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::chop (decltype (t))>));
    }

    // One element.
    {
        std::tuple <double> const t (6.3);
        BOOST_CHECK (!range::empty (t));
        check_equal_value (range::size (t), rime::size_t <1>());

        BOOST_MPL_ASSERT ((
            range::has <range::callable::first (decltype (t) &)>));
        BOOST_MPL_ASSERT ((
            range::has <range::callable::at (rime::size_t <0>, decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::at (rime::size_t <1>, decltype (t))>));

        BOOST_MPL_ASSERT ((range::has <
            range::callable::chop (decltype (t))>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::chop (decltype (range::view (t)))>));

        // Tuples are not homogeneous.
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (range::view (t)))>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (range::view (t)) &)>));

        check_equal_value (range::first (t), 6.3);
        check_equal_value (range::first (range::front, t), 6.3);
        check_equal_value (range::first (range::back, t), 6.3);

        check_equal_value (range::at (rime::size_t <0>(), t), 6.3);
        check_equal_value (
            range::at (range::front, rime::size_t <0>(), t), 6.3);
        check_equal_value (
            range::at (range::back, rime::size_t <0>(), t), 6.3);

        auto first_and_empty_1 = range::chop (t);
        auto first_and_empty_2 = range::chop (range::back, t);
        check_equal_value (first_and_empty_1.first(), 6.3);
        check_equal_value (first_and_empty_2.first(), 6.3);
        check_equal_value (
            range::empty (first_and_empty_1.rest()), rime::true_);
        check_equal_value (
            range::empty (first_and_empty_2.rest()), rime::true_);
    }

    // Two elements.
    {
        std::tuple <int, char> t (4, 'a');

        BOOST_CHECK (!range::empty (t));
        check_equal_value (range::size (t), rime::size_t <2>());

        check_equal_value (range::first (range::front, t), 4);
        check_equal_value (range::first (t), 4);
        check_equal_value (range::first (range::back, t), 'a');

        check_equal_value (range::first (range::drop (t)), 'a');

        BOOST_CHECK (!range::empty (range::drop (range::back, t)));
        BOOST_CHECK (!range::empty (range::drop (t)));
        BOOST_CHECK (range::empty (range::drop (rime::size_t <2>(), t)));
        BOOST_CHECK (range::empty (range::drop (
            range::back, rime::size_t <2>(), t)));

        check_equal_value (range::at (rime::size_t <0>(), t), 4);
        check_equal_value (range::at (rime::size_t <1>(), t), 'a');
        check_equal_value (range::at (range::front, rime::size_t <0>(), t), 4);
        check_equal_value (range::at (range::back, rime::size_t <0>(), t), 'a');
        check_equal_value (range::at (range::back, rime::size_t <1>(), t), 4);

        auto first_and_rest = range::chop (t);
        auto last_and_rest = range::chop (range::back, t);
        check_equal_value (first_and_rest.first(), 4);
        check_equal_value (last_and_rest.first(), 'a');
        check_equal_value (range::size (first_and_rest.rest()),
            rime::size_t <1>());
        check_equal_value (range::size (last_and_rest.rest()),
            rime::size_t <1>());

        auto first_and_empty = range::chop (last_and_rest.rest());
        check_equal_value (first_and_empty.first(), 4);
        check_equal_value (range::empty (first_and_empty.rest()), rime::true_);

        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (range::view (t)))>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (range::view (t)) &)>));
    }

    // Three elements
    {
        std::tuple <int, char, double> t (4, 'a', 6.3);

        BOOST_CHECK (!range::empty (t));
        check_equal_value (range::size (t), rime::size_t <3>());

        check_equal_value (range::first (range::front, t), 4);
        check_equal_value (range::first (t), 4);
        check_equal_value (range::first (range::drop (t)), 'a');
        check_equal_value (range::first (range::back,
            range::drop (range::back, t)), 'a');
        check_equal_value (range::first (range::back, t), 6.3);

        BOOST_CHECK (!range::empty (range::drop (range::back, t)));
        BOOST_CHECK (!range::empty (range::drop (t)));
        BOOST_CHECK (!range::empty (range::drop (rime::size_t <2>(), t)));
        BOOST_CHECK (range::empty (range::drop (rime::size_t <3>(), t)));
        BOOST_CHECK (!range::empty (range::drop (
            range::back, rime::size_t <2>(), t)));
        BOOST_CHECK (range::empty (range::drop (
            range::back, rime::size_t <3>(), t)));

        check_equal_value (range::at (rime::size_t <0>(), t), 4);
        check_equal_value (range::at (rime::size_t <1>(), t), 'a');
        check_equal_value (range::at (rime::size_t <2>(), t), 6.3);
        check_equal_value (range::at (range::front, rime::size_t <0>(), t), 4);
        check_equal_value (
            range::at (range::front, rime::size_t <1>(), t), 'a');
        check_equal_value (
            range::at (range::front, rime::size_t <2>(), t), 6.3);
        check_equal_value (range::at (range::back, rime::size_t <0>(), t), 6.3);
        check_equal_value (range::at (range::back, rime::size_t <1>(), t), 'a');
        check_equal_value (range::at (range::back, rime::size_t <2>(), t), 4);

        auto first_and_rest = range::chop (t);
        auto last_and_rest = range::chop (range::back, t);
        check_equal_value (first_and_rest.first(), 4);
        check_equal_value (last_and_rest.first(), 6.3);
        check_equal_value (range::size (first_and_rest.rest()),
            rime::size_t <2>());
        check_equal_value (range::size (last_and_rest.rest()),
            rime::size_t <2>());

        auto first_and_middle = range::chop (last_and_rest.rest());
        check_equal_value (first_and_middle.first(), 4);
        check_equal_value (range::size (first_and_middle.rest()),
            rime::size_t <1>());

        auto middle_and_empty = range::chop (first_and_middle.rest());
        check_equal_value (middle_and_empty.first(), 'a');
        check_equal_value (range::empty (middle_and_empty.rest()), rime::true_);

        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (range::view (t)))>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::chop_in_place (decltype (range::view (t)) &)>));
    }
}

BOOST_AUTO_TEST_CASE (test_std_pair) {
    std::tuple <int, char> t (4, 'a');
    std::pair <int, char> p (4, 'a');

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <rime::true_type, rime::true_type,
        rime::true_type, rime::false_type> (p, t);
}

BOOST_AUTO_TEST_SUITE_END()

