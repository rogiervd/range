/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_detail_fold_result
#include "utility/test/boost_unit_test.hpp"

#include "range/detail/fold_result.hpp"

#include <boost/mpl/assert.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>

#include <tuple>
#include <vector>

#include "range/std.hpp"
#include "range/take.hpp"

struct a {};
struct b {};

struct return_different_type {
    char operator() (int, int);
    long operator() (char, a);
    a operator() (long, b);
    b operator() (a, char);
};

struct return_different_type_2 {
    char operator() (int, int);
    long operator() (char, int);
    a operator() (long, int);
    b operator() (a, int);
    b operator() (b, int);
};

struct return_same_type {
    int operator() (a, b);
    int operator() (int, b);
};

struct uncallable {};

BOOST_AUTO_TEST_SUITE(test_range_detail_fold_result)

BOOST_AUTO_TEST_CASE (test_fold_result) {
    {
        std::vector <b> vec;
        auto v = range::view (vec);

        BOOST_MPL_ASSERT ((std::is_same <
            range::fold_detail::all_result_types <
                direction::front, return_same_type, a, decltype (v)>::type,
            meta::set <a, int>>));
    }
    {
        std::vector <char> vec;
        auto v = range::view (vec);

        BOOST_MPL_ASSERT ((std::is_same <
            range::fold_detail::all_result_types <direction::front,
                return_different_type_2, int, decltype (v)>::type,
            meta::set <int, char, long, a, b>>));
    }

    // Initially nonhomogeneous, because we start with std::vector.
    // It should normally have been converted into a view already, but here that
    // happens only on the second step.
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_same_type, int, std::vector <b>>::type,
        meta::set <int>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_same_type, int const &, std::vector <b>>::type,
        meta::set <int const &, int>>));

    // Tuple: it's known exactly where this ends.
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_same_type, a, std::tuple <>>::type,
        meta::set <a>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_same_type, a, std::tuple <b>>::type,
        meta::set <int>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_same_type, a, std::tuple <b, b>>::type,
        meta::set <int>>));

    // Empty tuple.
    // If the tuple is empty, operator() should not even be instantiated on the
    // function.
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            uncallable, a, std::tuple<>>::type,
        meta::set <a>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_different_type, int, std::tuple<>>::type,
        meta::set <int>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_different_type, int, std::tuple <int>>::type,
        meta::set <char>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_different_type, int, std::tuple <int, a>>::type,
        meta::set <long>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_different_type, int, std::tuple <int, a, b>>::type,
        meta::set <a>>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::fold_detail::all_result_types <direction::front,
            return_different_type, int, std::tuple <int, a, b, char>>::type,
        meta::set <b>>));

    {
        // It is unknown at compile time when this tuple ends.
        auto r = range::take (2, std::make_tuple (b(), b()));
        BOOST_MPL_ASSERT ((std::is_same <
            range::fold_detail::all_result_types <
                direction::front, return_same_type, a, decltype (r)>::type,
            meta::set <a, int>>));
    }
}

BOOST_AUTO_TEST_SUITE_END()
