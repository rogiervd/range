/*
Copyright 2014, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_zip
#include "utility/test/boost_unit_test.hpp"

#include "range/zip.hpp"

#include <vector>
#include <string>

#include "rime/check/check_equal.hpp"

#include "range/tuple.hpp"
#include "range/std.hpp"
#include "range/for_each_macro.hpp"

#include "weird_direction.hpp"
#include "weird_count.hpp"

#include "unique_range.hpp"

using range::tuple;

template <class ... Types> struct show_types;

using range::zip;
using range::zip_from;
using range::view;

using range::empty;
using range::size;
using range::first;
using range::second;
using range::third;

using range::drop;
using range::chop;
using range::chop_in_place;

using range::make_tuple;

using range::has;
namespace callable = range::callable;

BOOST_AUTO_TEST_SUITE(test_range_zip)

BOOST_AUTO_TEST_CASE (homogeneous) {
    std::vector <int> vi;
    std::vector <double> vd;

    {
        auto z = range::zip (vi, vd);

        static_assert (std::is_same <decltype (z),
            range::zip_range <direction::front,
                decltype (view (vi)), decltype (view (vd))>
            >::value, "");

        static_assert (range::is_range <decltype (z)>::value, "");
        static_assert (range::is_homogeneous <
            direction::front, decltype (z)>::value, "");
        BOOST_CHECK (range::default_direction (z) == range::front);

        BOOST_CHECK (range::empty (z));
        BOOST_CHECK (range::empty (range::front, z));

        BOOST_CHECK_EQUAL (range::size (z), 0);
        BOOST_CHECK_EQUAL (range::size (range::front, z), 0);

        static_assert (has <callable::drop (decltype (z))>::value, "");
        static_assert (has <callable::drop (direction::front, decltype (z))
            >::value, "");
        static_assert (!has <callable::drop (direction::back, decltype (z))
            >::value, "");

        static_assert (has <callable::chop (decltype (z))>::value, "");
        static_assert (has <callable::chop (direction::front, decltype (z))
            >::value, "");
        static_assert (!has <callable::chop (direction::back, decltype (z))
            >::value, "");
    }

    vi.push_back (1);
    vd.push_back (2.5);

    BOOST_CHECK_EQUAL (first (vi), 1);

    {
        auto z = zip (vi, vd);

        auto first_element = first (z);

        BOOST_CHECK_EQUAL (range::size (z), 1);

        BOOST_CHECK_EQUAL (first (first_element), 1);
        BOOST_CHECK_EQUAL (second (first_element), 2.5);

        auto z2 = drop (z);

        BOOST_CHECK (empty (first (z2.underlying())));
        BOOST_CHECK (empty (second (z2.underlying())));

        BOOST_CHECK (empty (z2));

        auto z3 = drop (1, z);
        BOOST_CHECK (empty (first (z3.underlying())));
        BOOST_CHECK (empty (second (z3.underlying())));

        BOOST_CHECK (empty (z3));

        // auto chopped = chop (z);
        // RIME_CHECK_EQUAL (first (chopped.first()), 1);
        // RIME_CHECK_EQUAL (second (chopped.first()), 2.5);
        // BOOST_CHECK (empty (chopped.rest()));
    }

    vi.push_back (3);
    vi.push_back (27);

    vd.push_back (5.5);
    vd.push_back (27.5);
    // This makes vd one longer than vi, so it is never seen from the front.
    vd.push_back (43.5);

    // Test zip_from.
    {
        std::tuple <std::vector <int>, std::vector <double>> t (vi, vd);
        {
            auto z = zip_from (t);

            auto first_element = first (z);

            BOOST_CHECK_EQUAL (first (first_element), 1);
            BOOST_CHECK_EQUAL (second (first_element), 2.5);
        }
        // front explicitly specified.
        {
            auto z = zip_from (range::front, t);

            auto first_element = first (z);

            static_assert (range::has <
                    range::callable::chop (decltype (drop (z)) const &)
                >::value, "");

            BOOST_CHECK_EQUAL (first (first_element), 1);
            BOOST_CHECK_EQUAL (second (first_element), 2.5);
        }
        // From the back.
        {
            auto z = zip_from (range::back, t);

            auto first_element = first (z);

            BOOST_CHECK_EQUAL (first (first_element), 27);
            BOOST_CHECK_EQUAL (second (first_element), 43.5);

            auto chopped = chop (drop (z));

            static_assert (range::has <
                    range::callable::chop (decltype (zip_from (t)))
                >::value, "");
            // show_types <decltype (z)> st;
            // show_types <decltype (range::default_direction (z))> st2;
            static_assert (range::has <
                    range::callable::chop (decltype (zip_from (range::back, t)))
                >::value, "");

            BOOST_CHECK_EQUAL (first (chopped.first()), 3);
            BOOST_CHECK_EQUAL (second (chopped.first()), 27.5);

            BOOST_CHECK (empty (drop (chopped.rest())));
        }
    }

    {
        auto z = zip (vi, vd);

        auto first_element = first (z);

        BOOST_CHECK_EQUAL (range::size (first (z.underlying())), 3);
        BOOST_CHECK_EQUAL (range::size (second (z.underlying())), 4);

        BOOST_CHECK_EQUAL (range::size (z), 3);

        BOOST_CHECK_EQUAL (first (first_element), 1);
        BOOST_CHECK_EQUAL (second (first_element), 2.5);

        BOOST_CHECK_EQUAL (first (second (z)), 3);
        BOOST_CHECK_EQUAL (second (second (z)), 5.5);

        BOOST_CHECK_EQUAL (first (third (z)), 27);
        BOOST_CHECK_EQUAL (second (third (z)), 27.5);

        auto z2 = drop (2, z);
        BOOST_CHECK_EQUAL (first (first (z2)), 27);
        BOOST_CHECK_EQUAL (second (first (z2)), 27.5);

        BOOST_CHECK (empty (drop (3, z)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
