/*
Copyright 2014, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_unique_range
#include "utility/test/boost_unit_test.hpp"

#include "unique_range.hpp"

#include <vector>

#include "range/std.hpp"

BOOST_AUTO_TEST_SUITE(test_range_unique_range)

using range::empty;
using range::size;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

using range::never_empty;
using range::always_empty;

using range::tag_of;
using range::has;

namespace callable = range::callable;

BOOST_AUTO_TEST_CASE (vector) {
    std::vector <int> v;

    v.push_back (5);
    v.push_back (6);
    v.push_back (7);

    typedef tag_of <decltype (range::view (v)) &>::type vector_view_tag;

    // Test unique_view.

    {
        auto view = unique_view (v);

        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view)>::type,
            unique_range_tag <vector_view_tag, false>>));
        // &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) &>::type,
            unique_range_tag <vector_view_tag, false>>));
        // const &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) const &>::type,
            unique_range_tag <vector_view_tag, false>>));

        // empty.
        BOOST_MPL_ASSERT ((has <callable::empty (decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::size (decltype (view))>));

        // first.
        BOOST_MPL_ASSERT ((has <callable::first (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::back, decltype (view))>));

        // Also for lvalues.
        BOOST_MPL_ASSERT ((has <callable::first (decltype (view) &)>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::back, decltype (view) &)>));

        // drop.
        BOOST_MPL_ASSERT ((has <callable::drop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::back, decltype (view))>));

        // Not for lvalues.
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT ((range::is_homogeneous <decltype (view)>));

        BOOST_MPL_ASSERT ((has <callable::drop (
            std::size_t, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::front, std::size_t, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::back, std::size_t, decltype (view))>));

        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, std::size_t, decltype (view) &)>));

        // chop.
        BOOST_MPL_ASSERT ((has <callable::chop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::back, decltype (view))>));

        // Not for lvalues (whether const or not).
        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) const &)>));

        // Run-time.
        BOOST_CHECK (!empty (view));
        BOOST_CHECK_EQUAL (size (view), 3);
        BOOST_CHECK_EQUAL (first (view), 5);

        view = drop (std::move (view));

        BOOST_CHECK (!empty (view));
        BOOST_CHECK_EQUAL (size (view), 2);
        BOOST_CHECK_EQUAL (first (view), 6);

        // Check that you can call "empty", "size", and "first" on these all day
        // without invalidating the object.
        BOOST_CHECK (!empty (std::move (view)));
        BOOST_CHECK (!empty (std::move (view)));
        BOOST_CHECK_EQUAL (size (std::move (view)), 2);
        BOOST_CHECK_EQUAL (size (std::move (view)), 2);
        BOOST_CHECK_EQUAL (first (std::move (view)), 6);
        BOOST_CHECK_EQUAL (first (std::move (view)), 6);

        chop_in_place (view);

        BOOST_CHECK (!empty (view));
        BOOST_CHECK_EQUAL (size (view), 1);
        BOOST_CHECK_EQUAL (first (view), 7);

        auto chopped = chop (std::move (view));

        BOOST_CHECK_EQUAL (chopped.first(), 7);
        BOOST_CHECK (empty (chopped.rest()));
        BOOST_CHECK_EQUAL (size (chopped.rest()), 0);
    }

    // Test one_time_view.

    {
        auto view = one_time_view (v);

        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view)>::type,
            unique_range_tag <vector_view_tag, true>>));
        // &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) &>::type,
            unique_range_tag <vector_view_tag, true>>));
        // const &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) const &>::type,
            unique_range_tag <vector_view_tag, true>>));

        // empty.
        BOOST_MPL_ASSERT ((has <callable::empty (decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::size (decltype (view))>));

        // first.
        BOOST_MPL_ASSERT ((has <callable::first (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::back, decltype (view))>));

        // Not for lvalues.
        BOOST_MPL_ASSERT_NOT ((has <callable::first (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::first (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::first (direction::back, decltype (view) &)>));

        // drop.
        BOOST_MPL_ASSERT ((has <callable::drop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::back, decltype (view))>));

        // Not for lvalues.
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT ((range::is_homogeneous <decltype (view)>));

        BOOST_MPL_ASSERT ((has <callable::drop (
            std::size_t, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::front, std::size_t, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::back, std::size_t, decltype (view))>));

        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, std::size_t, decltype (view) &)>));

        // chop.
        BOOST_MPL_ASSERT ((has <callable::chop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::back, decltype (view))>));

        // Not for lvalues (whether const or not).
        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) const &)>));

        // Run-time.
        BOOST_CHECK (!empty (view));
        BOOST_CHECK_EQUAL (size (view), 3);

        auto element = chop_in_place (view);

        BOOST_CHECK (!empty (view));
        BOOST_CHECK_EQUAL (size (view), 2);
        BOOST_CHECK_EQUAL (element, 5);

        auto chopped = chop (std::move (view));

        BOOST_CHECK_EQUAL (chopped.first(), 6);
        BOOST_CHECK (!empty (chopped.rest()));
        BOOST_CHECK_EQUAL (size (chopped.rest()), 1);

        // You can only call "first" by moving.
        BOOST_CHECK_EQUAL (first (chopped.move_rest()), 7);
        // Calling "first" again causes an assertion.
        // first (chopped.move_rest());
    }
}

BOOST_AUTO_TEST_CASE (tuple) {
    std::tuple <int, int> v (5, 6);

    typedef tag_of <decltype (range::view (v)) &>::type tuple_view_tag;

    // Test unique_view.

    {
        auto view = unique_view (v);

        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view)>::type,
            unique_range_tag <tuple_view_tag, false>>));
        // &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) &>::type,
            unique_range_tag <tuple_view_tag, false>>));
        // const &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) const &>::type,
            unique_range_tag <tuple_view_tag, false>>));

        // empty.
        BOOST_MPL_ASSERT ((has <callable::empty (decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::size (decltype (view))>));

        // first.
        BOOST_MPL_ASSERT ((has <callable::first (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::back, decltype (view))>));

        // Also for lvalues.
        BOOST_MPL_ASSERT ((has <callable::first (decltype (view) &)>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::back, decltype (view) &)>));

        // drop.
        BOOST_MPL_ASSERT ((has <callable::drop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::back, decltype (view))>));

        // Not for lvalues.
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <decltype (view)>));

        BOOST_MPL_ASSERT ((has <callable::drop (
            rime::size_t <2>, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::front, rime::size_t <2>, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::back, rime::size_t <2>, decltype (view))>));

        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, std::size_t, decltype (view) &)>));

        // chop.
        BOOST_MPL_ASSERT ((has <callable::chop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::back, decltype (view))>));

        // Not for lvalues (whether const or not).
        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) const &)>));

        // Run-time.
        BOOST_MPL_ASSERT ((never_empty <direction::front, decltype (view)>));
        BOOST_CHECK_EQUAL (size (view), 2);
        BOOST_CHECK_EQUAL (first (view), 5);

        auto view2 = drop (std::move (view));

        BOOST_MPL_ASSERT ((never_empty <direction::front, decltype (view2)>));
        BOOST_CHECK_EQUAL (size (view2), 1);
        BOOST_CHECK_EQUAL (first (view2), 6);

        auto chopped = chop (std::move (view2));

        BOOST_CHECK_EQUAL (chopped.first(), 6);
        BOOST_MPL_ASSERT ((
            always_empty <direction::front, decltype (chopped.rest())>));
        BOOST_CHECK_EQUAL (size (chopped.rest()), 0);
    }

    // Test one_time_view.

    {
        auto view = one_time_view (v);

        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view)>::type,
            unique_range_tag <tuple_view_tag, true>>));
        // &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) &>::type,
            unique_range_tag <tuple_view_tag, true>>));
        // const &.
        BOOST_MPL_ASSERT ((std::is_same <
            tag_of <decltype (view) const &>::type,
            unique_range_tag <tuple_view_tag, true>>));

        // empty.
        BOOST_MPL_ASSERT ((has <callable::empty (decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::size (decltype (view))>));

        // first.
        BOOST_MPL_ASSERT ((has <callable::first (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::first (direction::back, decltype (view))>));

        // Not for lvalues.
        BOOST_MPL_ASSERT_NOT ((has <callable::first (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::first (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::first (direction::back, decltype (view) &)>));

        // drop.
        BOOST_MPL_ASSERT ((has <callable::drop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::drop (direction::back, decltype (view))>));

        // Not for lvalues.
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT_NOT ((range::is_homogeneous <decltype (view)>));

        BOOST_MPL_ASSERT ((has <callable::drop (
            rime::size_t <2>, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::front, rime::size_t <2>, decltype (view))>));
        BOOST_MPL_ASSERT ((has <callable::drop (
            direction::back, rime::size_t <2>, decltype (view))>));

        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::front, std::size_t, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <callable::drop (
            direction::back, std::size_t, decltype (view) &)>));

        // chop.
        BOOST_MPL_ASSERT ((has <callable::chop (decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::front, decltype (view))>));
        BOOST_MPL_ASSERT ((has <
            callable::chop (direction::back, decltype (view))>));

        // Not for lvalues (whether const or not).
        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) &)>));

        BOOST_MPL_ASSERT_NOT ((has <callable::chop (decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::front, decltype (view) const &)>));
        BOOST_MPL_ASSERT_NOT ((has <
            callable::chop (direction::back, decltype (view) const &)>));

        // Run-time.
        BOOST_MPL_ASSERT ((never_empty <direction::front, decltype (view)>));
        BOOST_CHECK_EQUAL (size (view), 2);

        auto chopped1 = chop (std::move (view));

        BOOST_CHECK_EQUAL (chopped1.first(), 5);
        BOOST_MPL_ASSERT ((
            never_empty <direction::front, decltype (chopped1.rest())>));
        BOOST_CHECK_EQUAL (size (chopped1.rest()), 1);

        auto chopped2 = chop (chopped1.move_rest());

        BOOST_CHECK_EQUAL (chopped2.first(), 6);
        BOOST_MPL_ASSERT ((
            always_empty <direction::front, decltype (chopped2.rest())>));
        BOOST_CHECK_EQUAL (size (chopped2.rest()), 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()
