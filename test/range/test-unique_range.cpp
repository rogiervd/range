// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_unique_range
#include "boost/test/unit_test.hpp"

#include "unique_range.hpp"

#include <vector>

#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"

BOOST_AUTO_TEST_SUITE(test_range_unique_range)

using range::chop;
using range::chop_in_place;
using range::drop;
using range::empty;
using range::first;
using range::size;

using range::always_empty;
using range::never_empty;

using range::has;
using range::tag_of;

namespace callable = range::callable;

BOOST_AUTO_TEST_CASE(vector)
{
    std::vector<int> v;

    v.push_back(5);
    v.push_back(6);
    v.push_back(7);

    // Test unique_view.
    {
        auto view = unique_view(v);

        static_assert(std::is_same_v<
                      tag_of<decltype(view)>::type, unique_range_tag<false>>);
        // &.
        static_assert(std::is_same_v<
                      tag_of<decltype(view) &>::type, unique_range_tag<false>>);
        // const &.
        static_assert(
            std::is_same_v<
                tag_of<decltype(view) const &>::type, unique_range_tag<false>>);

        // empty.
        static_assert(has<callable::empty(decltype(view))>::value);
        static_assert(has<callable::size(decltype(view))>::value);

        // first.
        static_assert(has<callable::first(decltype(view))>::value);
        static_assert(
            has<callable::first(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::first(decltype(view), direction::back)>::value);

        // Also for lvalues.
        static_assert(has<callable::first(decltype(view) &)>::value);
        static_assert(
            has<callable::first(decltype(view) &, direction::front)>::value);
        static_assert(
            has<callable::first(decltype(view) &, direction::back)>::value);

        // drop.
        static_assert(has<callable::drop(decltype(view))>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::back)>::value);

        // Not for lvalues.
        static_assert(!has<callable::drop(decltype(view) &)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::back)>::value);

        static_assert(range::is_homogeneous<decltype(view)>::value);

        static_assert(has<callable::drop(decltype(view), std::size_t)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), std::size_t, direction::front)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), std::size_t, direction::back)>::value);

        static_assert(
            !has<callable::drop(decltype(view) &, std::size_t)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::front)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::back)>::value);

        // chop.
        static_assert(has<callable::chop(decltype(view))>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::back)>::value);

        // Not for lvalues (whether const or not).
        static_assert(!has<callable::chop(decltype(view) &)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::back)>::value);

        static_assert(!has<callable::chop(decltype(view) const &)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::front)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::back)>::value);

        // Run-time.
        BOOST_CHECK(!empty(view));
        BOOST_CHECK_EQUAL(size(view), 3u);
        BOOST_CHECK_EQUAL(first(view), 5);

        view = drop(std::move(view));

        BOOST_CHECK(!empty(view));
        BOOST_CHECK_EQUAL(size(view), 2u);
        BOOST_CHECK_EQUAL(first(view), 6);

        // Check that you can call "empty", "size", and "first" on these all day
        // without invalidating the object.
        BOOST_CHECK(!empty(std::move(view)));
        BOOST_CHECK(!empty(std::move(view)));
        BOOST_CHECK_EQUAL(size(std::move(view)), 2u);
        BOOST_CHECK_EQUAL(size(std::move(view)), 2u);
        BOOST_CHECK_EQUAL(first(std::move(view)), 6);
        BOOST_CHECK_EQUAL(first(std::move(view)), 6);

        chop_in_place(view);

        BOOST_CHECK(!empty(view));
        BOOST_CHECK_EQUAL(size(view), 1u);
        BOOST_CHECK_EQUAL(first(view), 7);

        auto chopped = chop(std::move(view));

        BOOST_CHECK_EQUAL(chopped.first(), 7);
        BOOST_CHECK(empty(chopped.rest()));
        BOOST_CHECK_EQUAL(size(chopped.rest()), 0u);
    }

    // Test one_time_view.

    {
        auto view = one_time_view(v);

        static_assert(std::is_same_v<
                      tag_of<decltype(view)>::type, unique_range_tag<true>>);
        // &.
        static_assert(std::is_same_v<
                      tag_of<decltype(view) &>::type, unique_range_tag<true>>);
        // const &.
        static_assert(
            std::is_same_v<
                tag_of<decltype(view) const &>::type, unique_range_tag<true>>);

        // empty.
        static_assert(has<callable::empty(decltype(view))>::value);
        static_assert(has<callable::size(decltype(view))>::value);

        // first.
        static_assert(has<callable::first(decltype(view))>::value);
        static_assert(
            has<callable::first(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::first(decltype(view), direction::back)>::value);

        // Not for lvalues.
        static_assert(!has<callable::first(decltype(view) &)>::value);
        static_assert(
            !has<callable::first(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::first(decltype(view) &, direction::back)>::value);

        // drop.
        static_assert(has<callable::drop(decltype(view))>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::back)>::value);

        // Not for lvalues.
        static_assert(!has<callable::drop(decltype(view) &)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::back)>::value);

        static_assert(range::is_homogeneous<decltype(view)>::value);

        static_assert(has<callable::drop(decltype(view), std::size_t)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), std::size_t, direction::front)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), std::size_t, direction::back)>::value);

        static_assert(
            !has<callable::drop(decltype(view) &, std::size_t)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::front)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::back)>::value);

        // chop.
        static_assert(has<callable::chop(decltype(view))>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::back)>::value);

        // Not for lvalues (whether const or not).
        static_assert(!has<callable::chop(decltype(view) &)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::back)>::value);

        static_assert(!has<callable::chop(decltype(view) const &)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::front)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::back)>::value);

        // Run-time.
        BOOST_CHECK(!empty(view));
        BOOST_CHECK_EQUAL(size(view), 3u);

        auto element = chop_in_place(view);

        BOOST_CHECK(!empty(view));
        BOOST_CHECK_EQUAL(size(view), 2u);
        BOOST_CHECK_EQUAL(element, 5);

        auto chopped = chop(std::move(view));

        BOOST_CHECK_EQUAL(chopped.first(), 6);
        BOOST_CHECK(!empty(chopped.rest()));
        BOOST_CHECK_EQUAL(size(chopped.rest()), 1u);

        // You can only call "first" by moving.
        BOOST_CHECK_EQUAL(first(chopped.move_rest()), 7);
        // Calling "first" again causes an assertion.
        // first (chopped.move_rest());
    }
}

