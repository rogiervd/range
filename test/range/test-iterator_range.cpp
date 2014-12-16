/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_iterator_range
#include "utility/test/boost_unit_test.hpp"

#include "range/iterator_range.hpp"

#include <type_traits>
#include <vector>
#include <list>
#include <forward_list>
#include <string>
#include <sstream>
#include <iterator>

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_range_iterator_range)

typedef std::vector <int> random_access_container_type;
typedef std::list <double> bidirectional_container_type;
typedef std::forward_list <char> forward_container_type;
typedef std::istream_iterator <float> input_iterator_type;

typedef range::iterator_range <random_access_container_type::iterator>
    random_access_type;
typedef range::iterator_range <bidirectional_container_type::iterator>
    bidirectional_type;
typedef range::iterator_range <bidirectional_container_type::const_iterator>
    const_bidirectional_type;
typedef range::iterator_range <forward_container_type::iterator>
    forward_type;
typedef range::iterator_range <input_iterator_type> input_type;

// If the standard library used has a debug iterator mode and it is switched on,
// the following will trigger an error.
// Even normal debugging does not normally switch on this mode, because it is so
// slow.
// The Jamfile in this directory tests this file with iterator debug mode on
// where it is available.
/*
BOOST_AUTO_TEST_CASE (test_range_iterator_debug_mode) {
    std::vector <int> v;
    std::vector <int>::iterator i = v.begin();
    // Increment an iterator beyond its limits.
    ++ i;
}
*/

