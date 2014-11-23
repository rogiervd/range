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

#include <boost/mpl/assert.hpp>
#include <boost/any.hpp>

#include "range/tuple.hpp"

#include "weird_direction.hpp"

using range::tuple;

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
        plus, tuple <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::for_each (
        direction::front, plus, tuple <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::for_each (
        direction::back, plus, tuple <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
        weird_direction, plus, tuple <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (float)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
        tuple <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
        float, plus, tuple <int>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::for_each (
        int, plus, tuple <int>)>));
}

BOOST_AUTO_TEST_CASE (add) {
    count c;

    BOOST_CHECK_EQUAL (c.number, 0);

    {
        tuple <> v;

        for_each (c, v);
        BOOST_CHECK_EQUAL (c.number, 0);
    }

    {
        tuple <int> v (27);

        for_each (c, v);
        BOOST_CHECK_EQUAL (c.number, 1);
    }

    {
        tuple <int, short> v (27, 32);

        for_each (c, v);
        BOOST_CHECK_EQUAL (c.number, 3);

        for_each (add_one(), v);
        BOOST_CHECK_EQUAL (at (rime::size_t <0>(), v), 28);
        BOOST_CHECK_EQUAL (at (rime::size_t <1>(), v), 33);
    }
}

/*
Check that all elements get passed in correctly.
Since the loop is manually unrolled, it is important to check up to the number
that is manually unrolled.
*/

// Collect all elements in a vector of boost::any.
struct collect_any {
    std::vector <boost::any> elements;

    template <class Element> void operator() (Element const & element)
    { elements.push_back (boost::any (element)); }
};

BOOST_AUTO_TEST_CASE (unrolled) {
    // Tuple with 0 elements.
    {
        tuple<> t;
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 0);
    }
    // Tuple with 1 elements.
    {
        tuple <double> t (87.5);
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 1);
        BOOST_CHECK_EQUAL (boost::any_cast <double> (c.elements[0]), 87.5);
    }
    // Tuple with 2 elements.
    {
        tuple <double, short> t (87.5, 43);
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 2);
        BOOST_CHECK_EQUAL (boost::any_cast <double> (c.elements [0]), 87.5);
        BOOST_CHECK_EQUAL (boost::any_cast <short> (c.elements [1]), 43);
    }
    // Tuple with 3 elements.
    {
        tuple <double, short, bool> t (87.5, 43, true);
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 3);
        BOOST_CHECK_EQUAL (boost::any_cast <double> (c.elements [0]), 87.5);
        BOOST_CHECK_EQUAL (boost::any_cast <short> (c.elements [1]), 43);
        BOOST_CHECK_EQUAL (boost::any_cast <bool> (c.elements [2]), true);
    }
    // Tuple with 4 elements.
    {
        tuple <double, short, bool, int> t (87.5, 43, true, -5);
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 4);
        BOOST_CHECK_EQUAL (boost::any_cast <double> (c.elements [0]), 87.5);
        BOOST_CHECK_EQUAL (boost::any_cast <short> (c.elements [1]), 43);
        BOOST_CHECK_EQUAL (boost::any_cast <bool> (c.elements [2]), true);
        BOOST_CHECK_EQUAL (boost::any_cast <int> (c.elements [3]), -5);
    }
    // Tuple with 5 elements.
    {
        tuple <double, short, bool, int, unsigned> t (87.5, 43, true, -5, 900u);
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 5);
        BOOST_CHECK_EQUAL (boost::any_cast <double> (c.elements [0]), 87.5);
        BOOST_CHECK_EQUAL (boost::any_cast <short> (c.elements [1]), 43);
        BOOST_CHECK_EQUAL (boost::any_cast <bool> (c.elements [2]), true);
        BOOST_CHECK_EQUAL (boost::any_cast <int> (c.elements [3]), -5);
        BOOST_CHECK_EQUAL (boost::any_cast <unsigned> (c.elements [4]), 900u);
    }
    // Tuple with 8 elements.
    {
        tuple <double, short, bool, int, unsigned, unsigned short, float, char>
            t (87.5, 43, true, -5, 900u, 30u, 4.5, 'A');
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 8);
        BOOST_CHECK_EQUAL (boost::any_cast <double> (c.elements [0]), 87.5);
        BOOST_CHECK_EQUAL (boost::any_cast <short> (c.elements [1]), 43);
        BOOST_CHECK_EQUAL (boost::any_cast <bool> (c.elements [2]), true);
        BOOST_CHECK_EQUAL (boost::any_cast <int> (c.elements [3]), -5);
        BOOST_CHECK_EQUAL (boost::any_cast <unsigned> (c.elements [4]), 900u);
        BOOST_CHECK_EQUAL (boost::any_cast <unsigned short> (c.elements [5]),
            30u);
        BOOST_CHECK_EQUAL (boost::any_cast <float> (c.elements [6]), 4.5f);
        BOOST_CHECK_EQUAL (boost::any_cast <char> (c.elements [7]), 'A');
    }
    // Tuple with 9 elements.
    {
        tuple <double, short, bool, int, unsigned, unsigned short, float, char,
                unsigned char>
            t (87.5, 43, true, -5, 900u, 30u, 4.5, 'A', 'q');
        collect_any c;
        for_each (c, t);
        BOOST_CHECK_EQUAL (c.elements.size(), 9);
        BOOST_CHECK_EQUAL (boost::any_cast <double> (c.elements [0]), 87.5);
        BOOST_CHECK_EQUAL (boost::any_cast <short> (c.elements [1]), 43);
        BOOST_CHECK_EQUAL (boost::any_cast <bool> (c.elements [2]), true);
        BOOST_CHECK_EQUAL (boost::any_cast <int> (c.elements [3]), -5);
        BOOST_CHECK_EQUAL (boost::any_cast <unsigned> (c.elements [4]), 900u);
        BOOST_CHECK_EQUAL (boost::any_cast <unsigned short> (c.elements [5]),
            30u);
        BOOST_CHECK_EQUAL (boost::any_cast <float> (c.elements [6]), 4.5f);
        BOOST_CHECK_EQUAL (boost::any_cast <char> (c.elements [7]), 'A');
        BOOST_CHECK_EQUAL (boost::any_cast <unsigned char> (c.elements [8]),
            'q');
    }
}

BOOST_AUTO_TEST_SUITE_END()
