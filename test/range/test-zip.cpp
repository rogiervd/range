// SPDX-FileCopyrightText: Copyright 2014, 2015, 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test zip, first on homogeneous ranges, and then on heterogeneous ranges.
*/

#define BOOST_TEST_MODULE test_range_zip
#include "boost/test/unit_test.hpp"

#include "range/zip.hpp"

#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "rime/check/check_equal.hpp"
#include "rime/core.hpp"

#include "range/core.hpp"
#include "range/direction.hpp"
#include "range/for_each_macro.hpp"
#include "range/std/string.hpp"
#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"
#include "range/tuple.hpp"

#include "unique_range.hpp"
#include "weird_count.hpp"
#include "weird_direction.hpp"

using range::tuple;

template <class... Types> struct show_types;

using range::view;
using range::zip;
using range::zip_from;

using range::empty;
using range::first;
using range::second;
using range::size;
using range::third;

using range::chop;
using range::chop_in_place;
using range::drop;

using range::make_tuple;

using range::has;
namespace callable = range::callable;

BOOST_AUTO_TEST_SUITE(test_range_zip)

template <class...> struct show_types;

BOOST_AUTO_TEST_CASE(example)
{
    std::vector<int> vi;
    vi.push_back(2);
    vi.push_back(7);

    std::vector<std::string> vs;
    vs.push_back("Hello");
    vs.push_back("there!");
    vs.push_back("Never change");

    std::size_t count = 0;

    auto z = zip(vi, vs);
    RANGE_FOR_EACH(pair, z)
    {
        if (first(pair) == 2) {
            BOOST_CHECK_EQUAL(second(pair), "Hello");
        } else {
            BOOST_CHECK_EQUAL(first(pair), 7);
            BOOST_CHECK_EQUAL(second(pair), "there!");
        }

        // Set the first element to the length of the second element.
        first(pair) = int(size(second(pair)));

        ++count;
    }

    BOOST_CHECK_EQUAL(count, 2u);

    BOOST_CHECK_EQUAL(first(vi), 5);
    BOOST_CHECK_EQUAL(second(vi), 6);

    // Mutate the whole pair, and thereby the underlying two vectors, at once.
    RANGE_FOR_EACH(pair, z)
    pair = make_tuple(77, "Something else");

    BOOST_CHECK_EQUAL(first(vi), 77);
    BOOST_CHECK_EQUAL(second(vi), 77);

    BOOST_CHECK_EQUAL(first(vs), "Something else");
    BOOST_CHECK_EQUAL(second(vs), "Something else");

    // The third element of vs is never seen since vi only has two elements.
    BOOST_CHECK_EQUAL(third(vs), "Never change");
}

BOOST_AUTO_TEST_CASE(begin_end)
{
    std::vector<int> vi {2, 7};

    std::vector<std::string> vs {"Hello", "there!", "Never change"};

    std::size_t count = 0;

    for (auto pair : zip(vi, vs)) {
        if (first(pair) == 2) {
            BOOST_CHECK_EQUAL(second(pair), "Hello");
        } else {
            BOOST_CHECK_EQUAL(first(pair), 7);
            BOOST_CHECK_EQUAL(second(pair), "there!");
        }

        // Set the first element to the length of the second element.
        first(pair) = int(size(second(pair)));

        ++count;
    }

    BOOST_CHECK_EQUAL(count, 2u);

    BOOST_CHECK_EQUAL(first(vi), 5);
    BOOST_CHECK_EQUAL(second(vi), 6);
}