BOOST_AUTO_TEST_CASE(tuple)
{
    std::tuple<int, int> v(5, 6);

    // Test unique_view.
    {
        auto view = unique_view(v);

        static_assert(std::is_same_v<
                      tag_of<decltype(view)>::type, unique_range_tag<false>>);
        // &.
        static_assert(std::is_same_v<
                      tag_of<decltype(view) &>::type, unique_range_tag<false>>);
        // const &.
        static_assert(
            std::is_same_v<
                tag_of<decltype(view) const &>::type, unique_range_tag<false>>);

        // empty.
        static_assert(has<callable::empty(decltype(view))>::value);
        static_assert(has<callable::size(decltype(view))>::value);

        // first.
        static_assert(has<callable::first(decltype(view))>::value);
        static_assert(
            has<callable::first(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::first(decltype(view), direction::back)>::value);

        // Also for lvalues.
        static_assert(has<callable::first(decltype(view) &)>::value);
        static_assert(
            has<callable::first(decltype(view) &, direction::front)>::value);
        static_assert(
            has<callable::first(decltype(view) &, direction::back)>::value);

        // drop.
        static_assert(has<callable::drop(decltype(view))>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::back)>::value);

        // Not for lvalues.
        static_assert(!has<callable::drop(decltype(view) &)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::back)>::value);

        static_assert(!range::is_homogeneous<decltype(view)>::value);

        static_assert(
            has<callable::drop(decltype(view), rime::size_t<2>)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), rime::size_t<2>, direction::front)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), rime::size_t<2>, direction::back)>::value);

        static_assert(
            !has<callable::drop(decltype(view) &, std::size_t)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::front)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::back)>::value);

        // chop.
        static_assert(has<callable::chop(decltype(view))>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::back)>::value);

        // Not for lvalues (whether const or not).
        static_assert(!has<callable::chop(decltype(view) &)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::back)>::value);

        static_assert(!has<callable::chop(decltype(view) const &)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::front)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::back)>::value);

        // Run-time.
        static_assert(never_empty<decltype(view), direction::front>::value);
        BOOST_CHECK_EQUAL(size(view), 2u);
        BOOST_CHECK_EQUAL(first(view), 5);

        auto view2 = drop(std::move(view));

        static_assert(never_empty<decltype(view2), direction::front>::value);
        BOOST_CHECK_EQUAL(size(view2), 1u);
        BOOST_CHECK_EQUAL(first(view2), 6);

        auto chopped = chop(std::move(view2));

        BOOST_CHECK_EQUAL(chopped.first(), 6);
        static_assert(
            always_empty<decltype(chopped.rest()), direction::front>::value);
        BOOST_CHECK_EQUAL(size(chopped.rest()), 0u);
    }

    // Test one_time_view.

    {
        auto view = one_time_view(v);

        static_assert(std::is_same_v<
                      tag_of<decltype(view)>::type, unique_range_tag<true>>);
        // &.
        static_assert(std::is_same_v<
                      tag_of<decltype(view) &>::type, unique_range_tag<true>>);
        // const &.
        static_assert(
            std::is_same_v<
                tag_of<decltype(view) const &>::type, unique_range_tag<true>>);

        // empty.
        static_assert(has<callable::empty(decltype(view))>::value);
        static_assert(has<callable::size(decltype(view))>::value);

        // first.
        static_assert(has<callable::first(decltype(view))>::value);
        static_assert(
            has<callable::first(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::first(decltype(view), direction::back)>::value);

        // Not for lvalues.
        static_assert(!has<callable::first(decltype(view) &)>::value);
        static_assert(
            !has<callable::first(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::first(decltype(view) &, direction::back)>::value);

        // drop.
        static_assert(has<callable::drop(decltype(view))>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::drop(decltype(view), direction::back)>::value);

        // Not for lvalues.
        static_assert(!has<callable::drop(decltype(view) &)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::drop(decltype(view) &, direction::back)>::value);

        static_assert(!range::is_homogeneous<decltype(view)>::value);

        static_assert(
            has<callable::drop(decltype(view), rime::size_t<2>)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), rime::size_t<2>, direction::front)>::value);
        static_assert(
            has<callable::drop(
                decltype(view), rime::size_t<2>, direction::back)>::value);

        static_assert(
            !has<callable::drop(decltype(view) &, std::size_t)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::front)>::value);
        static_assert(
            !has<callable::drop(
                decltype(view) &, std::size_t, direction::back)>::value);

        // chop.
        static_assert(has<callable::chop(decltype(view))>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::front)>::value);
        static_assert(
            has<callable::chop(decltype(view), direction::back)>::value);

        // Not for lvalues (whether const or not).
        static_assert(!has<callable::chop(decltype(view) &)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::front)>::value);
        static_assert(
            !has<callable::chop(decltype(view) &, direction::back)>::value);

        static_assert(!has<callable::chop(decltype(view) const &)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::front)>::value);
        static_assert(!has<callable::chop(
                          decltype(view) const &, direction::back)>::value);

        // Run-time.
        static_assert(never_empty<decltype(view), direction::front>::value);
        BOOST_CHECK_EQUAL(size(view), 2u);

        auto chopped1 = chop(std::move(view));

        BOOST_CHECK_EQUAL(chopped1.first(), 5);
        static_assert(
            never_empty<decltype(chopped1.rest()), direction::front>::value);
        BOOST_CHECK_EQUAL(size(chopped1.rest()), 1u);

        auto chopped2 = chop(chopped1.move_rest());

        BOOST_CHECK_EQUAL(chopped2.first(), 6);
        static_assert(
            always_empty<decltype(chopped2.rest()), direction::front>::value);
        BOOST_CHECK_EQUAL(size(chopped2.rest()), 0u);
    }
}

BOOST_AUTO_TEST_SUITE_END()
