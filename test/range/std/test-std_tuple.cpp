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

#include "../check_equal_behaviour.hpp"

BOOST_AUTO_TEST_SUITE(test_container_std_tuple)

using range::empty;
using range::size;
using range::first;
using range::at;
using range::at_c;
using range::drop;
using range::chop;

namespace callable = range::callable;

using range::front;
using range::back;

using range::view;

using range::always_empty;
using range::never_empty;
using range::has;
using range::is_view;
using range::is_homogeneous;

typedef rime::size_t <0> zero_type;
rime::size_t <1> one;
rime::size_t <2> two;
rime::size_t <3> three;

using rime::true_;
using rime::false_;

BOOST_AUTO_TEST_CASE (test_std_tuple_types) {
    {
        BOOST_MPL_ASSERT_NOT ((is_view <std::tuple<>>));
        BOOST_MPL_ASSERT_NOT ((is_view <std::tuple<>>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <std::tuple<>>));
        std::tuple<> t;
        BOOST_MPL_ASSERT ((is_view <decltype (view (t))>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (view (t))>));
    }
    {
        BOOST_MPL_ASSERT_NOT ((is_view <std::tuple <int>>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <std::tuple <int>>));
        std::tuple <int> t (4);
        BOOST_MPL_ASSERT ((is_view <decltype (view (t)) &>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (view (t))>));
    }
    {
        BOOST_MPL_ASSERT_NOT ((is_view <std::tuple <int, double> &>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <std::tuple <int, double>>));
        std::tuple <int, double> t (4, 5.);
        BOOST_MPL_ASSERT ((is_view <decltype (view (t)) const>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (view (t))>));
    }
}

