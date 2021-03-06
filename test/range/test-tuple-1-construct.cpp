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

#define BOOST_TEST_MODULE test_range_tuple_construct
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"

#include <type_traits>
#include <string>
#include <tuple>
#include <vector>

#include <boost/mpl/assert.hpp>

#include "utility/test/tracked.hpp"

#include "range/std.hpp"

#include "rime/check/check_equal.hpp"

#include "unique_range.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_construct)

using range::tuple;

using range::front;
using range::back;

using range::default_direction;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::at_c;
using range::second;

using range::size_mismatch;
using range::never_empty;

using rime::false_;
using rime::true_;
rime::size_t <2> two;
rime::size_t <3> three;
rime::size_t <4> four;

using utility::tracked_registry;
using utility::tracked;

struct source {};

struct convertible_target {
    convertible_target (source const &) {}
    convertible_target() = delete;
};

struct constructible_target {
    explicit constructible_target (source const &) {}
};

struct inconvertible_target {};

struct base {
    int id;
    base (int id) : id (id) {}
};

struct derived : base {
    derived (int id) : base (id) {}
};

BOOST_AUTO_TEST_CASE (tuple_construct_test_test) {
    // Check whether the tests make sense.
    static_assert (
        std::is_convertible <source, convertible_target>::value, "");
    static_assert (
        std::is_constructible <convertible_target, source>::value, "");

    static_assert (
        !std::is_convertible <source, constructible_target>::value, "");
    static_assert (
        std::is_constructible <constructible_target, source>::value, "");

    static_assert (
        !std::is_convertible <source, inconvertible_target>::value, "");
    static_assert (
        !std::is_constructible <inconvertible_target, source>::value, "");
}

/* Default-construction from elements. */

BOOST_AUTO_TEST_CASE (tuple_construct_default) {
    static_assert (utility::is_default_constructible <tuple<>>::value, "");
    static_assert (
        utility::is_default_constructible <tuple <source>>::value, "");
    static_assert (
        utility::is_default_constructible <tuple <source, source>>::value, "");

    {
        tuple<> t;
        BOOST_CHECK (default_direction (t) == range::front);
    }
    {
        tuple <int> t;
        static_assert (never_empty <tuple <int>, direction::front>::value, "");
        BOOST_CHECK_EQUAL (at_c <0> (t), 0);
    }
    {
        tuple <std::string, int> t;
        static_assert (never_empty <tuple <int>, direction::front>::value, "");
        BOOST_CHECK_EQUAL (at_c <0> (t), "");
        BOOST_CHECK_EQUAL (at_c <1> (t), 0);
    }
}

