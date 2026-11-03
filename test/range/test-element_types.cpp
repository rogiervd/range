// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_element_types
#include <boost/test/unit_test.hpp>

#include "range/element_types.hpp"

#include <list>
#include <tuple>
#include <type_traits>
#include <vector>

#include "meta/vector.hpp"

#include "range/std/list.hpp"
#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"
#include "range/take.hpp"

BOOST_AUTO_TEST_SUITE(test_range_element_types)

BOOST_AUTO_TEST_CASE(test_element_types_contents)
{
    {
        typedef std::tuple<> tuple;

        static_assert(std::is_same_v<
                      meta::as_vector<range::element_types<tuple>>::type,
                      meta::vector<>>);

        static_assert(std::is_same_v<
                      meta::as_vector<range::element_types<tuple &>>::type,
                      meta::vector<>>);

        static_assert(
            std::is_same_v<
                meta::as_vector<range::element_types<tuple const &>>::type,
                meta::vector<>>);
    }
    {
        typedef std::tuple<int> tuple;

        static_assert(std::is_same_v<
                      meta::as_vector<range::element_types<tuple>>::type,
                      meta::vector<int &&>>);

        static_assert(std::is_same_v<
                      meta::as_vector<range::element_types<tuple &>>::type,
                      meta::vector<int &>>);

        static_assert(
            std::is_same_v<
                meta::as_vector<range::element_types<tuple const &>>::type,
                meta::vector<int const &>>);
    }
    {
        typedef std::tuple<int, int, int const &, float> tuple;

        static_assert(std::is_same_v<
                      meta::as_vector<range::element_types<tuple>>::type,
                      meta::vector<int &&, int &&, int const &, float &&>>);

        static_assert(std::is_same_v<
                      meta::as_vector<range::element_types<tuple &>>::type,
                      meta::vector<int &, int &, int const &, float &>>);

        static_assert(
            std::is_same_v<
                meta::as_vector<range::element_types<tuple const &>>::type,
                meta::vector<
                    int const &, int const &, int const &, float const &>>);
    }
}

BOOST_AUTO_TEST_CASE(test_element_types_behaviour)
{
    {
        typedef meta::as_vector<range::element_types<std::tuple<>>>::type types;

        static_assert(meta::empty<types>::value);
        static_assert(meta::size<types>::value == 0);

        static_assert(meta::empty<direction::front, types>::value);
        static_assert(meta::size<direction::front, types>::value == 0);

        static_assert(meta::empty<direction::back, types>::value);
        static_assert(meta::size<direction::back, types>::value == 0);
    }
    {
        typedef range::element_types<std::tuple<float>> types;

        static_assert(!meta::empty<types>::value);
        static_assert(meta::size<types>::value == 1);

        static_assert(!meta::empty<direction::front, types>::value);
        static_assert(meta::size<direction::front, types>::value == 1);

        static_assert(!meta::empty<direction::back, types>::value);
        static_assert(meta::size<direction::back, types>::value == 1);

        static_assert(std::is_same_v<meta::first<types>::type, float &&>);
        static_assert(std::is_same_v<
                      meta::first<direction::front, types>::type, float &&>);
        static_assert(std::is_same_v<
                      meta::first<direction::back, types>::type, float &&>);

        static_assert(meta::empty<meta::drop<types>::type>::value);
        static_assert(
            meta::empty<meta::drop<direction::front, types>::type>::value);
        static_assert(
            meta::empty<meta::drop<direction::back, types>::type>::value);
    }
    {
        typedef range::element_types<std::tuple<float, bool, int>> types;

        static_assert(!meta::empty<types>::value);
        static_assert(meta::size<types>::value == 3);

        static_assert(!meta::empty<direction::front, types>::value);
        static_assert(meta::size<direction::front, types>::value == 3);

        static_assert(!meta::empty<direction::back, types>::value);
        static_assert(meta::size<direction::back, types>::value == 3);

        static_assert(std::is_same_v<meta::first<types>::type, float &&>);
        static_assert(std::is_same_v<
                      meta::first<direction::front, types>::type, float &&>);
        static_assert(
            std::is_same_v<meta::first<direction::back, types>::type, int &&>);

        static_assert(!meta::empty<meta::drop<types>::type>::value);
        static_assert(
            meta::empty<meta::drop<
                direction::front, rime::size_t<3>, types>::type>::value);

        static_assert(std::is_same_v<
                      meta::first<meta::drop<types>::type>::type, bool &&>);
    }
}

template <class... Types> struct show_types;

// Used on a homogeneous type, element_types becomes an infinite meta-range.
BOOST_AUTO_TEST_CASE(test_element_types_homogeneous)
{
    {
        typedef range::element_types<std::vector<int> &> types;

        static_assert(std::is_same_v<meta::first<types>::type, int &>);
        static_assert(
            std::is_same_v<meta::first<meta::drop<types>::type>::type, int &>);
        static_assert(
            std::is_same_v<
                meta::first<meta::drop<rime::size_t<34>, types>::type>::type,
                int &>);

        // After one call to "drop", the range is turned into a view.
        typedef meta::drop<types>::type view;
        static_assert(std::is_same_v<meta::drop<view>::type, view>);
        // It then becomes heterogeneous.
        static_assert(std::is_same_v<meta::drop<view>::type, view>);
        static_assert(
            std::is_same_v<meta::drop<rime::size_t<3>, view>::type, view>);
    }
    {
        std::list<int> l;
        auto v = range::take(l, rime::size_t<2>());
        typedef range::element_types<decltype(v)> types;

        static_assert(std::is_same_v<meta::first<types>::type, int &>);
        static_assert(
            std::is_same_v<meta::first<meta::drop<types>::type>::type, int &>);

        static_assert(!meta::empty<types>::value);
        static_assert(!meta::empty<meta::drop<types>::type>::value);
        static_assert(
            meta::empty<meta::drop<meta::drop<types>::type>::type>::value);
    }
}

BOOST_AUTO_TEST_SUITE_END()
