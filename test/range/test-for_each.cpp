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