/* Construction from elements. */
BOOST_AUTO_TEST_CASE (tuple_construct_one_element) {
    static_assert (std::is_constructible <
        tuple <convertible_target>, source>::value, "");

    static_assert (std::is_constructible <
        tuple <constructible_target>, source>::value, "");

    static_assert (!std::is_constructible <
        tuple <inconvertible_target>, source>::value, "");

    // Wrong number of elements.
    static_assert (!std::is_constructible <
        tuple <convertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target>, source, source>::value, "");

    // One element.
    {
        tuple <int> t (2);
        BOOST_CHECK (default_direction (t) == range::front);
        RIME_CHECK_EQUAL (range::empty (t), rime::false_);

        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (decltype (t) &)>::type, int &>));

        BOOST_CHECK_EQUAL (first (t), 2);
        BOOST_CHECK_EQUAL (first (t, front), 2);
        BOOST_CHECK_EQUAL (first (t, back), 2);

        first (t, back) += 7;
        BOOST_CHECK_EQUAL (first (t), 9);

        RIME_CHECK_EQUAL (empty (drop (t)), rime::true_);
    }

    // Copy.
    {
        tracked_registry c;
        {
            tracked <int> o (c, 5);
            tuple <tracked <int>> t (o);
            BOOST_CHECK_EQUAL (first (t).content(), 5);
            // value_construct, copy, move, copy_assign, move_assign, swap,
            // destruct, destruct_moved
            c.check_counts (1, 1, 0, 0, 0, 0, 0, 0);
        }
    }

    // Move.
    {
        tracked_registry c;
        {
            tracked <int> o (c, 7);
            tuple <tracked <int>> t (std::move (o));
            BOOST_CHECK_EQUAL (first (t).content(), 7);
            c.check_counts (1, 0, 1, 0, 0, 0, 0, 0);
        }
    }

    // Convertible.
    {
        char c = 31;
        tuple <int> t (c);
        BOOST_CHECK_EQUAL (first (t), 31);
    }

    // const, const &, &, &&.
    // In first and in other positions.
    // const.
    {
        tuple <int const> t1 (56);

        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::first (decltype (t1) &)>::type,
            int const &>));

        BOOST_CHECK_EQUAL (first (t1), 56);
    }
    // const &.
    {
        int i1 = 56;
        tuple <int const &> t1 (i1);

        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::first (decltype (t1) &)>::type,
            int const &>));

        BOOST_CHECK_EQUAL (first (t1), 56);
        i1 = 34;
        BOOST_CHECK_EQUAL (first (t1), 34);
    }
    // &.
    {
        int i1 = 56;
        tuple <int &> t1 (i1);

        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::first (decltype (t1) &)>::type,
            int &>));

        BOOST_CHECK_EQUAL (first (t1), 56);
        i1 = 34;
        BOOST_CHECK_EQUAL (first (t1), 34);
    }
    // &&.
    // Essentially should work like &, but then return &&.
    {
        int i1 = 56;
        tuple <int &&> t1 (std::move (i1));

        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::first (decltype (t1) &)>::type,
            int &&>));

        BOOST_CHECK_EQUAL (first (t1), 56);
        i1 = 34;
        BOOST_CHECK_EQUAL (first (t1), 34);
    }
}

