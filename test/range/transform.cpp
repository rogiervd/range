/*
Copyright 2013 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_transform
#include "../boost_unit_test.hpp"

#include "range/transform.hpp"

#include <type_traits>
#include <vector>
#include <list>
#include <tuple>

#include <boost/mpl/assert.hpp>

#include "range/std.hpp"

#include "check_equal.hpp"
#include "weird_count.hpp"

BOOST_AUTO_TEST_SUITE(test_range_transform)

using range::transform;
using range::front;
using range::back;

using range::default_direction;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::at;
using range::is_homogeneous;

struct callable_twice {
    template <class Argument>
        Argument operator() (Argument const & argument) const
    { return argument + argument; }
};

callable_twice twice;

struct callable_duplicate {
    template <class Argument>
        std::tuple <Argument, Argument>
            operator() (Argument const & argument) const
    { return std::make_tuple (argument, argument); }
};

callable_duplicate duplicate;

struct callable_point {
    template <class Argument>
        typename std::add_pointer <Argument>::type
            operator() (Argument && argument) const
    { return &argument; }
};
// \return Pointer to the argument.
callable_point point;

BOOST_AUTO_TEST_CASE (test_range_transform) {
    {
        std::tuple <> t;
        auto v = transform (duplicate, t);
        auto direction = default_direction (v);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (direction), direction::front>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (v)>));

        RIME_CHECK_EQUAL (empty (v), rime::true_);
        RIME_CHECK_EQUAL (size (v), rime::size_t <0u>());

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::first (decltype (v))>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::drop (decltype (v))>));
    }
    {
        std::tuple <int> t (7);
        auto v = transform (duplicate, t);
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (v)>));
        // v should have elements:
        // std::tuple <int, int> (7, 7)

        RIME_CHECK_EQUAL (empty (v), rime::false_);
        RIME_CHECK_EQUAL (size (v), rime::size_t <1u>());

        BOOST_MPL_ASSERT ((range::has <range::callable::first (decltype (v))>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (decltype (v))>));

        auto f = range::first (v);
        BOOST_MPL_ASSERT ((std::is_same <decltype (f), std::tuple <int, int>>));
        BOOST_CHECK_EQUAL (std::get <0> (f), 7);
        BOOST_CHECK_EQUAL (std::get <1> (f), 7);

        RIME_CHECK_EQUAL (empty (drop (v)), rime::true_);
    }
    {
        std::tuple <int, char, double> t (7, 'a', 9.25);
        {
            auto v = transform (duplicate, t);
            BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (v)>));
            // v should have elements:
            // std::tuple <int, int> (7, 7)
            // std::tuple <char, char> ('a', 'a')
            // std::tuple <double, double> (9.25, 9.25)

            RIME_CHECK_EQUAL (empty (v), rime::false_);
            RIME_CHECK_EQUAL (size (v), rime::size_t <3u>());

            BOOST_MPL_ASSERT ((
                range::has <range::callable::first (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (decltype (v))>));

            auto e1 = range::first (v);
            BOOST_MPL_ASSERT ((
                std::is_same <decltype (e1), std::tuple <int, int>>));
            BOOST_CHECK_EQUAL (std::get <0> (e1), 7);
            BOOST_CHECK_EQUAL (std::get <1> (e1), 7);

            auto e2 = range::first (drop (v));
            BOOST_MPL_ASSERT ((
                std::is_same <decltype (e2), std::tuple <char, char>>));
            BOOST_CHECK_EQUAL (std::get <0> (e2), 'a');
            BOOST_CHECK_EQUAL (std::get <1> (e2), 'a');

            auto e3 = range::first (drop (rime::size_t <2>(), v));
            BOOST_MPL_ASSERT ((
                std::is_same <decltype (e3), std::tuple <double, double>>));
            BOOST_CHECK_EQUAL (std::get <0> (e3), 9.25);
            BOOST_CHECK_EQUAL (std::get <1> (e3), 9.25);

            RIME_CHECK_EQUAL (
                empty (drop (rime::size_t <3>(), v)), rime::true_);
        }
        {
            /*
            Convert the tuple into a range of pointers to its element.
            Not that this is pretty neat.
            Conceptually, just because "point" is defined to return pointers,
            suddently v has a type equivalent to
                tuple <int &, char &, double &>
            but then on-the-fly.
            */
            auto v = transform (point, t);

            // Should be pointing at the actual element.
            BOOST_CHECK_EQUAL (first (v), &first (t));

            // Change original element through transform_view.
            // (This may be bad form, but it is supported.)
            *at (rime::size_t <2>(), v) = 4.5;
            BOOST_CHECK_EQUAL (first (back, t), 4.5);
        }
    }
}

