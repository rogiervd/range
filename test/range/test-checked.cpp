/*
Copyright 2017 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_checked
#include "utility/test/boost_unit_test.hpp"

#include "range/checked.hpp"

#include <type_traits>

#include "range/tuple.hpp"
#include "range/std/vector.hpp"

#include "weird_direction.hpp"
#include "weird_count.hpp"

BOOST_AUTO_TEST_SUITE(test_checked)

using range::checked;
using range::view;

using range::default_direction;
using range::empty;
using range::first;
using range::size;
using range::drop;
using range::at;
using range::chop;
using range::chop_in_place;

using range::front;
using range::back;

using range::is_homogeneous;

namespace callable = range::callable;

BOOST_AUTO_TEST_CASE (test_empty) {
    std::vector <int> v;

    auto c = checked (view (v));
    static_assert (range::is_view <decltype (c)>::value, "");
    static_assert (
        range::is_homogeneous <decltype (c), direction::front>::value, "");
    static_assert (
        range::is_homogeneous <decltype (c), direction::back>::value, "");
    BOOST_CHECK (empty (c));
    BOOST_CHECK_EQUAL (size (c), 0);

    BOOST_CHECK_THROW (first (c), range::out_of_range_error);

    {
        // This should not generate a nested type.
        auto cc = checked (c);
        static_assert (std::is_same <decltype (c), decltype (cc)>::value, "");
        BOOST_CHECK (empty (cc));
        BOOST_CHECK_EQUAL (size (cc), 0);
    }

    // drop (c, 0) should not throw, because dropping zero elements is fine.
    drop (c, 0);
    BOOST_CHECK_THROW (drop (c), range::out_of_range_error);
    BOOST_CHECK_THROW (drop (c, 7), range::out_of_range_error);
    BOOST_CHECK_THROW (chop (c), range::out_of_range_error);
    BOOST_CHECK_THROW (chop_in_place (c), range::out_of_range_error);

    static_assert (range::has <callable::empty (decltype (c))>::value, "");
    static_assert (range::has <
        callable::empty (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::empty (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <callable::size (decltype (c))>::value, "");
    static_assert (range::has <
        callable::size (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::size (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <callable::first (decltype (c))>::value, "");
    static_assert (range::has <
        callable::first (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::first (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <callable::drop (decltype (c))>::value, "");
    static_assert (range::has <
        callable::drop (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::drop (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <callable::drop (decltype (c), int)>::value, "");
    static_assert (range::has <
        callable::drop (decltype (c), int, direction::back)>::value, "");
    static_assert (!range::has <
        callable::drop (decltype (c), int, weird_direction)>::value, "");

    static_assert (range::has <callable::chop (decltype (c))>::value, "");
    static_assert (range::has <
        callable::chop (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::chop (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <
        callable::chop_in_place (decltype (c) &)>::value, "");
    static_assert (range::has <
        callable::chop_in_place (decltype (c) &, direction::back)>::value, "");
    static_assert (!range::has <
        callable::chop_in_place (decltype (c) &, weird_direction)>::value, "");
}

BOOST_AUTO_TEST_CASE (test_vector) {
    std::vector <int> v = {1, 2, 3};

    auto c = checked (view (v));
    BOOST_CHECK (!empty (c));
    BOOST_CHECK_EQUAL (size (c), 3);
    BOOST_CHECK_EQUAL (first (c), 1);
    BOOST_CHECK_EQUAL (first (c, back), 3);

    {
        // This should not generate a nested type.
        auto cc = checked (c);
        static_assert (std::is_same <decltype (c), decltype (cc)>::value, "");
        BOOST_CHECK (!empty (cc));
        BOOST_CHECK_EQUAL (size (cc), 3);
        BOOST_CHECK_EQUAL (first (cc), 1);
        BOOST_CHECK_EQUAL (first (cc, back), 3);
    }

    auto c2 = drop (c, front);
    BOOST_CHECK_EQUAL (size (c2), 2);
    BOOST_CHECK_EQUAL (first (c2), 2);
    BOOST_CHECK_EQUAL (first (c2, back), 3);

    auto c3 = drop (c2, back);
    BOOST_CHECK_EQUAL (size (c3), 1);
    BOOST_CHECK_EQUAL (first (c3), 2);
    BOOST_CHECK_EQUAL (first (c3, back), 2);

    auto chopped_3 = chop (c2, back);
    BOOST_CHECK_EQUAL (chopped_3.first(), 3);
    BOOST_CHECK_EQUAL (size (chopped_3.rest()), 1);
    BOOST_CHECK_EQUAL (first (chopped_3.rest()), 2);
    BOOST_CHECK_EQUAL (first (chopped_3.rest(), back), 2);

    auto c4 = drop (c, 2, back);
    BOOST_CHECK_EQUAL (size (c4), 1);
    BOOST_CHECK_EQUAL (first (c4), 1);
    BOOST_CHECK_EQUAL (first (c4, back), 1);

    // Copy construction.
    decltype (c) c2_copy = c2;
    BOOST_CHECK_EQUAL (size (c2_copy), 2);
    BOOST_CHECK_EQUAL (first (c2_copy), 2);
    BOOST_CHECK_EQUAL (first (c2_copy, back), 3);

    int i = chop_in_place (c2_copy);
    BOOST_CHECK_EQUAL (i, 2);
    BOOST_CHECK_EQUAL (size (c2_copy), 1);
    BOOST_CHECK_EQUAL (first (c2_copy), 3);
    BOOST_CHECK_EQUAL (first (c2_copy, back), 3);

    // Move construction.
    decltype (c) c3_moved = std::move (c3);
    // This would now trigger an assertion to fail:
    // size (c3);

    BOOST_CHECK_EQUAL (size (c3_moved), 1);
    BOOST_CHECK_EQUAL (first (c3_moved), 2);
    BOOST_CHECK_EQUAL (first (c3_moved, back), 2);

    auto c5 = drop (c, 3);
    BOOST_CHECK (empty (c5));

    BOOST_CHECK_THROW (drop (c, 4), range::out_of_range_error);

    // Copy assignment.
    c5 = c3_moved;
    BOOST_CHECK_EQUAL (size (c5), 1);
    BOOST_CHECK_EQUAL (first (c5), 2);
    BOOST_CHECK_EQUAL (first (c5, back), 2);

    // Move assignment.
    c5 = std::move (c2);
    BOOST_CHECK_EQUAL (size (c5), 2);
    BOOST_CHECK_EQUAL (first (c5), 2);
    BOOST_CHECK_EQUAL (first (c5, back), 3);
}

BOOST_AUTO_TEST_CASE (test_tuple) {
    range::tuple <int, short, double> v {1, 2, 3};

    auto c = checked (view (v));
    BOOST_CHECK (!empty (c));
    BOOST_CHECK_EQUAL (size (c), 3);
    BOOST_CHECK_EQUAL (first (c), 1);
    BOOST_CHECK_EQUAL (first (c, back), 3);

    {
        // This should not generate a nested type.
        auto cc = checked (c);
        static_assert (std::is_same <decltype (c), decltype (cc)>::value, "");
        BOOST_CHECK (!empty (cc));
        BOOST_CHECK_EQUAL (size (cc), 3);
        BOOST_CHECK_EQUAL (first (cc), 1);
        BOOST_CHECK_EQUAL (first (cc, back), 3);
    }

    auto c2 = drop (c, front);
    BOOST_CHECK_EQUAL (size (c2), 2);
    BOOST_CHECK_EQUAL (first (c2), 2);
    BOOST_CHECK_EQUAL (first (c2, back), 3);

    auto c3 = drop (c2, back);
    BOOST_CHECK_EQUAL (size (c3), 1);
    BOOST_CHECK_EQUAL (first (c3), 2);
    BOOST_CHECK_EQUAL (first (c3, back), 2);

    auto chopped_3 = chop (c2, back);
    BOOST_CHECK_EQUAL (chopped_3.first(), 3);
    BOOST_CHECK_EQUAL (size (chopped_3.rest()), 1);
    BOOST_CHECK_EQUAL (first (chopped_3.rest()), 2);
    BOOST_CHECK_EQUAL (first (chopped_3.rest(), back), 2);

    auto c4 = drop (c, rime::size_t <2>(), back);
    BOOST_CHECK_EQUAL (size (c4), 1);
    BOOST_CHECK_EQUAL (first (c4), 1);
    BOOST_CHECK_EQUAL (first (c4, back), 1);

    // Copy construction.
    decltype (c2) c2_copy = c2;
    BOOST_CHECK_EQUAL (size (c2_copy), 2);
    BOOST_CHECK_EQUAL (first (c2_copy), 2);
    BOOST_CHECK_EQUAL (first (c2_copy, back), 3);

    auto c5 = drop (c, rime::size_t <3>());
    BOOST_CHECK (empty (c5));

    static_assert (range::has <callable::empty (decltype (c))>::value, "");
    static_assert (range::has <
        callable::empty (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::empty (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <callable::size (decltype (c))>::value, "");
    static_assert (range::has <
        callable::size (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::size (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <callable::first (decltype (c))>::value, "");
    static_assert (range::has <
        callable::first (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::first (decltype (c), weird_direction)>::value, "");

    static_assert (range::has <callable::drop (decltype (c))>::value, "");
    static_assert (range::has <
        callable::drop (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::drop (decltype (c), weird_direction)>::value, "");

    static_assert (!range::has <callable::drop (decltype (c), int)>::value, "");
    static_assert (!range::has <
        callable::drop (decltype (c), int, direction::back)>::value, "");
    static_assert (!range::has <
        callable::drop (decltype (c), int, weird_direction)>::value, "");

    static_assert (range::has <
        callable::drop (decltype (c), rime::int_<1>)>::value, "");
    static_assert (range::has <
        callable::drop (decltype (c), rime::int_<1>, direction::back)>::value,
        "");
    static_assert (!range::has <
        callable::drop (decltype (c), int, weird_direction)>::value, "");

    static_assert (range::has <callable::chop (decltype (c))>::value, "");
    static_assert (range::has <
        callable::chop (decltype (c), direction::back)>::value, "");
    static_assert (!range::has <
        callable::chop (decltype (c), weird_direction)>::value, "");

    static_assert (!range::has <
        callable::chop_in_place (decltype (c) &)>::value, "");
    static_assert (!range::has <
        callable::chop_in_place (decltype (c) &, direction::back)>::value, "");
    static_assert (!range::has <
        callable::chop_in_place (decltype (c) &, weird_direction)>::value, "");
}

BOOST_AUTO_TEST_CASE (test_with_weird_count) {
    weird_count w;
    weird_direction direction (7);

    auto c = checked (view (w, direction));

    BOOST_CHECK_EQUAL (first (c, direction), 0);

    auto c5 = drop (c, 5, direction);
    BOOST_CHECK_EQUAL (first (c5, direction), 5);
}

BOOST_AUTO_TEST_SUITE_END()