// More than one elements.
BOOST_AUTO_TEST_CASE (tuple_construct_more_elements) {
    // Convertibility: the most restrictive element counts.
    // convertible_target, ...
    static_assert (std::is_constructible <
        tuple <convertible_target, convertible_target>,
        source, source>::value, "");

    static_assert (std::is_constructible <
        tuple <convertible_target, constructible_target>,
        source, source>::value, "");

    static_assert (!std::is_constructible <
        tuple <convertible_target, inconvertible_target>,
        source, source>::value, "");

    // constructible_target, ...
    static_assert (std::is_constructible <
        tuple <constructible_target, convertible_target>,
        source, source>::value, "");

    static_assert (std::is_constructible <
        tuple <constructible_target, constructible_target>,
        source, source>::value, "");

    static_assert (!std::is_constructible <
        tuple <constructible_target, inconvertible_target>,
        source, source>::value, "");

    // inconvertible_target, ...
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, convertible_target>,
        source, source>::value, "");

    static_assert (!std::is_constructible <
        tuple <inconvertible_target, constructible_target>,
        source, source>::value, "");

    static_assert (!std::is_constructible <
        tuple <inconvertible_target, inconvertible_target>,
        source, source>::value, "");

    // Wrong number of elements.
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        source>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        source, source, source>::value, "");

    // More elements: copy, move, convertible, and combinations.
    {
        tuple <int, bool> t (17, true);
        BOOST_CHECK (default_direction (t) == range::front);
        RIME_CHECK_EQUAL (range::empty (t), rime::false_);

        BOOST_CHECK_EQUAL (first (t), 17);
        BOOST_CHECK_EQUAL (first (t, front), 17);
        BOOST_CHECK_EQUAL (first (t, back), true);

        RIME_CHECK_EQUAL (empty (drop (t)), false_);
        RIME_CHECK_EQUAL (empty (drop (drop (t))), true_);
        RIME_CHECK_EQUAL (empty (drop (drop (t), back)), true_);
        RIME_CHECK_EQUAL (empty (drop (t, rime::size_t <2>())), true_);

        BOOST_CHECK_EQUAL (first (drop (t)), true);
        BOOST_CHECK_EQUAL (first (drop (t, back), back), 17);

        first (t, front) = 15;
        BOOST_CHECK_EQUAL (first (t), 15);
        first (t, back) = false;
        BOOST_CHECK_EQUAL (first (drop (t)), false);
    }

    {
        tuple <int, bool, double> t (17, true, 20.5);
        BOOST_CHECK (default_direction (t) == range::front);
        RIME_CHECK_EQUAL (range::empty (t), rime::false_);

        BOOST_CHECK_EQUAL (first (t), 17);
        BOOST_CHECK_EQUAL (first (t, front), 17);
        RIME_CHECK_EQUAL (first (t, back), 20.5);
        RIME_CHECK_EQUAL (first (drop (t, rime::size_t <2>())), 20.5);
        RIME_CHECK_EQUAL (first (drop (t)), true);
        RIME_CHECK_EQUAL (first (drop (t, back), back), true);

        RIME_CHECK_EQUAL (empty (drop (t)), false_);
        RIME_CHECK_EQUAL (empty (drop (drop (t))), false_);
        RIME_CHECK_EQUAL (empty (drop (drop (drop (t)))), true_);
    }
    {
        tracked_registry c;
        {
            tuple <tracked <int>, float, tracked <std::string>> t (
                tracked <int> (c, 45), .475,
                tracked <std::string> (c, "Hello"));
            c.check_counts (2, 0, 2, 0, 0, 0, 0, 2);
            BOOST_CHECK_EQUAL (first (t).content(), 45);
            BOOST_CHECK_EQUAL (first (t, back).content(), "Hello");
        }
    }

    // const, const &, &, &&.
    // In first and in other positions.
    // const.
    {
        tuple <bool, int const> const t2 (true, 67);

        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
                range::callable::first (decltype (t2) &, direction::back)
            >::type, int const &>));

        BOOST_CHECK_EQUAL (first (t2, back), 67);
    }
    // const &.
    {
        int const i2 = 67;
        tuple <bool, int const &> const t2 (true, i2);

        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
                range::callable::first (decltype (t2) &, direction::back)
            >::type, int const &>));

        BOOST_CHECK_EQUAL (first (t2, back), 67);
    }
    // &.
    {
        int i2 = 67;
        tuple <bool, int &> const t2 (true, i2);

        BOOST_MPL_ASSERT ((std::is_same <
            typename range::result_of <
                range::callable::first (decltype (t2) &, direction::back)
            >::type, int &>));

        BOOST_CHECK_EQUAL (first (t2, back), 67);
        i2 = 23;
        BOOST_CHECK_EQUAL (first (t2, back), 23);
    }
    // &&.
    // Essentially should work like &, but then return &&.
    {
        int i2 = 67;
        tuple <bool, int &&> const t2 (true, std::move (i2));

        BOOST_MPL_ASSERT ((std::is_same <
            typename range::result_of <
                range::callable::first (decltype (t2) &, direction::back)
            >::type, int &&>));

        BOOST_CHECK_EQUAL (first (t2, back), 67);
        i2 = 23;
        BOOST_CHECK_EQUAL (first (t2, back), 23);
    }
}

template <class Range> void test_view_from_empty (Range & range) {
    tuple<> t (range);
    RIME_CHECK_EQUAL (empty (t), rime::true_);
}

template <class IntRange> void test_view_from_int (IntRange & range) {
    tuple <int> t (range);
    BOOST_CHECK_EQUAL (first (t), 5);

    tuple <long> tl (range);
    BOOST_CHECK_EQUAL (first (tl), 5);

    tuple <int &> tr (range);
    BOOST_CHECK_EQUAL (&first (tr), &first (range));
    first (range) = 27;
    BOOST_CHECK_EQUAL (first (tr), 27);

    tuple <int const &> tcr (range);
    BOOST_CHECK_EQUAL (&first (tcr), &first (range));
    first (range) = 28;
    BOOST_CHECK_EQUAL (first (tcr), 28);

    tuple <int &&> trr (std::move (range));
    BOOST_CHECK_EQUAL (first (trr), 28);
    first (range) = 37;
    BOOST_CHECK_EQUAL (first (trr), 37);
}