BOOST_AUTO_TEST_CASE (test_range_iterator_range_static) {
    // is_homogeneous
    BOOST_MPL_ASSERT ((range::is_homogeneous <random_access_type>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        direction::front, random_access_type>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        direction::back, random_access_type const>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        direction::front, direction::back, random_access_type>));

    BOOST_MPL_ASSERT ((range::is_homogeneous <bidirectional_type &>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        direction::front, forward_type>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        direction::front, input_type>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        direction::back, const_bidirectional_type>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        direction::front, direction::back, random_access_type const &>));

    // has::
    {
        // view
        BOOST_MPL_ASSERT ((
            range::has <range::callable::view (random_access_type)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::view (int, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::view (
            direction::front, random_access_type const &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::view (
            direction::back &, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::view (
            direction::front const, direction::back &, random_access_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::view (forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::view (input_type)>));

        // empty
        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (random_access_type)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::empty (int, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            direction::front, random_access_type const &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            direction::back, random_access_type)>));

        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (bidirectional_type)>));
        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (const_bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            direction::front, bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            direction::back, bidirectional_type)>));

        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (forward_type &)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (direction::front, forward_type)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (direction::back, forward_type)>));

        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (input_type)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (direction::front, input_type const &)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (direction::back, input_type &&)>));

        // size
        BOOST_MPL_ASSERT ((
            range::has <range::callable::size (random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::size (
            direction::front, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::size (
            direction::back, random_access_type)>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (const_bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            direction::front, bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            direction::back, bidirectional_type const &)>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (forward_type const)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            direction::front, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            direction::back, forward_type)>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (input_type const)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            direction::front, input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            direction::back, input_type)>));

        // first
        BOOST_MPL_ASSERT ((
            range::has <range::callable::first (random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            direction::front, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            direction::back, random_access_type)>));

        BOOST_MPL_ASSERT ((
            range::has <range::callable::first (bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            direction::front, bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            direction::back, bidirectional_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::first (forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            direction::front, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            direction::back, forward_type)>));

        // Only for rvalue ranges.
        BOOST_MPL_ASSERT ((range::has <range::callable::first (input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            direction::front, input_type &&)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            direction::back, input_type)>));

        // Not for lvalue ranges.
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            direction::front, input_type const &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            direction::back, input_type &)>));

        // drop without increment
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::back, random_access_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::back, bidirectional_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, forward_type)>));

        // Only for rvalue input.
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, input_type &&)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, input_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::front, input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, input_type &)>));

        // drop with increment of 1
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            rime::int_ <1>, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, rime::int_ <1>, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::back, rime::int_ <1>, random_access_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            rime::int_ <1>, bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, rime::int_ <1>, bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::back, rime::int_ <1>, bidirectional_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            rime::int_ <1>, forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, rime::int_ <1>, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, rime::int_ <1>, forward_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            rime::int_ <1>, input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, rime::int_ <1>, input_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, rime::int_ <1>, input_type)>));

        // drop with increment of 2
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            rime::int_ <2>, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, rime::int_ <2>, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::back, rime::int_ <2>, random_access_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            rime::int_ <2>, bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::front, rime::int_ <2>, bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, rime::int_ <2>, bidirectional_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            rime::int_ <2>, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::front, rime::int_ <2>, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, rime::int_ <2>, forward_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            rime::int_ <2>, input_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::front, rime::int_ <2>, input_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, rime::int_ <2>, input_type)>));

        // drop with increment of int
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            int, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::front, int, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            direction::back, int, random_access_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            int, bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::front, int, bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, int, bidirectional_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            int, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::front, int, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, int, forward_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            int, input_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::front, int, input_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            direction::back, int, input_type)>));

        // chop.
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            direction::front, random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            direction::back, random_access_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            direction::front, bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            direction::back, bidirectional_type)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            direction::front, forward_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            direction::back, forward_type)>));

        // Only for rvalue input.
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            direction::front, input_type &&)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            direction::back, input_type)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            direction::front, input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            direction::back, input_type &)>));

        // chop_in_place: only for lvalue references.
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            random_access_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            direction::front, random_access_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            direction::back, random_access_type &)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            bidirectional_type &&)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            bidirectional_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            direction::front, bidirectional_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            direction::back, bidirectional_type &)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            forward_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            direction::front, forward_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            direction::back, forward_type &)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            input_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            direction::front, input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            direction::back, input_type &)>));
    }

    // Spot check result_of::
    {
        // view
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (random_access_type)>::type,
            random_access_type>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (direction::back,
            bidirectional_type &)>::type, bidirectional_type &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (direction::front &,
                forward_type const &)>::type, forward_type const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (direction::front &,
                input_type)>::type, input_type>));

        // forward_view.
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (
                random_access_type)>::type, random_access_type &&>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (direction::back,
                bidirectional_type &)>::type, bidirectional_type &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (direction::front &,
                forward_type const &)>::type, forward_type const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (direction::front &,
                input_type)>::type, input_type &&>));

        // empty
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (random_access_type)>::type, bool>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (direction::back, bidirectional_type)>::type,
            bool>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (direction::front, forward_type)>::type,
            bool>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (direction::front, input_type)>::type,
            bool>));

        // size returns an unsigned integer.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::size (random_access_type)>::type, std::size_t>));

        // first
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (random_access_type)>::type, int &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (direction::back, bidirectional_type)>::type,
            double &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (direction::back, const_bidirectional_type
            )>::type, double const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (direction::front, forward_type)>::type,
            char &>));
        // For input ranges, "first" should return the value_type.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (direction::front, input_type)>::type,
            float>));

        // drop
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::drop (random_access_type)>::type,
            random_access_type>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::drop (direction::back, bidirectional_type &
            )>::type, bidirectional_type>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::drop (forward_type)>::type,
            forward_type>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::drop (input_type)>::type,
            input_type>));

        // chop.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop (random_access_type)>::type,
            range::chopped <int &, random_access_type>>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop (direction::back, bidirectional_type &
            )>::type,
            range::chopped <double &, bidirectional_type>>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop (direction::back, const_bidirectional_type &
            )>::type,
            range::chopped <double const &, const_bidirectional_type>>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop (forward_type)>::type,
            range::chopped <char &, forward_type>>));
        // For input ranges, return a value, never a reference.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop (input_type &&)>::type,
            range::chopped <float, input_type>>));

        // chop_in_place returns the same as "first" (and mutates the range).
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (random_access_type &)>::type,
            int &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (
                direction::back, bidirectional_type &)>::type,
            double &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (
                direction::back, const_bidirectional_type &
            )>::type, double const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (
                direction::front, forward_type &)>::type,
            char &>));
        // For input ranges, "chop_in_place" should return the value_type.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (
                direction::front, input_type &)>::type,
            float>));
    }
}

