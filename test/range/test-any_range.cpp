// SPDX-FileCopyrightText: Copyright 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test any_range, its capabilities, and make_any_range.
*/

#define BOOST_TEST_MODULE test_range_any_range
#include "boost/test/unit_test.hpp"

#include "range/any_range.hpp"

#include <list>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "meta/map.hpp"
#include "meta/set.hpp"

#include "range/any_range/capability.hpp"
#include "range/core.hpp"
#include "range/direction.hpp"
#include "range/function_range.hpp"
#include "range/std/forward_list.hpp"
#include "range/std/list.hpp"
#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"
#include "range/tuple.hpp"

#include "unique_range.hpp"

using range::back;
using range::front;

using range::chop;
using range::chop_in_place;
using range::default_direction;
using range::drop;
using range::empty;
using range::first;
using range::size;

using range::has;
namespace callable = range::callable;

using range::any_range;
using range::make_any_range;

BOOST_AUTO_TEST_SUITE(test_range_any_range)

BOOST_AUTO_TEST_CASE(test_any_range_has)
{
    {
        typedef any_range<int, range::capability::unique_capabilities> range;
        // first and drop only available for rvalue references.
        static_assert(has<callable::empty(range const &)>::value);
        static_assert(!has<callable::first(range const &)>::value);
        static_assert(!has<callable::size(range const &)>::value);
        static_assert(!has<callable::drop(range const &)>::value);
        static_assert(!has<callable::drop(range const &, int)>::value);
        static_assert(has<callable::chop(range &&)>::value);
        static_assert(has<callable::chop_in_place(range &)>::value);

        static_assert(
            has<callable::empty(range const &, direction::back)>::value);
        static_assert(
            !has<callable::first(range const &, direction::back)>::value);
        static_assert(
            !has<callable::size(range const &, direction::back)>::value);
        static_assert(
            !has<callable::drop(range const &, direction::back)>::value);
        static_assert(
            !has<callable::drop(range const &, int, direction::back)>::value);
        static_assert(!has<callable::chop(range &&, direction::back)>::value);
        static_assert(
            !has<callable::chop_in_place(range &, direction::back)>::value);
    }
    {
        typedef any_range<int, range::capability::forward_capabilities> range;
        static_assert(has<callable::empty(range const &)>::value);
        static_assert(has<callable::first(range const &)>::value);
        static_assert(!has<callable::size(range const &)>::value);
        static_assert(has<callable::drop(range const &)>::value);
        static_assert(!has<callable::drop(range const &, int)>::value);
        static_assert(has<callable::chop(range &&)>::value);
        static_assert(has<callable::chop_in_place(range &)>::value);

        static_assert(
            has<callable::empty(range const &, direction::back)>::value);
        static_assert(
            !has<callable::first(range const &, direction::back)>::value);
        static_assert(
            !has<callable::size(range const &, direction::back)>::value);
        static_assert(
            !has<callable::drop(range const &, direction::back)>::value);
        static_assert(
            !has<callable::drop(range const &, int, direction::back)>::value);
        static_assert(!has<callable::chop(range &&, direction::back)>::value);
        static_assert(
            !has<callable::chop_in_place(range &, direction::back)>::value);
    }
    {
        typedef any_range<int, range::capability::bidirectional_capabilities>
            range;
        static_assert(has<callable::empty(range const &)>::value);
        static_assert(has<callable::first(range const &)>::value);
        static_assert(!has<callable::size(range const &)>::value);
        static_assert(has<callable::drop(range const &)>::value);
        static_assert(!has<callable::drop(range const &, int)>::value);
        static_assert(has<callable::chop(range &&)>::value);
        static_assert(has<callable::chop_in_place(range &)>::value);

        static_assert(
            has<callable::empty(range const &, direction::back)>::value);
        static_assert(
            has<callable::first(range const &, direction::back)>::value);
        static_assert(
            !has<callable::size(range const &, direction::back)>::value);
        static_assert(
            has<callable::drop(range const &, direction::back)>::value);
        static_assert(
            !has<callable::drop(range const &, int, direction::back)>::value);
        static_assert(has<callable::chop(range &&, direction::back)>::value);
        static_assert(
            has<callable::chop_in_place(range &, direction::back)>::value);
    }
    {
        typedef any_range<int, range::capability::random_access_capabilities>
            range;
        static_assert(has<callable::empty(range const &)>::value);
        static_assert(has<callable::first(range const &)>::value);
        static_assert(has<callable::size(range const &)>::value);
        static_assert(has<callable::drop(range const &)>::value);
        static_assert(has<callable::drop(range const &, int)>::value);
        static_assert(has<callable::chop(range &&)>::value);
        static_assert(has<callable::chop_in_place(range &)>::value);

        static_assert(
            has<callable::empty(range const &, direction::back)>::value);
        static_assert(
            has<callable::first(range const &, direction::back)>::value);
        static_assert(
            has<callable::size(range const &, direction::back)>::value);
        static_assert(
            has<callable::drop(range const &, direction::back)>::value);
        static_assert(
            has<callable::drop(range const &, int, direction::back)>::value);
        static_assert(has<callable::chop(range &&, direction::back)>::value);
        static_assert(
            has<callable::chop_in_place(range &, direction::back)>::value);
    }
}

