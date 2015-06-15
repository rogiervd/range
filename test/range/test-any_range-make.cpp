/*
Copyright 2013, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_make_any_range
#include "utility/test/boost_unit_test.hpp"

#include "range/any_range.hpp"

#include <type_traits>
#include <list>
#include <vector>
#include <tuple>

#include "range/std.hpp"
#include "range/tuple.hpp"
#include "range/function_range.hpp"

#include "weird_count.hpp"
#include "unique_range.hpp"

using range::has;
namespace callable = range::callable;

using range::any_range;
using range::make_any_range;

BOOST_AUTO_TEST_SUITE(test_range_make_any_range)

BOOST_AUTO_TEST_CASE (test_make_any_range) {
    /*
    make_any_range decides on the any_range type and puts the view in it.
    The types is therefore the only thing to test.
    */
    using range::capability::default_direction;
    using range::capability::copy_construct;

    using range::capability::empty;
    using range::capability::size;
    using range::capability::first;
    using range::capability::drop_one;
    using range::capability::drop_n;
    using range::capability::chop_destructive;

    using meta::set;
    using meta::map;
    using meta::map_element;

    static_assert (std::is_same <
        std::result_of <callable::make_any_range (std::vector <int> &)>::type,
        any_range <int &, map <
            map_element <default_direction, direction::front>,
            map_element <copy_construct, void>,
            map_element <direction::front, set <
                empty, size, first, drop_one, drop_n, chop_destructive>>,
            map_element <direction::back, set <
                empty, size, first, drop_one, drop_n, chop_destructive>>
        >> >::value, "");

    // Passing in front and back explicitly.
    static_assert (std::is_same <
        std::result_of <callable::make_any_range (
            std::vector <int> &, direction::front, direction::back)>::type,
        std::result_of <callable::make_any_range (std::vector <int> &)>::type
        >::value, "");

    // Only front.
    static_assert (std::is_same <
        std::result_of <callable::make_any_range (
            std::vector <int> &, direction::front)>::type,
        any_range <int &, map <
            map_element <default_direction, direction::front>,
            map_element <copy_construct, void>,
            map_element <direction::front, set <
                empty, size, first, drop_one, drop_n, chop_destructive>>
        >> >::value, "");

    // Only back: default_direction is still front.
    static_assert (std::is_same <
        std::result_of <callable::make_any_range (
            std::vector <int> &, direction::back)>::type,
        any_range <int &, map <
            map_element <default_direction, direction::front>,
            map_element <copy_construct, void>,
            map_element <direction::back, set <
                empty, size, first, drop_one, drop_n, chop_destructive>>
        >> >::value, "");

    {
        std::vector <int> v;
        v.push_back (6);
        v.push_back (65);

        auto a1 = make_any_range (v);
        BOOST_CHECK_EQUAL (range::first (a1), 6);
        BOOST_CHECK_EQUAL (range::first (a1, range::back), 65);

        auto a2 = make_any_range (v, range::back);
        // The default direction does not match the available directions.
        static_assert (!has <callable::first (decltype (a2))>::value, "");
        BOOST_CHECK_EQUAL (range::first (a1, range::back), 65);
        BOOST_CHECK (range::empty (range::drop (a1, 2, range::back)));
    }

    static_assert (std::is_same <
        std::result_of <callable::make_any_range (
            std::list <double> const &)>::type,
        any_range <double const &, map <
            map_element <default_direction, direction::front>,
            map_element <copy_construct, void>,
            map_element <direction::front, set <
                empty, first, drop_one, chop_destructive>>,
            map_element <direction::back, set <
                empty, first, drop_one, chop_destructive>>
        >> >::value, "");

    typedef range::function_range <int (*) ()> function_range;

    static_assert (std::is_same <
        std::result_of <callable::make_any_range (function_range &&)>::type,
        any_range <int, map <
            map_element <default_direction, direction::front>,
            map_element <direction::front, set <empty, chop_destructive>>
        >> >::value, "");

    {
        struct count {
            int i;
            count() : i (0) {}

            int operator() () { return ++i; }
        };

        auto a = make_any_range (range::make_function_range (count()));

        BOOST_CHECK_EQUAL (range::chop_in_place (a), 1);
        BOOST_CHECK_EQUAL (range::chop_in_place (a), 2);
    }

    // Heterogeneous: only if there is at least one element.
    static_assert (std::is_same <
        std::result_of <callable::make_any_range (range::tuple <int> &)>::type,
        any_range <int &, map <
            map_element <default_direction, direction::front>,
            map_element <copy_construct, void>,
            map_element <direction::front, set <
                empty, size, first, drop_one, chop_destructive>>,
            map_element <direction::back, set <
                empty, size, first, drop_one, chop_destructive>>
        >> >::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