BOOST_AUTO_TEST_CASE (test_range_iterator_range_dynamic) {
    using range::view;
    using range::empty;
    using range::size;
    using range::first;
    using range::drop;
    using range::chop;
    using range::chop_in_place;
    using range::front;
    using range::back;

    random_access_container_type random_access_container;
    bidirectional_container_type bidirectional_container;
    forward_container_type forward_container;
    std::string s ("");

    {
        random_access_type r (
            random_access_container.begin(), random_access_container.end());
        bidirectional_type b (
            bidirectional_container.begin(), bidirectional_container.end());
        const_bidirectional_type cb (
            bidirectional_container.begin(), bidirectional_container.end());
        forward_type f (forward_container.begin(), forward_container.end());

        std::stringstream stream (s);
        auto i = input_type (
            input_iterator_type (stream), input_iterator_type());

        BOOST_CHECK (range::empty (view (r)));
        BOOST_CHECK_EQUAL (range::size (r), 0u);
        BOOST_CHECK (range::empty (b));
        BOOST_CHECK (range::empty (cb));
        BOOST_CHECK (range::empty (view (f)));
        BOOST_CHECK (range::empty (i));

        random_access_type r2 = r;
        bidirectional_type b2 = b;
        const_bidirectional_type cb2 = cb;
        forward_type f2 = f;
        // Copying is disabled for input ranges.
        // input_type i2 = i;
        BOOST_MPL_ASSERT ((
            std::is_constructible <forward_type, forward_type &>));
        BOOST_MPL_ASSERT_NOT ((
            std::is_constructible <input_type, input_type &>));

        BOOST_CHECK (range::empty (view (r2)));
        BOOST_CHECK_EQUAL (range::size (r2), 0u);
        BOOST_CHECK (range::empty (b2));
        BOOST_CHECK (range::empty (cb2));
        BOOST_CHECK (range::empty (view (f2)));
        // BOOST_CHECK (range::empty (i2));

        random_access_type r3 = std::move (r);
        bidirectional_type b3 = std::move (b);
        const_bidirectional_type cb3 = std::move (cb);
        forward_type f3 = std::move (f);
        input_type i3 = std::move (i);

        BOOST_CHECK (range::empty (view (r3)));
        BOOST_CHECK_EQUAL (range::size (r3), 0u);
        BOOST_CHECK (range::empty (b3));
        BOOST_CHECK (range::empty (cb3));
        BOOST_CHECK (range::empty (view (f3)));
        BOOST_CHECK (range::empty (i3));
    }

    random_access_container.push_back (3);
    bidirectional_container.push_back (4.3);
    forward_container.push_front ('9');
    s = "19.5";

    {
        random_access_type r (
            random_access_container.begin(), random_access_container.end());
        bidirectional_type b (
            bidirectional_container.begin(), bidirectional_container.end());
        const_bidirectional_type cb (
            bidirectional_container.begin(), bidirectional_container.end());
        forward_type f (forward_container.begin(), forward_container.end());
        std::stringstream stream (s);
        auto i = input_type (
            input_iterator_type (stream), input_iterator_type());

        BOOST_CHECK (!empty (r));
        BOOST_CHECK_EQUAL (size (r), 1u);
        BOOST_CHECK_EQUAL (first (r), 3);
        BOOST_CHECK_EQUAL (first (back, r), 3);
        BOOST_CHECK (empty (drop (r)));
        BOOST_CHECK (empty (drop (front, r)));
        BOOST_CHECK (empty (drop (back, r)));
        BOOST_CHECK (empty (drop (front, 1, r)));
        BOOST_CHECK (empty (drop (back, 1, r)));
        {
            auto chopped = range::chop (r);
            BOOST_CHECK_EQUAL (chopped.first(), 3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto chopped = range::chop (back, r);
            BOOST_CHECK_EQUAL (chopped.first(), 3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto first = chop_in_place (back, r);
            BOOST_CHECK_EQUAL (first, 3);
            BOOST_CHECK (empty (r));
        }

        BOOST_CHECK (!empty (b));
        BOOST_CHECK_EQUAL (first (view (front, b)), 4.3);
        BOOST_CHECK_EQUAL (first (back, b), 4.3);
        BOOST_CHECK (empty (drop (b)));
        BOOST_CHECK (empty (drop (front, b)));
        BOOST_CHECK (empty (drop (back, b)));
        {
            auto chopped = range::chop (back, b);
            BOOST_CHECK_EQUAL (chopped.first(), 4.3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto chopped = range::chop (b);
            BOOST_CHECK_EQUAL (chopped.first(), 4.3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto first = chop_in_place (b);
            BOOST_CHECK_EQUAL (first, 4.3);
            BOOST_CHECK (empty (b));
        }

        BOOST_CHECK (!empty (cb));
        BOOST_CHECK_EQUAL (first (cb), 4.3);
        BOOST_CHECK_EQUAL (first (back, cb), 4.3);
        BOOST_CHECK (empty (drop (cb)));
        BOOST_CHECK (empty (drop (front, cb)));
        BOOST_CHECK (empty (drop (back, cb)));
        {
            auto chopped = range::chop (back, cb);
            BOOST_CHECK_EQUAL (chopped.first(), 4.3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto chopped = range::chop (cb);
            BOOST_CHECK_EQUAL (chopped.first(), 4.3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto first = chop_in_place (cb);
            BOOST_CHECK_EQUAL (first, 4.3);
            BOOST_CHECK (empty (cb));
        }

        BOOST_CHECK (!empty (f));
        BOOST_CHECK_EQUAL (first (f), '9');
        BOOST_CHECK (empty (drop (view (back, f))));
        BOOST_CHECK (empty (drop (front, f)));
        {
            auto chopped = range::chop (f);
            BOOST_CHECK_EQUAL (chopped.first(), '9');
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto chopped = range::chop (f);
            BOOST_CHECK_EQUAL (chopped.first(), '9');
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto first = chop_in_place (f);
            BOOST_CHECK_EQUAL (first, '9');
            BOOST_CHECK (empty (f));
        }

        BOOST_CHECK (!empty (i));
        {
            auto chopped = range::chop (std::move (i));
            BOOST_CHECK_EQUAL (chopped.first(), 19.5f);
            BOOST_CHECK (empty (chopped.rest()));
        }
    }

    // Test input iterator separately: it can only be moved.
    {
        std::stringstream stream (s);
        auto i = input_type (
            input_iterator_type (stream), input_iterator_type());
        BOOST_CHECK (!empty (i));
        BOOST_CHECK_EQUAL (first (std::move (i)), 19.5f);
    }
    {
        std::stringstream stream (s);
        auto i = input_type (
            input_iterator_type (stream), input_iterator_type());
        BOOST_CHECK (!empty (i));
        BOOST_CHECK (empty (drop (std::move (i))));
    }

    random_access_container.push_back (5);
    random_access_container.push_back (7);
    random_access_container.push_back (9);

    bidirectional_container.push_back (4.5);
    bidirectional_container.push_back (4.7);
    bidirectional_container.push_back (4.9);

    forward_container.push_front ('7');
    forward_container.push_front ('5');
    forward_container.push_front ('3');

    {
        random_access_type r (
            random_access_container.begin(), random_access_container.end());
        bidirectional_type b (
            bidirectional_container.begin(), bidirectional_container.end());
        const_bidirectional_type cb (
            bidirectional_container.begin(), bidirectional_container.end());
        forward_type f (forward_container.begin(), forward_container.end());

        // Extensive checks on random_access_type.
        BOOST_CHECK (!empty (r));
        BOOST_CHECK_EQUAL (size (r), 4u);
        BOOST_CHECK_EQUAL (first (r), 3);
        BOOST_CHECK_EQUAL (first (back, r), 9);
        BOOST_CHECK (!empty (drop (r)));
        BOOST_CHECK (!empty (drop (back, r)));
        BOOST_CHECK (empty (drop (4, r)));
        BOOST_CHECK (empty (drop (back, 4, r)));

        BOOST_CHECK_EQUAL (first (drop (r)), 5);
        BOOST_CHECK_EQUAL (first (drop (drop (r))), 7);
        BOOST_CHECK_EQUAL (first (drop (drop (drop (r)))), 9);
        BOOST_CHECK_EQUAL (first (drop (1, r)), 5);
        BOOST_CHECK_EQUAL (first (drop (rime::int_ <1>(), r)), 5);
        BOOST_CHECK_EQUAL (first (drop (2, r)), 7);
        BOOST_CHECK_EQUAL (first (drop (rime::int_ <2>(), r)), 7);
        BOOST_CHECK_EQUAL (first (drop (3, r)), 9);
        BOOST_CHECK_EQUAL (first (drop (rime::int_ <3>(), r)), 9);

        BOOST_CHECK_EQUAL (first (back, drop (r)), 9);
        BOOST_CHECK_EQUAL (first (back, drop (drop (r))), 9);
        BOOST_CHECK_EQUAL (first (back, drop (drop (drop (r)))), 9);
        BOOST_CHECK_EQUAL (first (back, drop (1, view (back, r))), 9);
        BOOST_CHECK_EQUAL (first (back, drop (rime::int_ <1>(), r)), 9);
        BOOST_CHECK_EQUAL (first (back, drop (2, r)), 9);
        BOOST_CHECK_EQUAL (first (back, drop (rime::int_ <2>(), r)), 9);
        BOOST_CHECK_EQUAL (first (back, drop (3, r)), 9);
        BOOST_CHECK_EQUAL (first (back, drop (rime::int_ <3>(), r)), 9);

        BOOST_CHECK_EQUAL (first (back, drop (back, r)), 7);
        BOOST_CHECK_EQUAL (first (back, drop (drop (back, r))), 7);
        BOOST_CHECK_EQUAL (first (back, drop (drop (drop (back, r)))), 7);
        BOOST_CHECK_EQUAL (first (back, drop (back, 1, r)), 7);
        BOOST_CHECK_EQUAL (first (back, drop (back, rime::int_ <1>(), r)), 7);
        BOOST_CHECK_EQUAL (first (back, drop (back, 2, r)), 5);
        BOOST_CHECK_EQUAL (first (back, drop (back, rime::int_ <2>(), r)), 5);
        BOOST_CHECK_EQUAL (first (back, drop (back, 3, r)), 3);
        BOOST_CHECK_EQUAL (first (back, drop (back, rime::int_ <3>(), r)), 3);

        // Mutate original container.
        first (r) = 30;
        BOOST_CHECK_EQUAL (random_access_container.front(), 30);
        first (back, drop (back, r)) = 70;
        BOOST_CHECK_EQUAL (random_access_container [2], 70);

        // 30 5 70 9
        {
            auto value = chop_in_place (back, r);
            BOOST_CHECK_EQUAL (value, 9);

            value = chop_in_place (front, r);
            BOOST_CHECK_EQUAL (value, 30);
            BOOST_CHECK_EQUAL (size (r), 2u);

            auto chopped = chop (back, r);
            BOOST_CHECK_EQUAL (chopped.first(), 70);
            BOOST_CHECK_EQUAL (size (chopped.rest()), 1u);

            // The first element may be a reference to an int, but this should
            // change the reference, not change the original value in the
            // container:
            chopped = chop (chopped.rest());
            BOOST_CHECK_EQUAL (random_access_container [2], 70);
            BOOST_CHECK_EQUAL (chopped.first(), 5);
            BOOST_CHECK (empty (chopped.rest()));
        }

        // Spot checks on (const_)bidirectional_type.
        BOOST_CHECK (!empty (b));
        BOOST_CHECK_EQUAL (first (b), 4.3);
        BOOST_CHECK_EQUAL (first (drop (b)), 4.5);
        BOOST_CHECK_EQUAL (first (drop (drop (b))), 4.7);
        BOOST_CHECK_EQUAL (first (drop (drop (drop (b)))), 4.9);

        BOOST_CHECK_EQUAL (first (back, cb), 4.9);
        BOOST_CHECK_EQUAL (first (back, drop (back, cb)), 4.7);
        BOOST_CHECK_EQUAL (first (back, drop (back, drop (back, cb))), 4.5);
        BOOST_CHECK_EQUAL (first (back,
            drop (back, drop (back, drop (back, cb)))), 4.3);

        BOOST_CHECK (empty (drop (drop (drop (drop (b))))));
        BOOST_CHECK (empty (drop (back, drop (drop (back, drop (cb))))));

        first (back, drop (back, b)) = 123.4;
        BOOST_CHECK_EQUAL (
            *boost::prior (boost::prior (bidirectional_container.end())),
            123.4);

        // Spot checks on forward_type.
        BOOST_CHECK (!empty (f));
        BOOST_CHECK_EQUAL (first (f), '3');
        BOOST_CHECK_EQUAL (first (drop (f)), '5');
        BOOST_CHECK_EQUAL (first (drop (drop (f))), '7');
        BOOST_CHECK_EQUAL (first (drop (drop (drop (f)))), '9');

        BOOST_CHECK (!empty (f));
        BOOST_CHECK (!empty (drop (f)));
        BOOST_CHECK (!empty (drop (drop (f))));
        BOOST_CHECK (!empty (drop (drop (drop (f)))));
        BOOST_CHECK (empty (drop (drop (drop (drop (f))))));

        first (drop (f)) = 'a';
        BOOST_CHECK_EQUAL (*boost::next (forward_container.begin()), 'a');

        // Check assignable without changing original container.
        random_access_container_type random_access_container_2 (
            random_access_container);
        r = random_access_type (
            random_access_container_2.begin(), random_access_container_2.end());
        BOOST_CHECK_EQUAL (random_access_container [0], 30);
        BOOST_CHECK_EQUAL (random_access_container [1], 5);
        BOOST_CHECK_EQUAL (random_access_container [2], 70);
        BOOST_CHECK_EQUAL (random_access_container [3], 9);

        // Change entries in random_access_container_2.
        first (r) = 90;
        first (drop (r)) = 91;
        BOOST_CHECK_EQUAL (random_access_container_2 [0], 90);
        BOOST_CHECK_EQUAL (random_access_container_2 [1], 91);
        // random_access_container should be unchanged.
        BOOST_CHECK_EQUAL (random_access_container [0], 30);
        BOOST_CHECK_EQUAL (random_access_container [1], 5);
    }

    // Check input_type separately.
    s = "19.5 18 17.25 16";
    {
        std::stringstream stream (s);
        auto i = input_type (
            input_iterator_type (stream), input_iterator_type());

        auto chopped = chop (std::move (i));
        BOOST_CHECK_EQUAL (chopped.first(), 19.5f);
        BOOST_CHECK (!empty (chopped.rest()));

        input_type i2 = chopped.move_rest();
        BOOST_CHECK (!empty (i2));

        chopped = chop (std::move (i2));
        BOOST_CHECK_EQUAL (chopped.first(), 18.f);
        BOOST_CHECK (!empty (chopped.rest()));

        i2 = chopped.move_rest();

        auto value = chop_in_place (i2);
        BOOST_CHECK_EQUAL (value, 17.25f);
        BOOST_CHECK (!empty (i2));

        value = chop_in_place (i2);
        BOOST_CHECK_EQUAL (value, 16);
        BOOST_CHECK (empty (i2));
    }
    // first and drop: only possible with temporaries.
    {
        std::stringstream stream (s);
        BOOST_CHECK_EQUAL (first (input_type (
            input_iterator_type (stream), input_iterator_type())), 19.5f);
    }
    {
        std::stringstream stream (s);
        BOOST_CHECK_EQUAL (first (drop (input_type (
            input_iterator_type (stream), input_iterator_type()))), 18.f);
    }
    {
        std::stringstream stream (s);
        BOOST_CHECK_EQUAL (first (drop (drop (input_type (
            input_iterator_type (stream), input_iterator_type())))), 17.25f);
    }
    {
        std::stringstream stream (s);
        BOOST_CHECK_EQUAL (first (drop (drop (drop (input_type (
            input_iterator_type (stream), input_iterator_type()))))), 16.f);
    }
}

BOOST_AUTO_TEST_CASE (test_make_iterator_range) {
    using range::empty;
    using range::size;
    using range::first;
    using range::at;

    std::vector <int> v;
    auto ir = range::make_iterator_range (v);
    static_assert (std::is_same <decltype (ir), random_access_type>::value, "");

    BOOST_CHECK (empty (ir));

    BOOST_CHECK (v.begin() == ir.begin());
    BOOST_CHECK (v.end() == ir.end());

    v.push_back (4);
    v.push_back (7);
    ir = range::make_iterator_range (v);

    BOOST_CHECK (v.begin() == ir.begin());
    BOOST_CHECK (v.end() == ir.end());

    BOOST_CHECK_EQUAL (size (ir), 2);
    BOOST_CHECK_EQUAL (first (ir), 4);
    BOOST_CHECK_EQUAL (at (1, ir), 7);

    v.push_back (27);

    auto ir2 = range::make_iterator_range (v.begin(), v.end());
    BOOST_CHECK (v.begin() == ir2.begin());
    BOOST_CHECK (v.end() == ir2.end());

    BOOST_CHECK_EQUAL (size (ir2), 3);
    BOOST_CHECK_EQUAL (first (ir2), 4);
    BOOST_CHECK_EQUAL (at (1, ir2), 7);
    BOOST_CHECK_EQUAL (at (2, ir2), 27);

#if !defined (BOOST_NO_INITIALIZER_LISTS)
    auto initializer_list = {6, 32};
    auto ilir = range::make_iterator_range (initializer_list);

    BOOST_CHECK_EQUAL (size (ilir), 2);
    BOOST_CHECK_EQUAL (first (ilir), 6);
    BOOST_CHECK_EQUAL (at (1, ilir), 32);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