BOOST_AUTO_TEST_CASE(test_any_range_homogeneous)
{
    std::vector<int> v;
    v.push_back(4);
    v.push_back(5);
    v.push_back(6);
    v.push_back(7);

    {
        any_range<int &> a(v);

        BOOST_CHECK(!range::empty(a));
        BOOST_CHECK_EQUAL(range::first(a), 4);

        a = range::drop(a);
        BOOST_CHECK(!range::empty(a));
        auto chopped = range::chop(a);
        BOOST_CHECK_EQUAL(chopped.first(), 5);

        BOOST_CHECK(!range::empty(chopped.rest()));

        a = chopped.move_rest();
        BOOST_CHECK(!range::empty(a));
        BOOST_CHECK_EQUAL(range::first(a), 6);

        int & e = range::chop_in_place(a);
        BOOST_CHECK_EQUAL(e, 6);
        BOOST_CHECK_EQUAL(&e, &v[2]);

        BOOST_CHECK(!range::empty(a));
        BOOST_CHECK_EQUAL(range::first(a), 7);

        a = range::drop(a);
        BOOST_CHECK(range::empty(a));
    }
    {
        auto a = range::make_any_range(v);

        static_assert(
            range::has<callable::empty(decltype(a), direction::back)>::value);
        static_assert(
            range::has<callable::first(decltype(a), direction::front)>::value);
        static_assert(
            range::has<callable::size(decltype(a), direction::back)>::value);
        static_assert(
            range::has<callable::drop(decltype(a), direction::back)>::value);
        static_assert(range::has<callable::drop(
                          decltype(a), int, direction::front)>::value);

        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(size(a), 4u);
        BOOST_CHECK_EQUAL(size(drop(a)), 3u);
        BOOST_CHECK_EQUAL(first(drop(a)), 5);
        BOOST_CHECK_EQUAL(first(drop(a, 2)), 6);
        BOOST_CHECK_EQUAL(first(a, back), 7);
        BOOST_CHECK_EQUAL(first(drop(a, back), back), 6);
        BOOST_CHECK_EQUAL(first(drop(a, 2, back), back), 5);
        BOOST_CHECK(empty(drop(a, 4), back));

        first(drop(a)) = 14;
        BOOST_CHECK_EQUAL(v[1], 14);

        // Convert to default capabilities

        any_range<int &> a2(a);

        BOOST_CHECK(!empty(a2));

        a2 = drop(a2);
        BOOST_CHECK(!empty(a2));
        // Was 5, now 14.
        BOOST_CHECK_EQUAL(first(a2), 14);

        a2 = drop(a2);
        BOOST_CHECK(!empty(a2));
        BOOST_CHECK_EQUAL(first(a2), 6);

        a2 = drop(a2);
        BOOST_CHECK(!empty(a2));
        BOOST_CHECK_EQUAL(first(a2), 7);

        a2 = drop(a2);
        BOOST_CHECK(empty(a2));

        // Convert to different type: int & -> long should be fine.

        any_range<long> al(a);
        BOOST_CHECK(!empty(al));
        BOOST_CHECK_EQUAL(first(al), 4l);
        BOOST_CHECK_EQUAL(first(drop(al)), 14l);
        al = drop(drop(al));
        BOOST_CHECK(!empty(al));
        BOOST_CHECK_EQUAL(first(al), 6l);
        BOOST_CHECK(!empty(drop(al)));
        BOOST_CHECK_EQUAL(first(drop(al)), 7l);
        al = drop(drop(al));
        BOOST_CHECK(empty(al));
    }
}

