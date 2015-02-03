/*
Copyright 2012-2015 Rogier van Dalen.

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
This tests range/member_view.hpp.
In the process, this also tests aspects of range/core.hpp.
It is therefore quite thorough.
*/

#define BOOST_TEST_MODULE test_range_member_view
#include "utility/test/boost_unit_test.hpp"

#include "range/member_view.hpp"

#include <type_traits>
#include <string>

#include "meta/vector.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_member_view)

struct structure {
    int i;
    double d;
    char const c;

    structure (char c) : c (c) {}

    std::string get_string() const { return "hello"; }
    char get_char() { return c; }
};

int & get_int (structure & s) { return s.i; }

double get_double (structure const & s) { return s.d; }

double && move_double (structure && s) { return std::move (s.d); }

BOOST_AUTO_TEST_CASE (test_range_member_view) {
    using range::view;
    using range::empty;
    using range::size;
    using range::first;
    using range::drop;
    using range::chop;
    using range::chop_in_place;
    using range::at;
    using range::at_c;
    using range::second;
    using range::third;
    using range::front;
    using range::back;

    structure s ('a');
    s.i = 4;
    s.d = 3.5;

    structure s2 (char (234));
    s2.i = 123;
    s2.d = 432.1;

    typedef range::member_extractor <int structure::*, &structure::i>
        member_i;
    typedef range::member_extractor <double structure::*, &structure::d>
        member_d;
    typedef range::member_extractor <char const (structure::*), &structure::c>
        member_c;

    {
        typedef range::member_view <structure &, meta::vector<>>
            empty_view_type;
        empty_view_type empty_view (s);

        static_assert (
            !std::is_convertible <structure &, empty_view_type>::value,
            "member_view should be only explicitly is_convertible.");

        static_assert (std::is_same <typename range::result_of <
            range::callable::default_direction (empty_view_type)>::type,
            direction::front>::value, "");
        auto d = range::default_direction (empty_view);
        static_assert (std::is_same <decltype (d), direction::front>::value,
            "");

        static_assert (!range::is_homogeneous <empty_view_type>::value, "");
        static_assert (!range::is_homogeneous <
            direction::back, empty_view_type>::value, "");

        // view.
        static_assert (range::has <range::callable::view (
            empty_view_type const)>::value, "");
        static_assert (!range::has <range::callable::view (
            int, empty_view_type const)>::value, "");
        static_assert (range::has <range::callable::view (
            direction::front, empty_view_type &)>::value, "");
        static_assert (range::has <range::callable::view (
            direction::back, empty_view_type const &)>::value, "");
        static_assert (range::has <range::callable::view (
                direction::back &, direction::front const,
                empty_view_type &)>::value,
            "");

        // view_once.
        static_assert (range::has <range::callable::view_once (
            empty_view_type const)>::value, "");
        static_assert (!range::has <range::callable::view_once (
            int, empty_view_type const)>::value, "");
        static_assert (range::has <range::callable::view_once (
            direction::front, empty_view_type &)>::value, "");
        static_assert (range::has <range::callable::view_once (
            direction::back, empty_view_type const &)>::value, "");
        static_assert (range::has <range::callable::view_once (
                direction::back &, direction::front const,
                empty_view_type &)>::value,
            "");

        // empty.
        static_assert (range::has <range::callable::empty (
            empty_view_type const)>::value, "");
        static_assert (range::has <range::callable::empty (
            direction::front, empty_view_type &)>::value, "");
        static_assert (range::has <range::callable::empty (
            direction::back, empty_view_type const &)>::value, "");

        // size.
        static_assert (range::has <range::callable::size (
            empty_view_type const)>::value, "");
        static_assert (range::has <range::callable::size (
            direction::front &, empty_view_type const &)>::value, "");
        static_assert (range::has <range::callable::size (
            direction::back const &, empty_view_type)>::value, "");

        // empty.
        RIME_CHECK_EQUAL (empty (empty_view_type (s)), rime::true_);
        RIME_CHECK_EQUAL (empty (view (front, empty_view_type (s))),
            rime::true_);
        RIME_CHECK_EQUAL (empty (front, empty_view), rime::true_);
        RIME_CHECK_EQUAL (empty (direction::back(), empty_view), rime::true_);
        BOOST_MPL_ASSERT ((
            range::always_empty <direction::front, empty_view_type>));
        BOOST_MPL_ASSERT ((
            range::always_empty <direction::back, empty_view_type>));

        // size.
        RIME_CHECK_EQUAL (size (empty_view), rime::size_t <0>());
        RIME_CHECK_EQUAL (size (front, empty_view), rime::size_t <0>());
        RIME_CHECK_EQUAL (size (back, empty_view_type (s)), rime::size_t <0>());

        // first.
        static_assert (!range::has <range::callable::first (
            empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::first (
            direction::front, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::first (
            direction::back, empty_view_type)>::value, "");

        // drop.
        static_assert (!range::has <range::callable::drop (
            empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::front, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, empty_view_type)>::value, "");

        // at.
        static_assert (!range::has <range::callable::at (
            empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::at (
            direction::front, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::at (
            direction::back, empty_view_type)>::value, "");

        // drop of size 0 is available.
        static_assert (range::has <range::callable::drop (
            rime::size_t <0>, empty_view_type)>::value, "");
        static_assert (range::has <range::callable::drop (
            direction::front, std::integral_constant <std::size_t, 0>,
            empty_view_type)>::value, "");
        static_assert (range::has <range::callable::drop (
            direction::back, rime::size_t <0>, empty_view_type)>::value, "");

        static_assert (!range::has <range::callable::drop (
            rime::size_t <1>, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::front, rime::size_t <1>, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, rime::size_t <1>, empty_view_type)>::value, "");

        static_assert (!range::has <range::callable::drop (
            rime::size_t <2>, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::front, std::integral_constant <std::size_t, 3>,
            empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, rime::size_t <3>, empty_view_type)>::value, "");

        // chop.
        static_assert (!range::has <range::callable::chop (
            empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop (
            direction::front, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop (
            direction::back, empty_view_type)>::value, "");

        // chop_in_place: not a homogeneous range.
        static_assert (!range::has <range::callable::chop_in_place (
            empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop_in_place (
            direction::front, empty_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop_in_place (
            direction::back, empty_view_type)>::value, "");

        // View must be assignable.
        empty_view = empty_view_type (s2);
        RIME_CHECK_EQUAL (empty (empty_view), rime::true_);
        // Original container, s, has not changed.
        BOOST_CHECK_EQUAL (s.i, 4);
    }

    {
        typedef range::member_view <structure const &, meta::vector <member_i>>
            int_view_type;
        typedef range::member_view <structure const &, meta::vector<>>
            empty_view_type;
        int_view_type int_view (s);

        // empty.
        static_assert (range::has <range::callable::empty (
            int_view_type)>::value, "");
        static_assert (range::has <range::callable::empty (
            direction::front, int_view_type)>::value, "");
        RIME_CHECK_EQUAL (range::empty (int_view), rime::false_);
        RIME_CHECK_EQUAL (range::empty (front, int_view), rime::false_);
        BOOST_MPL_ASSERT ((
            range::never_empty <direction::front, int_view_type>));
        BOOST_MPL_ASSERT ((
            range::never_empty <direction::back, int_view_type>));

        // size.
        static_assert (range::has <range::callable::size (
            int_view_type)>::value, "");
        static_assert (range::has <range::callable::size (
            direction::back, int_view_type)>::value, "");
        BOOST_MPL_ASSERT ((std::is_same <range::result_of <
            range::callable::size (int_view_type)>::type, rime::size_t <1>>));
        BOOST_MPL_ASSERT ((std::is_same < range::result_of <
            range::callable::size (direction::back, int_view_type)>::type,
            rime::size_t <1>>));

        // first.
        static_assert (range::has <range::callable::first (
            int_view_type)>::value, "");
        static_assert (range::has <range::callable::first (
            direction::front, int_view_type)>::value, "");
        static_assert (range::has <range::callable::first (
            direction::back, int_view_type)>::value, "");
        BOOST_MPL_ASSERT ((std::is_same <
            range::result_of <range::callable::first (
                int_view_type const)>::type, int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            range::result_of <range::callable::first (
                direction::back, int_view_type)>::type,
            int const &>));

        static_assert (!range::is_homogeneous <int_view_type>::value, "");
        static_assert (!range::is_homogeneous <
            direction::back, int_view_type>::value, "");

        // drop.
        static_assert (range::has <range::callable::drop (
            int_view_type)>::value, "");
        static_assert (range::has <range::callable::drop (
            direction::front, rime::int_<1>, int_view_type)>::value, "");
        BOOST_MPL_ASSERT ((std::is_same <
            range::result_of <range::callable::drop (
                int_view_type)>::type, empty_view_type>));
        BOOST_MPL_ASSERT ((std::is_same <
            range::result_of <range::callable::drop (
                direction::back, int_view_type)>::type,
            empty_view_type>));

        static_assert (!range::has <range::callable::drop (
            direction::back, rime::int_<2>, int_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, int, int_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, std::size_t, int_view_type)>::value, "");

        // chop.
        static_assert (range::has <range::callable::chop (
            int_view_type)>::value, "");
        static_assert (range::has <range::callable::chop (
            direction::front, int_view_type)>::value, "");
        static_assert (range::has <range::callable::chop (
            direction::back, int_view_type)>::value, "");
        BOOST_MPL_ASSERT ((std::is_same <range::result_of <
            range::callable::chop (int_view_type)>::type,
            range::chopped <int const &, empty_view_type>>));
        BOOST_MPL_ASSERT ((std::is_same <range::result_of <
            range::callable::chop (direction::front, int_view_type)>::type,
            range::chopped <int const &, empty_view_type>>));
        BOOST_MPL_ASSERT ((std::is_same <range::result_of <
            range::callable::chop (direction::back, int_view_type)>::type,
            range::chopped <int const &, empty_view_type>>));

        // chop_in_place: not a homogeneous range.
        static_assert (!range::has <range::callable::chop_in_place (
            int_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop_in_place (
            direction::front, int_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop_in_place (
            direction::back, int_view_type)>::value, "");

        // at: the implementation is based on drop and first.
        static_assert (range::has <range::callable::at (
            rime::size_t <0>, int_view_type)>::value, "");
        static_assert (range::has <range::callable::at (
            direction::front, rime::size_t <0>, int_view_type)>::value, "");
        static_assert (range::has <range::callable::at (
            direction::back, rime::size_t <0>, int_view_type)>::value, "");

        static_assert (!range::has <range::callable::at (
            rime::size_t <1>, int_view_type)>::value, "");
        static_assert (!range::has <range::callable::at (
            direction::front, rime::size_t <1>, int_view_type)>::value, "");
        static_assert (!range::has <range::callable::at (
            direction::back, rime::size_t <1>, int_view_type)>::value, "");

        BOOST_MPL_ASSERT ((std::is_same <
            range::result_of <range::callable::at (
                rime::size_t <0>, int_view_type const)>::type,
            int const &>));
        BOOST_MPL_ASSERT ((std::is_same <
            range::result_of <range::callable::at (
                direction::back, rime::size_t <0>, int_view_type)>::type,
            int const &>));

        BOOST_CHECK_EQUAL (at (rime::size_t <0>(), int_view), 4);
        BOOST_CHECK_EQUAL (at (back, rime::size_t <0>(), int_view), 4);
        BOOST_CHECK_EQUAL (at_c <0> (int_view), 4);
        BOOST_CHECK_EQUAL (at_c <0> (back, int_view), 4);

        // Run-time behaviour.
        auto empty_1 = drop (int_view);
        auto empty_2 = drop (back, int_view);
        auto first_and_empty_1 = chop (int_view);
        auto first_and_empty_2 = chop (back, int_view);

        RIME_CHECK_EQUAL (empty (int_view), rime::false_);
        RIME_CHECK_EQUAL (empty (front, int_view), rime::false_);
        RIME_CHECK_EQUAL (empty (back, view (front, int_view)), rime::false_);

        RIME_CHECK_EQUAL (size (int_view), rime::size_t <1>());
        RIME_CHECK_EQUAL (size (front, int_view), rime::size_t <1>());
        RIME_CHECK_EQUAL (size (back, int_view), rime::size_t <1>());

        BOOST_CHECK_EQUAL (first (int_view), 4);
        BOOST_CHECK_EQUAL (first (back, int_view), 4);
        BOOST_CHECK_EQUAL (first_and_empty_1.first(), 4);
        BOOST_CHECK_EQUAL (first_and_empty_2.first(), 4);

        RIME_CHECK_EQUAL (empty (empty_1), rime::true_);
        RIME_CHECK_EQUAL (empty (front, empty_1), rime::true_);
        RIME_CHECK_EQUAL (size (empty_1), rime::size_t <0>());
        RIME_CHECK_EQUAL (size (back, empty_1), rime::size_t <0>());

        RIME_CHECK_EQUAL (empty (empty_2), rime::true_);
        RIME_CHECK_EQUAL (empty (back, empty_2), rime::true_);
        RIME_CHECK_EQUAL (size (empty_2), rime::size_t <0>());
        RIME_CHECK_EQUAL (size (front, empty_2), rime::size_t <0>());

        // chopped ranges.
        RIME_CHECK_EQUAL (empty (first_and_empty_1.rest()), rime::true_);
        RIME_CHECK_EQUAL (empty (front, first_and_empty_1.rest()), rime::true_);
        RIME_CHECK_EQUAL (size (first_and_empty_1.rest()), rime::size_t <0>());
        RIME_CHECK_EQUAL (size (back, first_and_empty_1.rest()),
            rime::size_t <0>());

        RIME_CHECK_EQUAL (empty (first_and_empty_2.rest()), rime::true_);
        RIME_CHECK_EQUAL (empty (back, first_and_empty_2.rest()), rime::true_);
        RIME_CHECK_EQUAL (size (first_and_empty_2.rest()), rime::size_t <0>());
        RIME_CHECK_EQUAL (size (front, first_and_empty_2.rest()),
            rime::size_t <0>());

        // View must be assignable.
        int_view = int_view_type (s2);
        RIME_CHECK_EQUAL (empty (int_view), rime::false_);
        // Original container, s, has not changed.
        BOOST_CHECK_EQUAL (s.i, 4);
        BOOST_CHECK_EQUAL (first (int_view), 123);
        BOOST_CHECK_EQUAL (at (rime::size_t <0>(), int_view), 123);
    }
    {
        typedef range::member_view <structure &,
            meta::vector <member_i, member_d, member_c>> three_view_type;
        three_view_type three_view (s);

        static_assert (!range::is_homogeneous <three_view_type>::value, "");
        static_assert (!range::is_homogeneous <
            direction::back, three_view_type &>::value, "");
        static_assert (!range::is_homogeneous <
            direction::front, three_view_type const>::value, "");
        static_assert (!range::is_homogeneous <
            direction::back, direction::front, three_view_type const>::value,
            "");

        static_assert (range::has <range::callable::empty (
            three_view_type)>::value, "");
        static_assert (range::has <range::callable::empty (
            direction::front, three_view_type)>::value, "");

        static_assert (range::has <range::callable::size (
            three_view_type)>::value, "");
        static_assert (range::has <range::callable::size (
            direction::back, three_view_type)>::value, "");

        static_assert (range::has <range::callable::first (
            three_view_type)>::value, "");
        static_assert (range::has <range::callable::first (
            direction::front, three_view_type)>::value, "");
        static_assert (range::has <range::callable::first (
            direction::back, three_view_type)>::value, "");

        static_assert (range::has <range::callable::drop (
            three_view_type)>::value, "");
        static_assert (range::has <range::callable::drop (
            direction::front, rime::int_<1>, three_view_type)>::value, "");
        static_assert (range::has <range::callable::drop (
            direction::back, boost::mpl::int_<2>, three_view_type)>::value, "");
        static_assert (range::has <range::callable::drop (
            direction::back, std::integral_constant <std::size_t, 3>,
            three_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, rime::int_<4>, three_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, int, three_view_type)>::value, "");
        static_assert (!range::has <range::callable::drop (
            direction::back, std::size_t, three_view_type)>::value, "");

        static_assert (range::has <range::callable::chop (
            three_view_type)>::value, "");
        static_assert (range::has <range::callable::chop (
            direction::front, three_view_type)>::value, "");
        static_assert (range::has <range::callable::chop (
            direction::back, three_view_type)>::value, "");

        static_assert (!range::has <range::callable::chop_in_place (
            three_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop_in_place (
            direction::front, three_view_type)>::value, "");
        static_assert (!range::has <range::callable::chop_in_place (
            direction::back, three_view_type)>::value, "");

        RIME_CHECK_EQUAL (empty (three_view), rime::false_);
        RIME_CHECK_EQUAL (empty (front, three_view), rime::false_);
        RIME_CHECK_EQUAL (empty (back, three_view), rime::false_);
        BOOST_MPL_ASSERT ((
            range::never_empty <direction::front, three_view_type>));
        BOOST_MPL_ASSERT ((
            range::never_empty <direction::back, three_view_type>));

        RIME_CHECK_EQUAL (size (three_view), rime::size_t <3>());
        RIME_CHECK_EQUAL (size (front, three_view), rime::size_t <3>());
        RIME_CHECK_EQUAL (size (back, three_view), rime::size_t <3>());

        BOOST_CHECK_EQUAL (first (three_view), 4);
        BOOST_CHECK_EQUAL (first (back, three_view), 'a');
        BOOST_CHECK_EQUAL (first (drop (back, three_view)), 4);
        BOOST_CHECK_EQUAL (first (drop (back, rime::int_<2>(), three_view)), 4);
        BOOST_CHECK_EQUAL (first (back, drop (front, three_view)), 'a');

        RIME_CHECK_EQUAL (chop (three_view).first(), 4);
        RIME_CHECK_EQUAL (chop (back, three_view).first(), 'a');

        // Access second element.
        RIME_CHECK_EQUAL (empty (drop (three_view)), rime::false_);
        RIME_CHECK_EQUAL (empty (drop (front, three_view)), rime::false_);
        RIME_CHECK_EQUAL (empty (drop (back, view (front, back, three_view))),
            rime::false_);

        RIME_CHECK_EQUAL (size (drop (three_view)), rime::size_t <2>());
        RIME_CHECK_EQUAL (size (front, drop (three_view)), rime::size_t <2>());
        RIME_CHECK_EQUAL (size (back, drop (three_view)), rime::size_t <2>());
        RIME_CHECK_EQUAL (size (drop (front, three_view)), rime::size_t <2>());
        RIME_CHECK_EQUAL (size (front, drop (front, three_view)),
            rime::size_t <2>());
        RIME_CHECK_EQUAL (size (back, drop (front, three_view)),
            rime::size_t <2>());

        BOOST_CHECK_EQUAL (first (drop (three_view)), 3.5);
        BOOST_CHECK_EQUAL (first (drop (front, three_view)), 3.5);
        BOOST_CHECK_EQUAL (
            first (drop (std::integral_constant <int, 1>(), three_view)), 3.5);
        BOOST_CHECK_EQUAL (first (drop (front, rime::int_ <1>(), three_view)),
            3.5);

        RIME_CHECK_EQUAL (size (chop (three_view).rest()), rime::size_t <2>());
        BOOST_CHECK_EQUAL (first (chop (three_view).rest()), 3.5);
        RIME_CHECK_EQUAL (size (chop (back, three_view).rest()),
            rime::size_t <2>());
        BOOST_CHECK_EQUAL (first (back, chop (back, three_view).rest()), 3.5);

        // Access second element of the three_view reduced to two elements
        RIME_CHECK_EQUAL (empty (drop (drop (back, three_view))), rime::false_);
        BOOST_CHECK_EQUAL (first (drop (drop (back, three_view))), 3.5);
        BOOST_CHECK_EQUAL (first (drop (front, drop (back, three_view))), 3.5);
        BOOST_CHECK_EQUAL (first (drop (rime::int_ <1>(),
            drop (back, three_view))), 3.5);
        BOOST_CHECK_EQUAL (first (drop (front, rime::int_ <1>(),
            drop (back, three_view))), 3.5);

        // Third element.
        RIME_CHECK_EQUAL (empty (drop (drop (three_view))), rime::false_);
        BOOST_CHECK_EQUAL (first (drop (drop (three_view))), 'a');
        BOOST_CHECK_EQUAL (first (drop (front, drop (three_view))), 'a');
        BOOST_CHECK_EQUAL (first (drop (rime::int_ <2>(), three_view)), 'a');
        BOOST_CHECK_EQUAL (first (drop (front, rime::int_ <2>(), three_view)),
            'a');

        // at (d,n,r) is a shorthand for first (d, drop (d, n, r)).
        static_assert (range::has <range::callable::at (
            rime::size_t <0>, three_view_type)>::value, "");
        static_assert (range::has <range::callable::at (
            rime::size_t <1>, three_view_type)>::value, "");
        static_assert (range::has <range::callable::at (
            rime::size_t <2>, three_view_type)>::value, "");
        static_assert (!range::has <range::callable::at (
            rime::size_t <3>, three_view_type)>::value, "");

        BOOST_CHECK_EQUAL (at (rime::size_t <0>(), three_view), 4);
        BOOST_CHECK_EQUAL (at (rime::size_t <1>(), three_view), 3.5);
        BOOST_CHECK_EQUAL (at (rime::size_t <2>(), three_view), 'a');
        BOOST_CHECK_EQUAL (at (front, rime::size_t <0>(), three_view), 4);
        BOOST_CHECK_EQUAL (
            at (front, std::integral_constant <size_t, 1>(), three_view), 3.5);
        BOOST_CHECK_EQUAL (at (front, rime::size_t <2>(), three_view), 'a');
        BOOST_CHECK_EQUAL (at (back, rime::size_t <0>(), three_view), 'a');
        BOOST_CHECK_EQUAL (at (back, rime::size_t <1>(), three_view), 3.5);
        BOOST_CHECK_EQUAL (at (back, boost::mpl::size_t <2>(), three_view), 4);

        BOOST_CHECK_EQUAL (at_c <0> (three_view), 4);
        BOOST_CHECK_EQUAL (at_c <1> (three_view), 3.5);
        BOOST_CHECK_EQUAL (at_c <2> (three_view), 'a');
        BOOST_CHECK_EQUAL (at_c <0> (back, three_view), 'a');
        BOOST_CHECK_EQUAL (at_c <1> (back, three_view), 3.5);
        BOOST_CHECK_EQUAL (at_c <2> (back, three_view), 4);

        BOOST_CHECK_EQUAL (second (three_view), 3.5);
        BOOST_CHECK_EQUAL (third (three_view), 'a');
        BOOST_CHECK_EQUAL (second (back, three_view), 3.5);
        BOOST_CHECK_EQUAL (third (back, three_view), 4);

        rime::int_<2> two;
        // Three drops from whichever direction and the range is empty.
        RIME_CHECK_EQUAL (empty (drop (drop (drop (three_view)))), rime::true_);
        RIME_CHECK_EQUAL (empty (drop (drop (back, drop (three_view)))),
            rime::true_);
        RIME_CHECK_EQUAL (empty (drop (back, drop (drop (three_view)))),
            rime::true_);
        RIME_CHECK_EQUAL (empty (drop (drop (rime::int_<2>(),
            view (back, three_view)))), rime::true_);
        RIME_CHECK_EQUAL (empty (drop (drop (back, two, three_view))),
            rime::true_);
        RIME_CHECK_EQUAL (empty (drop (back, drop (two, three_view))),
            rime::true_);
        RIME_CHECK_EQUAL (empty (drop (back, rime::int_<2>(),
            drop (three_view))), rime::true_);

        first (three_view) = 6;
        BOOST_CHECK_EQUAL (s.i, 6);
        first (drop (three_view)) = 98.7;
        BOOST_CHECK_EQUAL (s.d, 98.7);

        // View must be assignable.
        three_view = three_view_type (s2);
        RIME_CHECK_EQUAL (empty (three_view), rime::false_);
        // Original container, s, has not changed.
        BOOST_CHECK_EQUAL (s.i, 6);
        BOOST_CHECK_EQUAL (first (three_view), 123);
        BOOST_CHECK_EQUAL (first (drop (three_view)), 432.1);
        BOOST_CHECK_EQUAL (first (back, three_view), char (234));
    }
    // Rvalue reference to the structure.
    {
        typedef range::member_view <structure &&,
            meta::vector <member_i, member_d, member_c>> three_view_type;
        three_view_type three_view (std::move (s));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (three_view)), int &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <1> (three_view)), double &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (second (three_view)), double &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (at_c <2> (three_view)), char const &&>));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (third (three_view)), char const &&>));

        BOOST_CHECK_EQUAL (first (three_view), 6);
        BOOST_CHECK_EQUAL (at_c <1> (three_view), 98.7);
        BOOST_CHECK_EQUAL (at_c <2> (three_view), 'a');
    }
    {
        // Check that member_view can be constructed only from member_view's
        // with a superset of extractors.
        typedef range::member_view <structure &, meta::vector<>>
            empty_view_type;
        typedef range::member_view <structure &, meta::vector <member_i>>
            int_view_type;
        typedef range::member_view <structure &,
            meta::vector <member_i, member_d, member_c>> three_view_type;

        BOOST_MPL_ASSERT ((
            std::is_convertible <int_view_type, empty_view_type>));
        BOOST_MPL_ASSERT ((
            std::is_convertible <three_view_type, empty_view_type>));
        BOOST_MPL_ASSERT ((
            std::is_convertible <three_view_type, int_view_type>));

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <empty_view_type, int_view_type>));
        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <empty_view_type, three_view_type>));
        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <int_view_type, three_view_type>));
    }
}