// Empty tuple.
BOOST_AUTO_TEST_CASE (tuple_to_tuple_conversion_empty) {
    static_assert (std::is_convertible <tuple<>, tuple<>>::value, "");
    static_assert (std::is_constructible <tuple<>, tuple<>>::value, "");

    // Wrong size.
    static_assert (!std::is_constructible <tuple<>, tuple <int>>::value, "");

    {
        tuple<> t1;
        test_view_from_empty (t1);
    }
}

BOOST_AUTO_TEST_CASE (range_to_tuple_conversion_empty) {
    // From std::tuple.
    static_assert (std::is_convertible <std::tuple<>, tuple<>>::value, "");
    static_assert (std::is_constructible <tuple<>, std::tuple<>>::value, "");

    // Wrong size.
    static_assert (!std::is_constructible <
        tuple<>, std::tuple <source>>::value, "");
    static_assert (!std::is_constructible <
        tuple<>, std::tuple <source, source>>::value, "");

    // From std::vector.
    static_assert (!std::is_convertible <
        std::vector <int>, tuple<>>::value, "");
    static_assert (std::is_constructible <
        tuple<>, std::vector <int>>::value, "");

    {
        std::tuple<> t1;
        test_view_from_empty (t1);
    }
    {
        std::vector <int> v;
        test_view_from_empty (v);

        // Throw if the size is mismatched.
        v.push_back (4);
        BOOST_CHECK_THROW (tuple<> t3 (v), size_mismatch);
    }
}

// One element.
BOOST_AUTO_TEST_CASE (tuple_to_tuple_conversion_one) {
    static_assert (std::is_convertible <
        tuple <source>, tuple <convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target>, tuple <source>>::value, "");

    static_assert (!std::is_convertible <
        tuple <source>, tuple <constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target>, tuple <source>>::value, "");

    static_assert (!std::is_convertible <
        tuple <source>, tuple <inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target>, tuple <source>>::value, "");

    // Wrong size.
    static_assert (!std::is_constructible <
        tuple <convertible_target>, tuple<>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target>, tuple <source, source>>::value, "");

    {
        tuple <int> t1 (5);
        test_view_from_int (t1);
    }
    // One element, reference to derived.
    {
        derived d (4);
        tuple <derived &> td (d);
        tuple <base const &> tb (d);
        BOOST_CHECK_EQUAL (first (tb).id, 4);
    }
}

BOOST_AUTO_TEST_CASE (range_to_tuple_conversion_one) {
    // std::tuple.
    static_assert (std::is_convertible <
        std::tuple <source>, tuple <convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target>, std::tuple <source>>::value, "");

    static_assert (!std::is_convertible <
        std::tuple <source>, tuple <constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target>, std::tuple <source>>::value, "");

    static_assert (!std::is_convertible <
        std::tuple <source>, tuple <inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target>, std::tuple <source>>::value, "");

    // Wrong size.
    static_assert (!std::is_constructible <
        tuple <convertible_target>, std::tuple<>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target>, std::tuple <source, source>>::value, "");


    // std::vector: only explicitly convertible.
    static_assert (!std::is_convertible <
        std::vector <source>, tuple <convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target>, std::vector <source>>::value, "");

    static_assert (!std::is_convertible <
        std::vector <source>, tuple <constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target>, std::vector <source>>::value, "");

    static_assert (!std::is_convertible <
        std::vector <source>, tuple <inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target>, std::vector <source>>::value, "");

    {
        std::tuple <int> t1 (5);
        test_view_from_int (t1);
    }
    {
        tracked_registry c;
        {
            std::tuple <tracked <int>> t (tracked <int> (c, 39));
            c.check_counts (1, 0, 1, 0, 0, 0, 0, 1);
            tuple <tracked <int>> t2 (std::move (t));
            c.check_counts (1, 0, 2, 0, 0, 0, 0, 1);
        }
    }
    {
        std::vector <int> v;
        BOOST_CHECK_THROW (tuple <int> t (v), size_mismatch);

        v.push_back (5);
        test_view_from_int (v);

        v.push_back (27);
        BOOST_CHECK_THROW (tuple <int> t (v), size_mismatch);
    }
    {
        tracked_registry c;
        {
            std::vector <tracked <int>> v;
            v.push_back (tracked <int> (c, 39));
            c.check_counts (1, 0, 1, 0, 0, 0, 0, 1);
            tuple <tracked <int>> t2 (std::move (v));
            c.check_counts (1, 0, 2, 0, 0, 0, 0, 1);
        }
    }
}

