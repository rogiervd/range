/*
Copyright 2014 Rogier van Dalen.

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