BOOST_AUTO_TEST_CASE (test_range_functions) {
    using range::view;
    using range::empty;
    using range::size;
    using range::first;
    using range::at_c;
    using range::drop;
    using range::front;
    using range::back;

    typedef range::member_extractor <int structure::*, &structure::i>
        member_i;
    typedef range::member_extractor <std::string (structure::*)() const,
        &structure::get_string> member_s;
    typedef range::member_extractor <char (structure::*)(),
        &structure::get_char> member_c;
    typedef range::member_extractor <int & (*) (structure &),
        &get_int> member_i_2;
    typedef range::member_extractor <double (*) (structure const &),
        &get_double> member_d_2;

    structure s ('b');
    s.i = 678;
    s.d = 890.1;

    {
        typedef range::member_view <structure &,
                meta::vector <member_i, member_s, member_c, member_i_2,
                member_d_2>>
            view_type;
        view_type structure_view (s);

        BOOST_CHECK_EQUAL (first (structure_view), 678);
        BOOST_CHECK_EQUAL (at_c <1> (structure_view), "hello");
        BOOST_CHECK_EQUAL (at_c <2> (structure_view), 'b');
        BOOST_CHECK_EQUAL (at_c <3> (structure_view), 678);
        at_c <3>(structure_view) = 3;
        BOOST_CHECK_EQUAL (s.i, 3);
        BOOST_CHECK_EQUAL (at_c <4> (structure_view), 890.1);
    }

    {
        // Rvalue reference.
        typedef range::member_extractor <double && (*) (structure &&),
            &move_double> member_d_move;
        typedef range::member_view <structure &&,
            meta::vector <member_d_move, member_s>> view_type;
        view_type structure_view (std::move (s));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (first (structure_view)), double &&>));
        BOOST_CHECK_EQUAL (first (structure_view), 890.1);
        // Use nonconst function.
        BOOST_CHECK_EQUAL (at_c <1> (structure_view), "hello");
    }
}

BOOST_AUTO_TEST_SUITE_END()