BOOST_AUTO_TEST_CASE (test_std_tuple) {
    // Zero elements
    {
        std::tuple<> t;
        BOOST_CHECK (empty (t));
        BOOST_MPL_ASSERT ((always_empty <direction::front, decltype (t)>));
        BOOST_MPL_ASSERT ((always_empty <direction::back, decltype (t)>));
        check_equal_value (size (t), zero_type());

        BOOST_MPL_ASSERT_NOT ((has <range::callable::first (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((
            has <range::callable::at (zero_type, decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((
            has <range::callable::at (rime::size_t <1>, decltype (t))>));

        BOOST_MPL_ASSERT_NOT ((has <range::callable::chop (decltype (t))>));
    }

    // One element.
    {
        std::tuple <double> const t (6.3);
        BOOST_CHECK (!empty (t));
        BOOST_MPL_ASSERT ((never_empty <direction::front, decltype (t)>));
        BOOST_MPL_ASSERT ((never_empty <direction::back, decltype (t)>));
        check_equal_value (size (t), one);

        BOOST_MPL_ASSERT ((has <range::callable::first (decltype (t) &)>));
        BOOST_MPL_ASSERT ((
            has <range::callable::at (zero_type, decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((
            has <range::callable::at (rime::size_t <1>, decltype (t))>));

        BOOST_MPL_ASSERT ((has <range::callable::chop (decltype (t))>));
        BOOST_MPL_ASSERT ((has <range::callable::chop (decltype (view (t)))>));

        // Tuples are not homogeneous.
        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (view (t)))>));
        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (view (t)) &)>));

        check_equal_value (first (t), 6.3);
        check_equal_value (first (front, t), 6.3);
        check_equal_value (first (back, t), 6.3);

        check_equal_value (at (zero_type(), t), 6.3);
        check_equal_value (at (front, zero_type(), t), 6.3);
        check_equal_value (at (back, zero_type(), t), 6.3);

        auto first_and_empty_1 = range::chop (t);
        auto first_and_empty_2 = range::chop (back, t);
        check_equal_value (first_and_empty_1.first(), 6.3);
        check_equal_value (first_and_empty_2.first(), 6.3);
        check_equal_value (empty (first_and_empty_1.rest()), true_);
        check_equal_value (empty (first_and_empty_2.rest()), true_);
        BOOST_MPL_ASSERT ((always_empty <
            direction::front, decltype (first_and_empty_2.rest())>));
        BOOST_MPL_ASSERT ((always_empty <
            direction::back, decltype (first_and_empty_2.rest())>));
    }

    // Two elements.
    {
        std::tuple <int, char> t (4, 'a');

        BOOST_CHECK (!empty (t));
        BOOST_MPL_ASSERT ((never_empty <direction::front, decltype (t)>));
        BOOST_MPL_ASSERT ((never_empty <direction::back, decltype (t)>));
        check_equal_value (size (t), two);

        check_equal_value (first (front, t), 4);
        check_equal_value (first (t), 4);
        check_equal_value (first (back, t), 'a');

        check_equal_value (first (drop (t)), 'a');

        BOOST_CHECK (!empty (drop (back, t)));
        BOOST_CHECK (!empty (drop (t)));
        BOOST_CHECK (empty (drop (two, t)));
        BOOST_CHECK (empty (drop (back, two, t)));

        check_equal_value (at (zero_type(), t), 4);
        check_equal_value (at (one, t), 'a');
        check_equal_value (at (front, zero_type(), t), 4);
        check_equal_value (at (back, zero_type(), t), 'a');
        check_equal_value (at (back, one, t), 4);

        auto first_and_rest = range::chop (t);
        auto last_and_rest = range::chop (back, t);
        check_equal_value (first_and_rest.first(), 4);
        check_equal_value (last_and_rest.first(), 'a');
        check_equal_value (size (first_and_rest.rest()), one);
        check_equal_value (size (last_and_rest.rest()), one);

        auto first_and_empty = range::chop (last_and_rest.rest());
        check_equal_value (first_and_empty.first(), 4);
        check_equal_value (empty (first_and_empty.rest()), true_);

        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (view (t)))>));
        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (view (t)) &)>));
    }

    // Three elements
    {
        std::tuple <int, char, double> t (4, 'a', 6.3);

        BOOST_CHECK (!empty (t));
        check_equal_value (size (t), three);

        check_equal_value (first (front, t), 4);
        check_equal_value (first (t), 4);
        check_equal_value (first (drop (t)), 'a');
        check_equal_value (first (back, drop (back, t)), 'a');
        check_equal_value (first (back, t), 6.3);

        BOOST_CHECK (!empty (drop (back, t)));
        BOOST_CHECK (!empty (drop (t)));
        BOOST_CHECK (!empty (drop (two, t)));
        BOOST_CHECK (empty (drop (three, t)));
        BOOST_CHECK (!empty (drop (back, two, t)));
        BOOST_CHECK (empty (drop (back, three, t)));

        check_equal_value (at (zero_type(), t), 4);
        check_equal_value (at (one, t), 'a');
        check_equal_value (at (two, t), 6.3);
        check_equal_value (at (front, zero_type(), t), 4);
        check_equal_value (at (front, one, t), 'a');
        check_equal_value (at (front, two, t), 6.3);
        check_equal_value (at (back, zero_type(), t), 6.3);
        check_equal_value (at (back, one, t), 'a');
        check_equal_value (at (back, two, t), 4);

        auto first_and_rest = range::chop (t);
        auto last_and_rest = range::chop (back, t);
        check_equal_value (first_and_rest.first(), 4);
        check_equal_value (last_and_rest.first(), 6.3);
        check_equal_value (size (first_and_rest.rest()), two);
        check_equal_value (size (last_and_rest.rest()), two);

        auto first_and_middle = range::chop (last_and_rest.rest());
        check_equal_value (first_and_middle.first(), 4);
        check_equal_value (size (first_and_middle.rest()), one);

        auto middle_and_empty = range::chop (first_and_middle.rest());
        check_equal_value (middle_and_empty.first(), 'a');
        check_equal_value (empty (middle_and_empty.rest()), true_);

        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (view (t)))>));
        BOOST_MPL_ASSERT_NOT ((has <
            range::callable::chop_in_place (decltype (view (t)) &)>));
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