BOOST_AUTO_TEST_CASE(test_any_range_unique)
{
    std::vector<int> v;
    v.push_back(4);
    v.push_back(5);
    v.push_back(6);
    v.push_back(7);

    {
        auto a = make_any_range(unique_view(v));

        static_assert(
            !std::is_constructible<decltype(a), decltype(a) const &>::value);

        BOOST_CHECK_EQUAL(first(a), 4);
        a = drop(std::move(a));
        BOOST_CHECK_EQUAL(chop_in_place(a), 5);
        auto b = std::move(a);
        BOOST_CHECK_EQUAL(chop_in_place(b), 6);
        BOOST_CHECK_EQUAL(chop_in_place(b), 7);
        BOOST_CHECK(empty(b));
    }
    {
        any_range<int, range::capability::unique_capabilities> a(
            one_time_view(v));
        // first and drop are not available.
        BOOST_CHECK_EQUAL(chop_in_place(a), 4);

        auto chopped = chop(std::move(a));
        BOOST_CHECK_EQUAL(chopped.first(), 5);
        a = chopped.move_rest();

        BOOST_CHECK_EQUAL(chop_in_place(a), 6);
        auto b = std::move(a);
        BOOST_CHECK_EQUAL(chop_in_place(b), 7);
        BOOST_CHECK(empty(b));
    }
    {
        struct count
        {
            int i;
            count() : i(0) {}

            int operator()() { return ++i; }
        };

        any_range<int, range::capability::unique_capabilities> a(
            range::make_function_range(count()));

        BOOST_CHECK_EQUAL(chop_in_place(a), 1);
        BOOST_CHECK_EQUAL(chop_in_place(a), 2);
    }
}

BOOST_AUTO_TEST_CASE(test_any_range_heterogeneous)
{
    {
        std::tuple<> t;
        any_range<int> a(t);
        BOOST_CHECK(empty(a));
    }
    {
        std::tuple<int> t(7);
        any_range<int> a(t);
        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(first(a), 7);
        any_range<int> a_next = drop(a);
        BOOST_CHECK(empty(a_next));
    }
    {
        std::tuple<int, char, long> t(7, 'a', 294l);
        any_range<long> a(t);
        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(first(a), 7l);
        a = drop(a);
        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(first(a), long('a'));
        a = drop(a);
        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(first(a), 294l);
        a = drop(a);
        BOOST_CHECK(empty(a));
    }
    {
        std::tuple<int, char, long> t(7, 'a', 294l);
        any_range<long, range::capability::bidirectional_capabilities> a(t);
        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(chop_in_place(a), 7l);
        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(chop_in_place(a, back), 294l);
        BOOST_CHECK(!empty(a));
        BOOST_CHECK_EQUAL(chop_in_place(a, back), long('a'));
        BOOST_CHECK(empty(a));
    }
}

