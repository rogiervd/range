/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_any_range_capability
#include "utility/test/boost_unit_test.hpp"

#include "range/any_range/capability.hpp"

#include <type_traits>
#include <forward_list>
#include <list>
#include <vector>

#include "range/tuple.hpp"
#include "range/std.hpp"
#include "range/function_range.hpp"

BOOST_AUTO_TEST_SUITE(test_range_any_range_capability)

using range::capability::detect_capabilities_for_key;
using range::capability::detect_capability_keys;
using range::capability::detect_capabilities;
using range::capability::is_subset;

using range::capability::default_direction;
using range::capability::copy_construct;

using range::capability::empty;
using range::capability::size;
using range::capability::first;
using range::capability::drop_one;
using range::capability::drop_n;
using range::capability::chop_destructive;

typedef decltype (range::view (std::declval <std::vector <int> &>())) vector;
typedef decltype (range::view (std::declval <std::list <int> &>())) list;
typedef decltype (range::view (std::declval <std::forward_list <int> &>()))
    forward_list;

typedef decltype (range::view (std::declval <range::tuple <int> &>())) tuple;
typedef decltype (range::view (std::declval <range::tuple<> &>())) empty_tuple;

typedef range::function_range <int (*) ()> function_range;

BOOST_AUTO_TEST_CASE (test_capabilities_for_direction) {
    static_assert (std::is_same <detect_capabilities_for_key <
            vector, direction::front>::type,
        meta::set <empty, size, first, drop_one, drop_n, chop_destructive>
        >::value, "");

    static_assert (std::is_same <detect_capabilities_for_key <
            function_range, direction::front>::type,
        meta::set <empty, chop_destructive>
        >::value, "");

    // An known-empty range has all capabilities!
    // (But they're all not allowed at run time.)
    static_assert (std::is_same <detect_capabilities_for_key <
            range::tuple <>, direction::back>::type,
        meta::set <empty, size, first, drop_one, drop_n, chop_destructive>
        >::value, "");
    static_assert (std::is_same <detect_capabilities_for_key <
            range::tuple <int>, direction::back>::type,
        meta::set <empty, size, first, drop_one, chop_destructive>
        >::value, "");
}

BOOST_AUTO_TEST_CASE (test_detect_capability_keys) {
    static_assert (std::is_same <
        detect_capability_keys <vector>::type,
        meta::set <copy_construct, direction::front, direction::back>>::value,
        "");

    static_assert (std::is_same <
        detect_capability_keys <forward_list>::type,
        meta::set <copy_construct, direction::front>>::value, "");

    static_assert (std::is_same <
        detect_capability_keys <tuple>::type,
        meta::set <copy_construct, direction::front, direction::back>>::value,
        "");
    static_assert (std::is_same <
        detect_capability_keys <empty_tuple>::type,
        meta::set <copy_construct, direction::front, direction::back>>::value,
        "");

    static_assert (std::is_same <
        detect_capability_keys <function_range>::type,
        meta::set <direction::front>>::value, "");
}

BOOST_AUTO_TEST_CASE (test_detect_capabilities) {
    static_assert (std::is_same <
        detect_capabilities <vector>::type,
        meta::map <
            meta::map_element <default_direction, direction::front>,
            meta::map_element <copy_construct, void>,
            meta::map_element <direction::front, meta::set <
                empty, size, first, drop_one, drop_n, chop_destructive>>,
            meta::map_element <direction::back, meta::set <
                empty, size, first, drop_one, drop_n, chop_destructive>>
        >>::value, "");

    static_assert (std::is_same <
        detect_capabilities <list>::type,
        meta::map <
            meta::map_element <default_direction, direction::front>,
            meta::map_element <copy_construct, void>,
            meta::map_element <direction::front, meta::set <
                empty, first, drop_one, chop_destructive>>,
            meta::map_element <direction::back, meta::set <
                empty, first, drop_one, chop_destructive>>
        >>::value, "");

    static_assert (std::is_same <
        detect_capabilities <tuple>::type,
        meta::map <
            meta::map_element <default_direction, direction::front>,
            meta::map_element <copy_construct, void>,
            meta::map_element <direction::front, meta::set <
                empty, size, first, drop_one, chop_destructive>>,
            meta::map_element <direction::back, meta::set <
                empty, size, first, drop_one, chop_destructive>>
        >>::value, "");

    static_assert (std::is_same <
        detect_capabilities <empty_tuple>::type,
        meta::map <
            meta::map_element <default_direction, direction::front>,
            meta::map_element <copy_construct, void>,
            meta::map_element <direction::front, meta::set <
                empty, size, first, drop_one, drop_n, chop_destructive>>,
            meta::map_element <direction::back, meta::set <
                empty, size, first, drop_one, drop_n, chop_destructive>>
        >>::value, "");

    // function_range: no copy constrution.
    static_assert (std::is_same <
        detect_capabilities <function_range>::type,
        meta::map <
            meta::map_element <default_direction, direction::front>,
            meta::map_element <direction::front, meta::set <
                empty, chop_destructive>>
        >>::value, "");
}

BOOST_AUTO_TEST_CASE (test_is_subset) {
    static_assert (is_subset <
        detect_capabilities <function_range>::type,
        detect_capabilities <function_range>::type
        >::value, "");

    static_assert (is_subset <
        detect_capabilities <function_range>::type,
        detect_capabilities <vector>::type
        >::value, "");
    static_assert (!is_subset <
        detect_capabilities <vector>::type,
        detect_capabilities <function_range>::type
        >::value, "");

    static_assert (is_subset <
        detect_capabilities <list>::type,
        detect_capabilities <vector>::type
        >::value, "");
    static_assert (!is_subset <
        detect_capabilities <vector>::type,
        detect_capabilities <list>::type
        >::value, "");

    static_assert (is_subset <
        detect_capabilities <forward_list>::type,
        detect_capabilities <vector>::type
        >::value, "");
    static_assert (!is_subset <
        detect_capabilities <vector>::type,
        detect_capabilities <forward_list>::type
        >::value, "");

    static_assert (is_subset <
        detect_capabilities <forward_list>::type,
        detect_capabilities <list>::type
        >::value, "");
    static_assert (!is_subset <
        detect_capabilities <list>::type,
        detect_capabilities <forward_list>::type
        >::value, "");

    static_assert (is_subset <
        detect_capabilities <function_range>::type,
        detect_capabilities <tuple>::type
        >::value, "");
    static_assert (!is_subset <
        detect_capabilities <tuple>::type,
        detect_capabilities <function_range>::type
        >::value, "");

    static_assert (is_subset <
        detect_capabilities <tuple>::type,
        detect_capabilities <empty_tuple>::type
        >::value, "");
    static_assert (!is_subset <
        detect_capabilities <empty_tuple>::type,
        detect_capabilities <tuple>::type
        >::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
