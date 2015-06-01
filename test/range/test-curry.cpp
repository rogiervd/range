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

#define BOOST_TEST_MODULE test_range_curry
#include "utility/test/boost_unit_test.hpp"

#include "range/curry.hpp"

#include <vector>

#include "range/tuple.hpp"
#include "range/std.hpp"

using range::tuple;

BOOST_AUTO_TEST_SUITE(test_range_curry)

struct take_4_true_tuple {
    int operator() (int i, bool b, tuple<> t) const{
        BOOST_CHECK_EQUAL (i, 4);
        BOOST_CHECK_EQUAL (b, true);
        return 71;
    }

    int operator() (int i, bool b, tuple <int> t) const{
        BOOST_CHECK_EQUAL (i, 4);
        BOOST_CHECK_EQUAL (b, true);
        return range::first (t);
    }

    double operator() (tuple <double> t) const { return range::first (t); }

    void operator() (double & target, tuple <double> source) const
    { target = range::first (source); }
};

BOOST_AUTO_TEST_CASE (test_range_callable_curried) {
    range::callable::curried <take_4_true_tuple> f;
    {
        auto f2 = f (4);
        auto f3 = f2();
        auto f4 = f3 (true);
        auto result = f4 (tuple<>());

        static_assert (std::is_same <decltype (result), int>::value, "");
        BOOST_CHECK_EQUAL (result, 71);
    }
    {
        auto f2 =  f (4, true);
        auto result = f2 (tuple <int> (27));

        static_assert (std::is_same <decltype (result), int>::value, "");
        BOOST_CHECK_EQUAL (result, 27);
    }
    {
        auto result = f (tuple <double> (8.25));

        static_assert (std::is_same <decltype (result), double>::value, "");
        BOOST_CHECK_EQUAL (result, 8.25);
    }
    {
        double d = 2.0;
        // Hold a reference.
        auto f2 = f (std::ref (d));
        // Assign to d.
        f2 (tuple <double> (8.25));

        BOOST_CHECK_EQUAL (d, 8.25);
    }
}

BOOST_AUTO_TEST_CASE (test_range_curry) {
    std::vector <int> v;

    auto empty_back = range::curry::empty (range::back);
    BOOST_CHECK (empty_back (v));

    auto size_back = range::curry::size (range::back);
    BOOST_CHECK_EQUAL (size_back (v), 0);
    v.push_back (1);
    BOOST_CHECK_EQUAL (size_back (v), 1);

    v.push_back (7);
    auto first_back = range::curry::first (range::back);
    BOOST_CHECK_EQUAL (first_back (v), 7);

    auto drop_1 = range::curry::drop (1);
    auto drop_2 = range::curry::drop (2);
    BOOST_CHECK_EQUAL (range::first (drop_1 (v)), 7);
    BOOST_CHECK (empty_back (drop_2 (v)));
}

BOOST_AUTO_TEST_SUITE_END()
