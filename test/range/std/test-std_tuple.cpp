/*
Copyright 2013-2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_container_std_tuple
#include "utility/test/boost_unit_test.hpp"

#include "range/std/tuple.hpp"

#include <tuple>
#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "utility/test/tracked.hpp"

#include "rime/check/check_equal.hpp"
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
using range::view_once;

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

using utility::tracked;

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

    {
        std::tuple <int> t;
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int &>));
    }
    {
        std::tuple <int> const t;
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int const &>));
    }
    {
        std::tuple <int> t;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (std::move (t))), int &&>));
    }
}

BOOST_AUTO_TEST_CASE (test_std_tuple) {
    // Zero elements
    {
        std::tuple<> t;
        BOOST_CHECK (empty (t));
        BOOST_MPL_ASSERT ((always_empty <decltype (t), direction::front>));
        BOOST_MPL_ASSERT ((always_empty <decltype (t), direction::back>));
        check_equal_value (size (t), zero_type());

        BOOST_MPL_ASSERT_NOT ((has <range::callable::first (decltype (t))>));
        BOOST_MPL_ASSERT_NOT ((
            has <range::callable::at (decltype (t), zero_type)>));
        BOOST_MPL_ASSERT_NOT ((
            has <range::callable::at (decltype (t), rime::size_t <1>)>));

        BOOST_MPL_ASSERT_NOT ((has <range::callable::chop (decltype (t))>));
    }

    // One element.
    {
        std::tuple <double> const t (6.3);
        BOOST_CHECK (!empty (t));
        BOOST_MPL_ASSERT ((never_empty <decltype (t), direction::front>));
        BOOST_MPL_ASSERT ((never_empty <decltype (t), direction::back>));
        check_equal_value (size (t), one);

        BOOST_MPL_ASSERT ((has <range::callable::first (decltype (t) &)>));
        BOOST_MPL_ASSERT ((
            has <range::callable::at (decltype (t), zero_type)>));
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
        check_equal_value (first (t, front), 6.3);
        check_equal_value (first (t, back), 6.3);

        check_equal_value (at (t, zero_type()), 6.3);
        check_equal_value (at (t, zero_type(), front), 6.3);
        check_equal_value (at (t, zero_type(), back), 6.3);

        auto first_and_empty_1 = range::chop (t);
        auto first_and_empty_2 = range::chop (t, back);
        check_equal_value (first_and_empty_1.first(), 6.3);
        check_equal_value (first_and_empty_2.first(), 6.3);
        check_equal_value (empty (first_and_empty_1.rest()), true_);
        check_equal_value (empty (first_and_empty_2.rest()), true_);
        BOOST_MPL_ASSERT ((always_empty <
            decltype (first_and_empty_2.rest()), direction::front>));
        BOOST_MPL_ASSERT ((always_empty <
            decltype (first_and_empty_2.rest()), direction::back>));
    }

    // Two elements.
    {
        std::tuple <int, char> t (4, 'a');

        BOOST_CHECK (!empty (t));
        BOOST_MPL_ASSERT ((never_empty <decltype (t), direction::front>));
        BOOST_MPL_ASSERT ((never_empty <decltype (t), direction::back>));
        check_equal_value (size (t), two);

        check_equal_value (first (t, front), 4);
        check_equal_value (first (t), 4);
        check_equal_value (first (t, back), 'a');

        check_equal_value (first (drop (t)), 'a');

        BOOST_CHECK (!empty (drop (t, back)));
        BOOST_CHECK (!empty (drop (t)));
        BOOST_CHECK (empty (drop (t, two)));
        BOOST_CHECK (empty (drop (t, two, back)));

        check_equal_value (at (t, zero_type()), 4);
        check_equal_value (at (t, one), 'a');
        check_equal_value (at (t, zero_type(), front), 4);
        check_equal_value (at (t, zero_type(), back), 'a');
        check_equal_value (at (t, one, back), 4);

        auto first_and_rest = range::chop (t);
        auto last_and_rest = range::chop (t, back);
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

        check_equal_value (first (t, front), 4);
        check_equal_value (first (t), 4);
        check_equal_value (first (drop (t)), 'a');
        check_equal_value (first (drop (t, back), back), 'a');
        check_equal_value (first (t, back), 6.3);

        BOOST_CHECK (!empty (drop (t, back)));
        BOOST_CHECK (!empty (drop (t)));
        BOOST_CHECK (!empty (drop (t, two)));
        BOOST_CHECK (empty (drop (t, three)));
        BOOST_CHECK (!empty (drop (t, two, back)));
        BOOST_CHECK (empty (drop (t, three, back)));

        check_equal_value (at (t, zero_type()), 4);
        check_equal_value (at (t, one), 'a');
        check_equal_value (at (t, two), 6.3);
        check_equal_value (at (t, zero_type(), front), 4);
        check_equal_value (at (t, one, front), 'a');
        check_equal_value (at (t, two, front), 6.3);
        check_equal_value (at (t, zero_type(), back), 6.3);
        check_equal_value (at (t, one, back), 'a');
        check_equal_value (at (t, two, back), 4);

        auto first_and_rest = range::chop (t);
        auto last_and_rest = range::chop (t, back);
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

    utility::tracked_registry r;
    {
        // Use view_once: elements are returned by rvalue reference.
        std::tuple <tracked <int>, tracked <double>> t (
            tracked <int> (r, 7), tracked <double> (r, 4.5));
        RIME_CHECK_EQUAL (first (t).content(), 7);
        RIME_CHECK_EQUAL (first (t, back).content(), 4.5);

        // Check the status quo.
        r.check_counts (2, 0, 2, 0, 0, 0, 0, 2);

        auto v = view_once (std::move (t));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (v)), tracked <int> &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (v, back)), tracked <double> &&>));

        // The elements should be moved out.
        tracked <int> i = at_c <0> (v);
        BOOST_CHECK_EQUAL (i.content(), 7);
        r.check_counts (2, 0, 3, 0, 0, 0, 0, 2);

        tracked <double> d = at_c <1> (v);
        BOOST_CHECK_EQUAL (d.content(), 4.5);
        r.check_counts (2, 0, 4, 0, 0, 0, 0, 2);
    }
}

BOOST_AUTO_TEST_CASE (test_std_pair) {
    std::tuple <int, char> t (4, 'a');
    std::pair <int, char> p (4, 'a');

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour <rime::true_type, rime::true_type,
        rime::true_type, rime::false_type> (p, t);

    auto && second = at_c <1> (view_once (p));
    BOOST_MPL_ASSERT ((std::is_same <decltype (second), char &>));
    BOOST_CHECK_EQUAL (second, 'a');

    auto && second_move = at_c <1> (view_once (std::move (p)));
    BOOST_MPL_ASSERT ((std::is_same <decltype (second_move), char &&>));
    BOOST_CHECK_EQUAL (second_move, 'a');
}

BOOST_AUTO_TEST_SUITE_END()
