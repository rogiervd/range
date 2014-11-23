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

#define BOOST_TEST_MODULE test_range_for_each
#include "utility/test/boost_unit_test.hpp"

#include "range/for_each.hpp"

#include <vector>
#include <tuple>

#include <boost/mpl/assert.hpp>

#include "range/std.hpp"

#include "weird_direction.hpp"

using range::for_each;
using range::at;

BOOST_AUTO_TEST_SUITE(test_range_for_each)

struct plus {
    int operator() (int a, int b) const { return a + b; }
};

struct count {
    int number;
    count() : number (0) {}

    template <class Element> void operator() (Element const &)
    { ++ number; }
};

struct add_one {
    template <class Element> void operator() (Element & element) const
    { element += 1; }
};

BOOST_AUTO_TEST_CASE (has) {
    BOOST_MPL_ASSERT ((range::has <range::callable::for_each (
        plus, std::vector <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::for_each (
        direction::front, plus, std::vector <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::for_each (
        direction::back, plus, std::vector <int>)>));

    // BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
    //     weird_direction, plus, std::vector <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (float)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
        std::vector <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
        float, plus, std::vector <int>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
        int, plus, std::vector <int>)>));
}

BOOST_AUTO_TEST_CASE (homogeneous) {
    count c;

    BOOST_CHECK_EQUAL (c.number, 0);

    std::vector <int> v;

    for_each (c, v);
    BOOST_CHECK_EQUAL (c.number, 0);

    v.push_back (27);
    for_each (c, v);
    BOOST_CHECK_EQUAL (c.number, 1);

    v.push_back (32);
    for_each (c, v);
    BOOST_CHECK_EQUAL (c.number, 3);

    for_each (add_one(), v);
    BOOST_CHECK_EQUAL (at (0, v), 28);
    BOOST_CHECK_EQUAL (at (1, v), 33);
}

BOOST_AUTO_TEST_CASE (heterogeneous) {
    count c;

    BOOST_CHECK_EQUAL (c.number, 0);

    {
        std::tuple <> v;

        for_each (c, v);
        BOOST_CHECK_EQUAL (c.number, 0);
    }

    {
        std::tuple <int> v (27);

        for_each (c, v);
        BOOST_CHECK_EQUAL (c.number, 1);
    }

    {
        std::tuple <int, short> v (27, 32);

        for_each (c, v);
        BOOST_CHECK_EQUAL (c.number, 3);

        for_each (add_one(), v);
        BOOST_CHECK_EQUAL (at (rime::size_t <0>(), v), 28);
        BOOST_CHECK_EQUAL (at (rime::size_t <1>(), v), 33);
    }
}

BOOST_AUTO_TEST_SUITE_END()