// The same as above, but using structured binding.
BOOST_AUTO_TEST_CASE(structured_binding)
{
    std::vector<int> vi {2, 7};

    std::vector<std::string> vs {"Hello", "there!", "Never change"};

    std::size_t count = 0;

    for (auto [number, name] : zip(vi, vs)) {
        if (number == 2) {
            BOOST_CHECK_EQUAL(name, "Hello");
        } else {
            BOOST_CHECK_EQUAL(number, 7);
            BOOST_CHECK_EQUAL(name, "there!");
        }

        // Set the first element to the length of the second element.
        number = int(size(name));

        ++count;
    }

    BOOST_CHECK_EQUAL(count, 2u);

    BOOST_CHECK_EQUAL(first(vi), 5);
    BOOST_CHECK_EQUAL(second(vi), 6);
}

BOOST_AUTO_TEST_CASE(homogeneous)
{
    std::vector<int> vi;
    std::vector<double> vd;

    {
        auto z = range::zip(vi, vd);

        static_assert(
            std::is_same_v<
                decltype(z),
                range::zip_range<
                    direction::front, decltype(view(vi)), decltype(view(vd))>>);

        static_assert(range::is_range<decltype(z)>::value);
        static_assert(
            range::is_homogeneous<decltype(z), direction::front>::value);
        BOOST_CHECK(range::default_direction(z) == range::front);

        BOOST_CHECK(range::empty(z));
        BOOST_CHECK(range::empty(z, range::front));

        BOOST_CHECK_EQUAL(range::size(z), 0u);
        BOOST_CHECK_EQUAL(range::size(z, range::front), 0u);

        static_assert(has<callable::drop(decltype(z))>::value);
        static_assert(
            has<callable::drop(decltype(z), direction::front)>::value);
        static_assert(
            !has<callable::drop(decltype(z), direction::back)>::value);

        static_assert(has<callable::chop(decltype(z))>::value);
        static_assert(
            has<callable::chop(decltype(z), direction::front)>::value);
        static_assert(
            !has<callable::chop(decltype(z), direction::back)>::value);
    }

    vi.push_back(1);
    vd.push_back(2.5);

    BOOST_CHECK_EQUAL(first(vi), 1);

    {
        auto z = zip(vi, vd);

        auto first_element = first(z);

        BOOST_CHECK_EQUAL(range::size(z), 1u);

        BOOST_CHECK_EQUAL(first(first_element), 1);
        BOOST_CHECK_EQUAL(second(first_element), 2.5);

        auto z2 = drop(z);

        BOOST_CHECK(empty(first(z2.underlying())));
        BOOST_CHECK(empty(second(z2.underlying())));

        BOOST_CHECK(empty(z2));

        auto z3 = drop(z, 1);
        BOOST_CHECK(empty(first(z3.underlying())));
        BOOST_CHECK(empty(second(z3.underlying())));

        BOOST_CHECK(empty(z3));

        // auto chopped = chop (z);
        // RIME_CHECK_EQUAL (first (chopped.first()), 1);
        // RIME_CHECK_EQUAL (second (chopped.first()), 2.5);
        // BOOST_CHECK (empty (chopped.rest()));
    }

    vi.push_back(3);
    vi.push_back(27);

    vd.push_back(5.5);
    vd.push_back(27.5);
    // This makes vd one longer than vi, so it is never seen from the front.
    vd.push_back(43.5);

    // Test zip_from.
    {
        std::tuple<std::vector<int>, std::vector<double>> t(vi, vd);
        {
            auto z = zip_from(t);

            auto first_element = first(z);

            BOOST_CHECK_EQUAL(first(first_element), 1);
            BOOST_CHECK_EQUAL(second(first_element), 2.5);
        }
        // front explicitly specified.
        {
            auto z = zip_from(t, range::front);

            auto first_element = first(z);

            static_assert(range::has<range::callable::chop(
                              decltype(drop(z)) const &)>::value);

            BOOST_CHECK_EQUAL(first(first_element), 1);
            BOOST_CHECK_EQUAL(second(first_element), 2.5);
        }
        // From the back.
        {
            auto z = zip_from(t, range::back);

            auto first_element = first(z);

            BOOST_CHECK_EQUAL(first(first_element), 27);
            BOOST_CHECK_EQUAL(second(first_element), 43.5);

            auto chopped = chop(drop(z));

            static_assert(range::has<range::callable::chop(
                              decltype(zip_from(t)))>::value);
            // show_types <decltype (z)> st;
            // show_types <decltype (range::default_direction (z))> st2;
            static_assert(range::has<range::callable::chop(
                              decltype(zip_from(t, range::back)))>::value);

            BOOST_CHECK_EQUAL(first(chopped.first()), 3);
            BOOST_CHECK_EQUAL(second(chopped.first()), 27.5);

            BOOST_CHECK(empty(drop(chopped.rest())));
        }
    }

    {
        auto z = zip(vi, vd);

        auto first_element = first(z);

        BOOST_CHECK_EQUAL(range::size(first(z.underlying())), 3u);
        BOOST_CHECK_EQUAL(range::size(second(z.underlying())), 4u);

        BOOST_CHECK_EQUAL(range::size(z), 3u);

        BOOST_CHECK_EQUAL(first(first_element), 1);
        BOOST_CHECK_EQUAL(second(first_element), 2.5);

        BOOST_CHECK_EQUAL(first(second(z)), 3);
        BOOST_CHECK_EQUAL(second(second(z)), 5.5);

        BOOST_CHECK_EQUAL(first(third(z)), 27);
        BOOST_CHECK_EQUAL(second(third(z)), 27.5);

        auto z2 = drop(z, 2);
        BOOST_CHECK_EQUAL(first(first(z2)), 27);
        BOOST_CHECK_EQUAL(second(first(z2)), 27.5);

        BOOST_CHECK(empty(drop(z, 3)));
    }
}