// Two elements: the most restrictive element counts.
BOOST_AUTO_TEST_CASE (tuple_to_tuple_conversion_two) {
    // convertible_target, ...
    static_assert (std::is_convertible <tuple <source, source>,
        tuple <convertible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, convertible_target>,
        tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <convertible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, constructible_target>,
        tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <convertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, inconvertible_target>,
        tuple <source, source>>::value, "");

    // constructible_target, ...
    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <constructible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, convertible_target>,
        tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <constructible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, constructible_target>,
        tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <constructible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <constructible_target, inconvertible_target>,
        tuple <source, source>>::value, "");

    // inconvertible_target, ...
    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <inconvertible_target, convertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, convertible_target>,
        tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <inconvertible_target, constructible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, constructible_target>,
        tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <tuple <source, source>,
        tuple <inconvertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, inconvertible_target>,
        tuple <source, source>>::value, "");

    // Wrong size.
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        tuple<>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        tuple <source>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        tuple <source, source, source>>::value, "");

    // Two types.
    {
        tuple <bool, int> t1 (false, 7);
        tuple <bool, int> t2 (t1);
        BOOST_CHECK_EQUAL (first (t2), false);
        BOOST_CHECK_EQUAL (first (drop (t2)), 7);
    }
    {
        tracked_registry c;
        tuple <tracked <int>> t (tracked <int> (c, 39));
        c.check_counts (1, 0, 1, 0, 0, 0, 0, 1);
        tuple <tracked <int>> t2 (std::move (t));
        c.check_counts (1, 0, 2, 0, 0, 0, 0, 1);
    }

    // More than one element.
    {
        tracked_registry c;
        {
            tracked <int> ci (c, 50);
            tracked <float> cf (c, 23.);
            tuple <tracked <int>, char, tracked <float>> t (ci, 'Z', cf);
            c.check_counts (2, 2, 0, 0, 0, 0, 0, 0);

            tuple <tracked <void>, int, tracked <void>> t2 (t);
            c.check_counts (2, 4, 0, 0, 0, 0, 0, 0);
            BOOST_CHECK_EQUAL (at_c <1> (t2), int ('Z'));

            tuple <tracked <void>, int, tracked <void>> t3 (std::move (t));
            c.check_counts (2, 4, 2, 0, 0, 0, 0, 0);
            // If moveability was not exploited, it would be this:
            // c.check_counts (2, 6, 0, 0, 0, 0, 0, 0);
            BOOST_CHECK_EQUAL (at_c <1> (t3), int ('Z'));
        }
    }

    // All possible complicated types.
    tracked_registry c;
    {
        typedef tuple <
            int, bool const, float &,
            tracked <double> const &, tracked <int> &&, tracked <long>>
            tuple_type;
        float f = 22.25f;
        tracked <double> d (c, 17.5);
        tracked <int> o (c, 25);
        tracked <long> l (c, 27l);
        tuple_type t1 (7, false, f, d, std::move (o), l);
        // For d, and o, the object has not been moved: only the reference has
        // been copied.
        c.check_counts (3, 1, 0, 0, 0, 0, 0, 0);

        tuple_type t2 (t1);
        RIME_CHECK_EQUAL (first (t2), 7);
        RIME_CHECK_EQUAL (first (drop (t2)), false);
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <2>())), 22.25f);
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <3>())).content(),
            17.5);
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <4>())).content(), 25);
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <5>())).content(), 27l);

        c.check_counts (3, 2, 0, 0, 0, 0, 0, 0);

        f = -3.75;
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <2>())), -3.75f);
        d.content() = -2.;
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <3>())).content(), -2.);
        o.content() = 987;
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <4>())).content(), 987);
        l.content() = 34;
        // Copy of l does not change.
        RIME_CHECK_EQUAL (first (drop (t2, rime::size_t <5>())).content(), 27l);

        // Moving a tuple.
        tuple_type t3 (std::move (t1));
        RIME_CHECK_EQUAL (first (t3), 7);
        RIME_CHECK_EQUAL (first (drop (t3)), false);
        RIME_CHECK_EQUAL (first (drop (t3, two)), -3.75f);
        RIME_CHECK_EQUAL (first (drop (t3, three)).content(), -2.);
        BOOST_CHECK_EQUAL (&first (drop (t3, three)), &d);
        RIME_CHECK_EQUAL (first (drop (t3, four)).content(), 987);
        BOOST_CHECK_EQUAL (
            &static_cast <tracked <int> const &> (first (drop (t3, four))), &o);
        RIME_CHECK_EQUAL (first (drop (t3, rime::size_t <5>())).content(), 27l);

        // If moveability was not exploited, it would be this:
        // c.check_counts (3, 3, 0, 0, 0, 0, 0, 0);
        c.check_counts (3, 2, 1, 0, 0, 0, 0, 0);
    }
}