BOOST_AUTO_TEST_CASE(test_any_range_copy_move)
{
    typedef any_range<
        int,
        meta::map<
            meta::map_element<
                range::capability::default_direction, direction::front>,
            meta::map_element<
                direction::front,
                meta::set<
                    range::capability::empty, range::capability::size,
                    range::capability::first>>>>
        range_with_size;

    typedef any_range<
        int,
        meta::map<
            meta::map_element<
                range::capability::default_direction, direction::front>,
            meta::map_element<
                direction::front,
                meta::set<range::capability::empty, range::capability::first>>>>
        range_without_size;

    std::vector<int> v;
    v.push_back(26);

    static_assert(
        !std::is_constructible<range_with_size, range_with_size const &>::value,
        "The range cannot be copied so it is not copy-constructible");
    static_assert(
        !std::is_constructible<
            range_without_size, range_with_size const &>::value,
        "The range cannot be copied so it is not copy-constructible");

    static_assert(
        std::is_constructible<range_with_size, range_with_size &&>::value,
        "Moving to the same type is a pointer operation.");
    static_assert(
        !std::is_constructible<range_without_size, range_with_size &&>::value,
        "The range cannot be copied so it is not move-constructible "
        "with different capabilities.");

    range_with_size r(v);
    range_with_size r2(std::move(r));
    BOOST_CHECK_EQUAL(size(r2), 1u);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_range_any_range_capability)

using range::capability::detect_capabilities;
using range::capability::detect_capabilities_for_key;
using range::capability::detect_capability_keys;
using range::capability::is_subset;

using range::capability::copy_construct;
using range::capability::default_direction;

using range::capability::chop_destructive;
using range::capability::drop_n;
using range::capability::drop_one;
using range::capability::empty;
using range::capability::first;
using range::capability::size;

typedef decltype(range::view(std::declval<std::vector<int> &>())) vector;
typedef decltype(range::view(std::declval<std::list<int> &>())) list;
typedef decltype(range::view(
    std::declval<std::forward_list<int> &>())) forward_list;

typedef decltype(range::view(std::declval<range::tuple<int> &>())) tuple;
typedef decltype(range::view(std::declval<range::tuple<> &>())) empty_tuple;

typedef range::function_range<int (*)()> function_range;

BOOST_AUTO_TEST_CASE(test_capabilities_for_direction)
{
    static_assert(
        std::is_same_v<
            detect_capabilities_for_key<vector, direction::front>::type,
            meta::set<empty, size, first, drop_one, drop_n, chop_destructive>>);

    static_assert(
        std::is_same_v<
            detect_capabilities_for_key<function_range, direction::front>::type,
            meta::set<empty, chop_destructive>>);

    // An known-empty range has all capabilities!
    // (But they're all not allowed at run time.)
    static_assert(
        std::is_same_v<
            detect_capabilities_for_key<range::tuple<>, direction::back>::type,
            meta::set<empty, size, first, drop_one, drop_n, chop_destructive>>);
    static_assert(std::is_same_v<
                  detect_capabilities_for_key<
                      range::tuple<int>, direction::back>::type,
                  meta::set<empty, size, first, drop_one, chop_destructive>>);
}

BOOST_AUTO_TEST_CASE(test_detect_capability_keys)
{
    static_assert(
        std::is_same_v<
            detect_capability_keys<vector>::type,
            meta::set<copy_construct, direction::front, direction::back>>);

    static_assert(std::is_same_v<
                  detect_capability_keys<forward_list>::type,
                  meta::set<copy_construct, direction::front>>);

    static_assert(
        std::is_same_v<
            detect_capability_keys<tuple>::type,
            meta::set<copy_construct, direction::front, direction::back>>);
    static_assert(
        std::is_same_v<
            detect_capability_keys<empty_tuple>::type,
            meta::set<copy_construct, direction::front, direction::back>>);

    static_assert(std::is_same_v<
                  detect_capability_keys<function_range>::type,
                  meta::set<direction::front>>);
}

