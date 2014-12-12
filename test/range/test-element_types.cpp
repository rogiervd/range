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

#define BOOST_TEST_MODULE test_range_element_types
#include "utility/test/boost_unit_test.hpp"

#include "range/element_types.hpp"

#include <type_traits>
#include <list>
#include <tuple>

#include "meta/vector.hpp"

#include "range/std.hpp"
#include "range/take.hpp"

// For debugging.
template <class ... Types> struct show_types;

BOOST_AUTO_TEST_SUITE(test_range_element_types)

BOOST_AUTO_TEST_CASE (test_element_types_contents) {
    {
        typedef std::tuple<> tuple;

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple>>::type,
            meta::vector<>>::value, "");

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple &>>::type,
            meta::vector<>>::value, "");

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple const &>>::type,
            meta::vector<>>::value, "");
    }
    {
        typedef std::tuple <int> tuple;

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple>>::type,
            meta::vector <int &&>>::value, "");

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple &>>::type,
            meta::vector <int &>>::value, "");

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple const &>>::type,
            meta::vector <int const &>>::value, "");
    }
    {
        typedef std::tuple <int, int, int const &, float> tuple;

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple>>::type,
            meta::vector <int &&, int &&, int const &, float &&>
            >::value, "");

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple &>>::type,
            meta::vector <int &, int &, int const &, float &>
            >::value, "");

        static_assert (std::is_same <
            meta::as_vector <range::element_types <tuple const &>>::type,
            meta::vector <
                int const &, int const &, int const &, float const &>
            >::value, "");
    }
}

BOOST_AUTO_TEST_CASE (test_element_types_behaviour) {
    {
        typedef meta::as_vector <range::element_types <std::tuple<>>>::type
            types;

        static_assert (meta::empty <types>::value, "");
        static_assert (meta::size <types>::value == 0, "");

        static_assert (meta::empty <direction::front, types>::value, "");
        static_assert (meta::size <direction::front, types>::value == 0, "");

        static_assert (meta::empty <direction::back, types>::value, "");
        static_assert (meta::size <direction::back, types>::value == 0, "");
    }
    {
        typedef range::element_types <std::tuple <float>> types;

        static_assert (!meta::empty <types>::value, "");
        static_assert (meta::size <types>::value == 1, "");

        static_assert (!meta::empty <direction::front, types>::value, "");
        static_assert (meta::size <direction::front, types>::value == 1, "");

        static_assert (!meta::empty <direction::back, types>::value, "");
        static_assert (meta::size <direction::back, types>::value == 1, "");

        static_assert (std::is_same <
            meta::first <types>::type, float &&>::value, "");
        static_assert (std::is_same <
            meta::first <direction::front, types>::type, float &&>::value, "");
        static_assert (std::is_same <
            meta::first <direction::back, types>::type, float &&>::value, "");

        static_assert (meta::empty <meta::drop <types>::type>::value, "");
        static_assert (meta::empty <
            meta::drop <direction::front, types>::type>::value, "");
        static_assert (meta::empty <
            meta::drop <direction::back, types>::type>::value, "");
    }
    {
        typedef range::element_types <std::tuple <float, bool, int>> types;

        static_assert (!meta::empty <types>::value, "");
        static_assert (meta::size <types>::value == 3, "");

        static_assert (!meta::empty <direction::front, types>::value, "");
        static_assert (meta::size <direction::front, types>::value == 3, "");

        static_assert (!meta::empty <direction::back, types>::value, "");
        static_assert (meta::size <direction::back, types>::value == 3, "");

        static_assert (std::is_same <
            meta::first <types>::type, float &&>::value, "");
        static_assert (std::is_same <
            meta::first <direction::front, types>::type, float &&>::value, "");
        static_assert (std::is_same <
            meta::first <direction::back, types>::type, int &&>::value, "");

        static_assert (!meta::empty <meta::drop <types>::type>::value, "");
        static_assert (meta::empty <meta::drop <
            direction::front, rime::size_t <3>, types>::type>::value, "");

        static_assert (std::is_same <
            meta::first <meta::drop <types>::type>::type, bool &&>::value, "");
    }
}

template <class ... Types> struct show_types;

// Used on a homogeneous type, element_types becomes an infinite meta-range.
BOOST_AUTO_TEST_CASE (test_element_types_homogeneous) {
    {
        typedef range::element_types <std::vector <int> &> types;

        static_assert (std::is_same <
            meta::first <types>::type, int &>::value, "");
        static_assert (std::is_same <
            meta::first <meta::drop <types>::type>::type, int &>::value, "");
        static_assert (std::is_same <
            meta::first <meta::drop <rime::size_t <34>, types>::type>::type,
            int &>::value, "");

        // After one call to "drop", the range is turned into a view.
        typedef meta::drop <types>::type view;
        static_assert (std::is_same <meta::drop <view>::type, view>::value, "");
        // It then becomes heterogeneous.
        static_assert (std::is_same <
            meta::drop <view>::type, view>::value, "");
        static_assert (std::is_same <
            meta::drop <rime::size_t <3>, view>::type, view>::value, "");
    }
    {
        std::list <int> l;
        auto v = range::take (rime::size_t <2>(), l);
        typedef range::element_types <decltype (v)> types;

        static_assert (std::is_same <
            meta::first <types>::type, int &>::value, "");
        static_assert (std::is_same <
            meta::first <meta::drop <types>::type>::type, int &>::value, "");

        static_assert (!meta::empty <types>::value, "");
        static_assert (!meta::empty <meta::drop <types>::type>::value, "");
        static_assert (meta::empty <
            meta::drop <meta::drop <types>::type>::type>::value, "");
    }
}

BOOST_AUTO_TEST_SUITE_END()