BOOST_AUTO_TEST_CASE (test_range_transform_homogeneous) {
    {
        std::vector <double> c;
        c.push_back (6);
        c.push_back (10.5);
        c.push_back (-8);
        {
            auto v = transform (twice, c);
            BOOST_MPL_ASSERT ((is_homogeneous <decltype (v)>));
            // v should have elements 12, 21, -16.

            BOOST_MPL_ASSERT ((
                range::has <range::callable::empty (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::size (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::first (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (int, decltype (v))>));

            BOOST_CHECK (!empty (v));
            BOOST_CHECK_EQUAL (size (v), 3u);

            BOOST_CHECK_EQUAL (first (v), 12.);
            BOOST_CHECK_EQUAL (at (1, v), 21.);
            BOOST_CHECK_EQUAL (at (2, v), -16.);
            BOOST_CHECK_EQUAL (first (back, v), -16.);
            BOOST_CHECK_EQUAL (at (back, 1, v), 21.);
            BOOST_CHECK_EQUAL (at (back, 2, v), 12.);
        }
        {
            // Transform the container into a range of pointers to elements.
            auto v = transform (point, c);

            BOOST_CHECK_EQUAL (first (v), &first (c));
            BOOST_CHECK_EQUAL (at (1, v), &at (1, c));

            *first (v) = 27.5;
            BOOST_CHECK_EQUAL (first (c), 27.5);
        }
    }

    // std::list.
    {
        std::list <double> c;
        c.push_back (6);
        c.push_back (10.5);
        c.push_back (-8);
        {
            auto v = transform (twice, c);
            BOOST_MPL_ASSERT ((is_homogeneous <decltype (v)>));
            // v should have elements 12, 21, -16.

            BOOST_MPL_ASSERT ((
                range::has <range::callable::empty (decltype (v))>));
            BOOST_MPL_ASSERT_NOT ((
                range::has <range::callable::size (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::first (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (decltype (v))>));
            BOOST_MPL_ASSERT_NOT ((
                range::has <range::callable::drop (int, decltype (v))>));

            BOOST_CHECK (!empty (v));

            BOOST_CHECK_EQUAL (first (v), 12.);
            BOOST_CHECK_EQUAL (first (drop (v)), 21.);
            BOOST_CHECK_EQUAL (first (drop (drop (v))), -16.);
            BOOST_CHECK_EQUAL (first (back, v), -16.);
            BOOST_CHECK_EQUAL (first (back, drop (back, v)), 21.);
            BOOST_CHECK_EQUAL (first (back, drop (back, drop (back, v))), 12.);
        }
    }

    // Stacked transforms.
    {
        std::vector <double> c;
        c.push_back (6);
        c.push_back (10.5);
        c.push_back (-8);

        auto v = transform (duplicate, transform (twice, c));
        // v should contain (12, 12), (21, 21), (-16, -16).

        BOOST_CHECK_EQUAL (size (v), 3u);
        BOOST_CHECK (first (v) == std::make_tuple (12., 12.));
        BOOST_CHECK (first (drop (v)) == std::make_tuple (21., 21.));
        BOOST_CHECK (first (back, v) == std::make_tuple (-16., -16.));
    }
}

BOOST_AUTO_TEST_CASE (test_range_transform_weird_count) {
    {
        weird_count w;
        weird_direction direction (7);

        auto v = transform (weird_direction (7), twice, w);

        BOOST_MPL_ASSERT ((std::is_same <range::result_of <
                range::callable::default_direction (decltype (v))>::type,
            forgotten_to_define_direction>));

        BOOST_CHECK (!empty (direction, v));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::size (weird_direction, decltype (v))>));

        BOOST_CHECK_EQUAL (first (direction, v), 0);
        BOOST_CHECK_EQUAL (first (direction, drop (direction, v)), 2);
        BOOST_CHECK_EQUAL (first (direction, drop (direction, 5, v)), 10);
    }
    // Should work with views as well.
    {
        weird_count w;
        weird_direction direction (7);

        auto view = range::view (direction, w);
        auto transformed = transform (direction, duplicate, view);

        BOOST_CHECK (!empty (direction, transformed));

        BOOST_CHECK (first (direction, transformed) == std::make_tuple (0, 0));
        BOOST_CHECK (first (direction, drop (direction, 2, transformed))
            == std::make_tuple (2, 2));
    }
}

BOOST_AUTO_TEST_SUITE_END()