BOOST_AUTO_TEST_CASE(test_detect_capabilities)
{
    static_assert(std::is_same_v<
                  detect_capabilities<vector>::type,
                  meta::map<
                      meta::map_element<default_direction, direction::front>,
                      meta::map_element<copy_construct, void>,
                      meta::map_element<
                          direction::front,
                          meta::set<
                              empty, size, first, drop_one, drop_n,
                              chop_destructive>>,
                      meta::map_element<
                          direction::back,
                          meta::set<
                              empty, size, first, drop_one, drop_n,
                              chop_destructive>>>>);

    static_assert(
        std::is_same_v<
            detect_capabilities<list>::type,
            meta::map<
                meta::map_element<default_direction, direction::front>,
                meta::map_element<copy_construct, void>,
                meta::map_element<
                    direction::front,
                    meta::set<empty, first, drop_one, chop_destructive>>,
                meta::map_element<
                    direction::back,
                    meta::set<empty, first, drop_one, chop_destructive>>>>);

    static_assert(
        std::is_same_v<
            detect_capabilities<tuple>::type,
            meta::map<
                meta::map_element<default_direction, direction::front>,
                meta::map_element<copy_construct, void>,
                meta::map_element<
                    direction::front,
                    meta::set<empty, size, first, drop_one, chop_destructive>>,
                meta::map_element<
                    direction::back,
                    meta::set<
                        empty, size, first, drop_one, chop_destructive>>>>);

    static_assert(std::is_same_v<
                  detect_capabilities<empty_tuple>::type,
                  meta::map<
                      meta::map_element<default_direction, direction::front>,
                      meta::map_element<copy_construct, void>,
                      meta::map_element<
                          direction::front,
                          meta::set<
                              empty, size, first, drop_one, drop_n,
                              chop_destructive>>,
                      meta::map_element<
                          direction::back,
                          meta::set<
                              empty, size, first, drop_one, drop_n,
                              chop_destructive>>>>);

    // function_range: no copy constrution.
    static_assert(
        std::is_same_v<
            detect_capabilities<function_range>::type,
            meta::map<
                meta::map_element<default_direction, direction::front>,
                meta::map_element<
                    direction::front, meta::set<empty, chop_destructive>>>>);
}

BOOST_AUTO_TEST_CASE(test_is_subset)
{
    static_assert(is_subset<
                  detect_capabilities<function_range>::type,
                  detect_capabilities<function_range>::type>::value);

    static_assert(is_subset<
                  detect_capabilities<function_range>::type,
                  detect_capabilities<vector>::type>::value);
    static_assert(!is_subset<
                  detect_capabilities<vector>::type,
                  detect_capabilities<function_range>::type>::value);

    static_assert(is_subset<
                  detect_capabilities<list>::type,
                  detect_capabilities<vector>::type>::value);
    static_assert(!is_subset<
                  detect_capabilities<vector>::type,
                  detect_capabilities<list>::type>::value);

    static_assert(is_subset<
                  detect_capabilities<forward_list>::type,
                  detect_capabilities<vector>::type>::value);
    static_assert(!is_subset<
                  detect_capabilities<vector>::type,
                  detect_capabilities<forward_list>::type>::value);

    static_assert(is_subset<
                  detect_capabilities<forward_list>::type,
                  detect_capabilities<list>::type>::value);
    static_assert(!is_subset<
                  detect_capabilities<list>::type,
                  detect_capabilities<forward_list>::type>::value);

    static_assert(is_subset<
                  detect_capabilities<function_range>::type,
                  detect_capabilities<tuple>::type>::value);
    static_assert(!is_subset<
                  detect_capabilities<tuple>::type,
                  detect_capabilities<function_range>::type>::value);

    static_assert(is_subset<
                  detect_capabilities<tuple>::type,
                  detect_capabilities<empty_tuple>::type>::value);
    static_assert(!is_subset<
                  detect_capabilities<empty_tuple>::type,
                  detect_capabilities<tuple>::type>::value);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_range_make_any_range)