BOOST_AUTO_TEST_CASE (range_to_tuple_conversion_two) {
    // std::tuple.
    // convertible_target, ...
    static_assert (std::is_convertible <std::tuple <source, source>,
        tuple <convertible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, convertible_target>,
        std::tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <convertible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, constructible_target>,
        std::tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <convertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, inconvertible_target>,
        std::tuple <source, source>>::value, "");

    // constructible_target, ...
    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <constructible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, convertible_target>,
        std::tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <constructible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, constructible_target>,
        std::tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <constructible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <constructible_target, inconvertible_target>,
        std::tuple <source, source>>::value, "");

    // inconvertible_target, ...
    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <inconvertible_target, convertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, convertible_target>,
        std::tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <inconvertible_target, constructible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, constructible_target>,
        std::tuple <source, source>>::value, "");

    static_assert (!std::is_convertible <std::tuple <source, source>,
        tuple <inconvertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, inconvertible_target>,
        std::tuple <source, source>>::value, "");

    // Wrong size.
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        std::tuple<>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        std::tuple <source>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, convertible_target>,
        std::tuple <source, source, source>>::value, "");

    // std::pair.
    // convertible_target, ...
    static_assert (std::is_convertible <std::pair <source, source>,
        tuple <convertible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, convertible_target>,
        std::pair <source, source>>::value, "");

    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <convertible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, constructible_target>,
        std::pair <source, source>>::value, "");

    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <convertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, inconvertible_target>,
        std::pair <source, source>>::value, "");

    // constructible_target, ...
    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <constructible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, convertible_target>,
        std::pair <source, source>>::value, "");

    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <constructible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, constructible_target>,
        std::pair <source, source>>::value, "");

    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <constructible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <constructible_target, inconvertible_target>,
        std::pair <source, source>>::value, "");

    // inconvertible_target, ...
    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <inconvertible_target, convertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, convertible_target>,
        std::pair <source, source>>::value, "");

    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <inconvertible_target, constructible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, constructible_target>,
        std::pair <source, source>>::value, "");

    static_assert (!std::is_convertible <std::pair <source, source>,
        tuple <inconvertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, inconvertible_target>,
        std::pair <source, source>>::value, "");

    // std::vector: only explicitly convertible.
    // convertible_target, ...
    static_assert (!std::is_convertible <std::vector <source>,
        tuple <convertible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, convertible_target>,
        std::vector <source>>::value, "");

    static_assert (!std::is_convertible <std::vector <source>,
        tuple <convertible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <convertible_target, constructible_target>,
        std::vector <source>>::value, "");

    static_assert (!std::is_convertible <std::vector <source>,
        tuple <convertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <convertible_target, inconvertible_target>,
        std::vector <source>>::value, "");

    // constructible_target, ...
    static_assert (!std::is_convertible <std::vector <source>,
        tuple <constructible_target, convertible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, convertible_target>,
        std::vector <source>>::value, "");

    static_assert (!std::is_convertible <std::vector <source>,
        tuple <constructible_target, constructible_target>>::value, "");
    static_assert (std::is_constructible <
        tuple <constructible_target, constructible_target>,
        std::vector <source>>::value, "");

    static_assert (!std::is_convertible <std::vector <source>,
        tuple <constructible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <constructible_target, inconvertible_target>,
        std::vector <source>>::value, "");

    // inconvertible_target, ...
    static_assert (!std::is_convertible <std::vector <source>,
        tuple <inconvertible_target, convertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, convertible_target>,
        std::vector <source>>::value, "");

    static_assert (!std::is_convertible <std::vector <source>,
        tuple <inconvertible_target, constructible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, constructible_target>,
        std::vector <source>>::value, "");

    static_assert (!std::is_convertible <std::vector <source>,
        tuple <inconvertible_target, inconvertible_target>>::value, "");
    static_assert (!std::is_constructible <
        tuple <inconvertible_target, inconvertible_target>,
        std::vector <source>>::value, "");

    // std::tuple.
    {
        std::tuple <bool, int> t1 (false, 7);
        tuple <bool, int> t2 (t1);
        BOOST_CHECK_EQUAL (first (t2), false);
        BOOST_CHECK_EQUAL (first (drop (t2)), 7);
    }

    // std::pair.
    {
        std::pair <bool, int> t1 (false, 7);
        tuple <bool, int> t2 (t1);
        BOOST_CHECK_EQUAL (first (t2), false);
        BOOST_CHECK_EQUAL (first (drop (t2)), 7);
    }

    // std::vector
    {
        std::vector <float> v;
        BOOST_CHECK_THROW ((tuple <float, double> (v)), size_mismatch);
        v.push_back (7.25);
        BOOST_CHECK_THROW ((tuple <float, double> (v)), size_mismatch);
        v.push_back (9.5);

        tuple <float, double> t2 (v);
        BOOST_CHECK_EQUAL (first (t2), 7.25);
        BOOST_CHECK_EQUAL (first (drop (t2)), 9.5);

        v.push_back (10.1);
        BOOST_CHECK_THROW ((tuple <float, double> (v)), size_mismatch);
    }
}