BOOST_AUTO_TEST_CASE(single)
{
    std::vector<int> v;
    auto z = zip(v);
    static_assert(range::is_homogeneous<decltype(z), direction::front>::value);

    RIME_CHECK_EQUAL(empty(z), true);
}

BOOST_AUTO_TEST_CASE(with_weird_count)
{
    weird_count w1(8);
    weird_count w2(17);
    auto z = zip_from(make_tuple(w1, w2), weird_direction(7));

    auto d = range::default_direction(z);
    static_assert(std::is_same_v<decltype(d), weird_direction>);

    BOOST_CHECK_EQUAL(first(first(z)), 8);
    BOOST_CHECK_EQUAL(second(first(z)), 17);

    static_assert(!has<callable::size(decltype(z))>::value);

    for (int count = 0; count != 10; ++count) {
        BOOST_CHECK_EQUAL(first(first(z)), 8 + count);
        BOOST_CHECK_EQUAL(second(first(z)), 17 + count);

        if ((count % 2) == 0) {
            z = drop(z, weird_direction(7));
        } else {
            z = chop(z, weird_direction(7)).rest();
        }
    }
}

BOOST_AUTO_TEST_CASE(unique_range)
{
    std::vector<int> vi;
    vi.push_back(3);
    vi.push_back(7);

    std::vector<double> vd;
    vd.push_back(3.5);
    vd.push_back(7.5);

    {
        auto z = zip(unique_view(vi), vd);

        RIME_CHECK_EQUAL(first(first(z)), 3);
        RIME_CHECK_EQUAL(second(first(z)), 3.5);

        z = drop(std::move(z));

        RIME_CHECK_EQUAL(first(first(z)), 7);
        RIME_CHECK_EQUAL(second(first(z)), 7.5);

        z = drop(std::move(z));
        BOOST_CHECK(empty(z));
    }
    {
        auto z = zip(one_time_view(vi), vd);

        auto chopped = chop(std::move(z));
        RIME_CHECK_EQUAL(first(chopped.first()), 3);
        RIME_CHECK_EQUAL(second(chopped.first()), 3.5);

        // Can call "first" with rvalue reference.
        auto element = first(chopped.move_rest());
        RIME_CHECK_EQUAL(first(element), 7);
        RIME_CHECK_EQUAL(second(element), 7.5);
    }
}