BOOST_AUTO_TEST_CASE(test_make_any_range)
{
    /*
    make_any_range decides on the any_range type and puts the view in it.
    The types is therefore the only thing to test.
    */
    using range::capability::copy_construct;
    using range::capability::default_direction;

    using range::capability::chop_destructive;
    using range::capability::drop_n;
    using range::capability::drop_one;
    using range::capability::empty;
    using range::capability::first;
    using range::capability::size;

    using meta::map;
    using meta::map_element;
    using meta::set;

    static_assert(std::is_same_v<
                  std::invoke_result<
                      callable::make_any_range, std::vector<int> &>::type,
                  any_range<
                      int &,
                      map<map_element<default_direction, direction::front>,
                          map_element<copy_construct, void>,
                          map_element<
                              direction::front,
                              set<empty, size, first, drop_one, drop_n,
                                  chop_destructive>>,
                          map_element<
                              direction::back,
                              set<empty, size, first, drop_one, drop_n,
                                  chop_destructive>>>>>);

    // Passing in front and back explicitly.
    static_assert(std::is_same_v<
                  std::invoke_result<
                      callable::make_any_range, std::vector<int> &,
                      direction::front, direction::back>::type,
                  std::invoke_result<
                      callable::make_any_range, std::vector<int> &>::type>);

    // Only front.
    static_assert(std::is_same_v<
                  std::invoke_result<
                      callable::make_any_range, std::vector<int> &,
                      direction::front>::type,
                  any_range<
                      int &,
                      map<map_element<default_direction, direction::front>,
                          map_element<copy_construct, void>,
                          map_element<
                              direction::front,
                              set<empty, size, first, drop_one, drop_n,
                                  chop_destructive>>>>>);

    // Only back: default_direction is still front.
    static_assert(std::is_same_v<
                  std::invoke_result<
                      callable::make_any_range, std::vector<int> &,
                      direction::back>::type,
                  any_range<
                      int &,
                      map<map_element<default_direction, direction::front>,
                          map_element<copy_construct, void>,
                          map_element<
                              direction::back,
                              set<empty, size, first, drop_one, drop_n,
                                  chop_destructive>>>>>);

    {
        std::vector<int> v;
        v.push_back(6);
        v.push_back(65);

        auto a1 = make_any_range(v);
        BOOST_CHECK_EQUAL(range::first(a1), 6);
        BOOST_CHECK_EQUAL(range::first(a1, range::back), 65);

        auto a2 = make_any_range(v, range::back);
        // The default direction does not match the available directions.
        static_assert(!has<callable::first(decltype(a2))>::value);
        BOOST_CHECK_EQUAL(range::first(a1, range::back), 65);
        BOOST_CHECK(range::empty(range::drop(a1, 2, range::back)));
    }

    static_assert(
        std::is_same_v<
            std::invoke_result<
                callable::make_any_range, std::list<double> const &>::type,
            any_range<
                double const &,
                map<map_element<default_direction, direction::front>,
                    map_element<copy_construct, void>,
                    map_element<
                        direction::front,
                        set<empty, first, drop_one, chop_destructive>>,
                    map_element<
                        direction::back,
                        set<empty, first, drop_one, chop_destructive>>>>>);

    typedef range::function_range<int (*)()> function_range;

    static_assert(
        std::is_same_v<
            std::invoke_result<
                callable::make_any_range, function_range &&>::type,
            any_range<
                int,
                map<map_element<default_direction, direction::front>,
                    map_element<
                        direction::front, set<empty, chop_destructive>>>>>);

    {
        struct count
        {
            int i;
            count() : i(0) {}

            int operator()() { return ++i; }
        };

        auto a = make_any_range(range::make_function_range(count()));

        BOOST_CHECK_EQUAL(range::chop_in_place(a), 1);
        BOOST_CHECK_EQUAL(range::chop_in_place(a), 2);
    }

    // Heterogeneous: only if there is at least one element.
    static_assert(
        std::is_same_v<
            std::invoke_result<
                callable::make_any_range, range::tuple<int> &>::type,
            any_range<
                int &,
                map<map_element<default_direction, direction::front>,
                    map_element<copy_construct, void>,
                    map_element<
                        direction::front,
                        set<empty, size, first, drop_one, chop_destructive>>,
                    map_element<
                        direction::back,
                        set<empty, size, first, drop_one,
                            chop_destructive>>>>>);
}

BOOST_AUTO_TEST_SUITE_END()