BOOST_AUTO_TEST_CASE (unique) {
    // Construct from a unique heterogeneous range.
    {
        range::tuple <int, double> t (6, 6.25);

        range::tuple <int, double> copy (unique_view (std::move (t)));
        BOOST_CHECK_EQUAL (first (copy), 6);
        BOOST_CHECK_EQUAL (second (copy), 6.25);
    }
    {
        range::tuple <int, double> t (6, 6.25);

        range::tuple <int, double> copy (one_time_view (std::move (t)));
        BOOST_CHECK_EQUAL (first (copy), 6);
        BOOST_CHECK_EQUAL (second (copy), 6.25);
    }

    // Construct from a unique homogeneous range.
    {
        std::vector <int> v;
        v.push_back (4);
        v.push_back (7);

        range::tuple <int, double> copy (unique_view (std::move (v)));
        BOOST_CHECK_EQUAL (first (copy), 4);
        BOOST_CHECK_EQUAL (second (copy), 7);
    }
    {
        std::vector <int> v;
        v.push_back (4);
        v.push_back (7);

        range::tuple <int, double> copy (one_time_view (std::move (v)));
        BOOST_CHECK_EQUAL (first (copy), 4);
        BOOST_CHECK_EQUAL (second (copy), 7);
    }
}

BOOST_AUTO_TEST_SUITE_END()