BOOST_AUTO_TEST_CASE(heterogeneous)
{
    {
        std::tuple<> t;
        auto z1 = zip(t);
        RIME_CHECK_EQUAL(empty(z1), rime::true_);
        auto z2 = zip(t, t);
        RIME_CHECK_EQUAL(empty(z2), rime::true_);
        auto z3 = zip(t, t, t);
        RIME_CHECK_EQUAL(empty(z3), rime::true_);

        RIME_CHECK_EQUAL(size(z3), rime::size_t<0>());
    }
    {
        std::tuple<int> ti(4);
        std::tuple<double> td(7.5);
        auto z = zip(ti, td);
        RIME_CHECK_EQUAL(empty(z), rime::false_);

        RIME_CHECK_EQUAL(first(first(z)), 4);
        RIME_CHECK_EQUAL(second(first(z)), 7.5);

        RIME_CHECK_EQUAL(size(z), rime::size_t<1>());

        auto tuple_tuple = std::make_tuple(ti, td);

        auto z2 = zip_from(tuple_tuple);
        static_assert(std::is_same_v<decltype(z), decltype(z2)>);
        RIME_CHECK_EQUAL(first(first(z2)), 4);
        RIME_CHECK_EQUAL(second(first(z2)), 7.5);

        auto z3 = zip(ti, unique_view(td));

        auto chopped = chop(std::move(z3));
        RIME_CHECK_EQUAL(first(chopped.first()), 4);
        RIME_CHECK_EQUAL(second(chopped.first()), 7.5);
        RIME_CHECK_EQUAL(empty(chopped.rest()), rime::true_);
    }
}

BOOST_AUTO_TEST_CASE(homogeneous_and_heterogeneous)
{
    std::vector<int> v;
    {
        std::tuple<> t;
        auto z = zip(v, t);
        RIME_CHECK_EQUAL(empty(z), rime::true_);
        // In theory this could be known to be 0 at compile time.
        // It is not currently, and it is not clear that that is overly
        // valuable anyway.
        BOOST_CHECK_EQUAL(size(z), 0u);
    }
    {
        std::tuple<int> t;
        auto z = zip(v, t);
        RIME_CHECK_EQUAL(empty(z), true);
        RIME_CHECK_EQUAL(size(z), std::size_t(0));
    }

    v.push_back(5);
    {
        std::tuple<double> t(4.5);
        auto z = zip(v, t);
        BOOST_CHECK(!empty(z));
        BOOST_CHECK_EQUAL(size(z), 1u);
        RIME_CHECK_EQUAL(first(first(z)), 5);
        RIME_CHECK_EQUAL(second(first(z)), 4.5);
        // It is known at compile-time that t is empty.
        RIME_CHECK_EQUAL(empty(drop(z)), rime::true_);
    }

    {
        std::tuple<double, std::string> t(4.5, "");
        auto z = zip(v, t);
        BOOST_CHECK(!empty(z));
        BOOST_CHECK_EQUAL(size(z), 1u);
        RIME_CHECK_EQUAL(first(first(z)), 5);
        RIME_CHECK_EQUAL(second(first(z)), 4.5);
        // Only known at run-time that this is empty.
        RIME_CHECK_EQUAL(empty(drop(z)), true);
    }

    v.push_back(7);
    {
        std::tuple<double, std::string> t(27.5, "Test");
        auto z = zip(t, v);

        BOOST_CHECK_EQUAL(size(z), 2u);

        RIME_CHECK_EQUAL(first(first(z)), 27.5);
        RIME_CHECK_EQUAL(first(second(z)), std::string("Test"));
        RIME_CHECK_EQUAL(second(first(z)), 5);
        RIME_CHECK_EQUAL(second(second(z)), 7);

        RIME_CHECK_EQUAL(empty(drop(z, rime::size_t<2>())), rime::true_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
