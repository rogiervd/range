// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test structured binding for range::tuple.
*/

#define BOOST_TEST_MODULE range_test_tuple_less
#include <boost/test/unit_test.hpp>

#include "range/tuple.hpp"

#include <string>
#include <vector>
#include "range/std/vector.hpp"

namespace std {

// template <class ... Types>
// struct tuple_size<::range::tuple<Types...>>
// // This assumes that the size is a compile-time constant.
// : ::range::result_of<::range::callable::size(::range::tuple<Types...>,
// ::direction::front)>::type {};

// template <std::size_t Position, class ... Types>
// struct tuple_element<Position, ::range::tuple<Types...>>
// : ::range::result_of<::range::callable::at(::range::tuple<Types...>,
// rime::size_t<Position>)> {};

template <class... Types> struct tuple_size<::range::tuple<Types...>>
: rime::size_t<sizeof...(Types)> {};

template <std::size_t Position, class... Types>
struct tuple_element<Position, ::range::tuple<Types...>>
: meta::first<typename meta::drop<
      rime::size_t<Position>, meta::vector<Types...>>::type> {};
}  // namespace std

namespace range {
template <std::size_t Position, class... Types> auto get(tuple<Types...> & t)
{
    return ::range::at(t, rime::size_t<Position>());
}

template <std::size_t Position, class... Types> auto get(tuple<Types...> && t)
{
    return ::range::at(std::move(t), rime::size_t<Position>());
}

template <std::size_t Position, class... Types>
auto get(tuple<Types...> const & t)
{
    return ::range::at(t, rime::size_t<Position>());
}
}  // namespace range


BOOST_AUTO_TEST_SUITE(range_test_tuple_structured_binding)

using range::tuple;

BOOST_AUTO_TEST_CASE(compile_time)
{
    static_assert(std::tuple_size<tuple<char>>::value == 1);
    static_assert(std::tuple_size<tuple<char, int>>::value == 2);
    static_assert(std::tuple_size<tuple<char, long, bool>>::value == 3);

    static_assert(
        std::is_same_v<std::tuple_element<0, tuple<char>>::type, char>);

    static_assert(
        std::is_same_v<std::tuple_element<0, tuple<char, int>>::type, char>);
    static_assert(
        std::is_same_v<std::tuple_element<1, tuple<char, int>>::type, int>);

    static_assert(std::is_same_v<
                  std::tuple_element<0, tuple<char, long, bool>>::type, char>);
    static_assert(std::is_same_v<
                  std::tuple_element<1, tuple<char, long, bool>>::type, long>);
    static_assert(std::is_same_v<
                  std::tuple_element<2, tuple<char, long, bool>>::type, bool>);
}


BOOST_AUTO_TEST_CASE(run_time)
{
    tuple<int> t1(27);
    tuple<char, int> t2('c', 28);
    tuple<char, int, bool> t3('c', 29, true);

    auto [i1] = t1;
    BOOST_CHECK(i1 == 27);

    auto [c2, i2] = t2;
    BOOST_CHECK(c2 == 'c');
    BOOST_CHECK(i2 == 28);

    auto [c3, i3, b3] = t3;
    BOOST_CHECK(c3 == 'c');
    BOOST_CHECK(i3 == 29);
    BOOST_CHECK(b3 == true);

    // TODO check return types.
}

BOOST_AUTO_TEST_CASE(example)
{
    std::vector<range::tuple<int, std::string>> v {
        {0, "zero"}, {1, "one"}, {2, "two"}};

    for (auto [number, name] : v) {
        if (number == 0) {
            BOOST_CHECK(name == "zero");
        } else if (number == 1) {
            BOOST_CHECK(name == "one");
        } else {
            BOOST_CHECK(number == 2);
            BOOST_CHECK(name == "two");
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
