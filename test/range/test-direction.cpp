/*
Copyright 2012 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_direction
#include "utility/test/boost_unit_test.hpp"

#include "range/direction.hpp"

#include <type_traits>

#include "weird_direction.hpp"

BOOST_AUTO_TEST_SUITE(test_range_direction)

BOOST_AUTO_TEST_CASE (test_weird_direction) {
    // make_forward
    static_assert (!direction::has <direction::callable::make_forward (
        int)>::value, "");
    static_assert (!direction::has <direction::callable::make_forward (
        int const)>::value, "");
    static_assert (!direction::has <direction::callable::make_forward (
        direction::front)>::value, "");
    static_assert (!direction::has <direction::callable::make_forward (
        direction::front const)>::value, "");
    static_assert (direction::has <direction::callable::make_forward (
        direction::back)>::value, "");
    static_assert (direction::has <direction::callable::make_forward (
        direction::back const)>::value, "");
    static_assert (!direction::has <direction::callable::make_forward (
        weird_direction)>::value, "");
    static_assert (!direction::has <direction::callable::make_forward (
        weird_direction const)>::value, "");
    static_assert (direction::has <direction::callable::make_forward (
        weird_reverse_direction)>::value, "");
    static_assert (direction::has <direction::callable::make_forward (
        weird_reverse_direction const)>::value, "");

    static_assert (!direction::has <direction::callable::ensure_forward (
        int)>::value, "");
    static_assert (direction::has <direction::callable::ensure_forward (
        direction::front)>::value, "");
    static_assert (direction::has <direction::callable::ensure_forward (
        direction::back)>::value, "");
    static_assert (direction::has <direction::callable::ensure_forward (
        weird_direction)>::value, "");
    static_assert (direction::has <direction::callable::ensure_forward (
        weird_reverse_direction)>::value, "");
    static_assert (direction::has <direction::callable::ensure_forward (
        weird_reverse_direction &)>::value, "");

    static_assert (!direction::has <direction::callable::reverse (
        int)>::value, "");
    static_assert (direction::has <direction::callable::reverse (
        direction::front)>::value, "");
    static_assert (direction::has <direction::callable::reverse (
        direction::back)>::value, "");
    static_assert (direction::has <direction::callable::reverse (
        weird_direction)>::value, "");
    static_assert (direction::has <direction::callable::reverse (
        weird_reverse_direction)>::value, "");
    static_assert (direction::has <direction::callable::reverse (
        weird_reverse_direction &&)>::value, "");

    // 7 is the magic number.
    weird_direction d (7);

    auto rd1 = direction::reverse (d);
    static_assert (std::is_same <
        decltype (rd1), weird_reverse_direction>::value, "");
    auto d1 = direction::reverse (rd1);
    static_assert (std::is_same <decltype (d1), weird_direction>::value, "");
    auto d2 = direction::ensure_forward (rd1);
    static_assert (std::is_same <decltype (d2), weird_direction>::value, "");
    auto d3 = direction::ensure_forward (d1);
    static_assert (std::is_same <decltype (d3), weird_direction>::value, "");

    static_assert (std::is_same <direction::result_of <
        direction::callable::make_forward (weird_reverse_direction)>::type,
        weird_direction>::value, "");

    static_assert (std::is_same < direction::result_of <
        direction::callable::reverse (weird_direction const)>::type,
        weird_reverse_direction>::value, "");
    static_assert (std::is_same < direction::result_of <
        direction::callable::reverse (weird_reverse_direction &)>::type,
        weird_direction>::value, "");

    static_assert (std::is_same < direction::result_of <
        direction::callable::ensure_forward (weird_direction &&)>::type,
        weird_direction>::value, "");
    static_assert (std::is_same < direction::result_of <
        direction::callable::ensure_forward (weird_reverse_direction)>::type,
        weird_direction>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()

