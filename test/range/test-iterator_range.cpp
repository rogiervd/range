/*
Copyright 2013-2015 Rogier van Dalen.

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
        random_access_type, direction::front>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        random_access_type const, direction::back>));

    BOOST_MPL_ASSERT ((range::is_homogeneous <bidirectional_type &>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        forward_type, direction::front>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        input_type, direction::front>));
    BOOST_MPL_ASSERT ((range::is_homogeneous <
        const_bidirectional_type, direction::back>));

    // has::
    {
        // view
        BOOST_MPL_ASSERT ((
            range::has <range::callable::view (random_access_type)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::view (random_access_type, int)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::view (
            random_access_type const &, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::view (
            random_access_type, direction::back &)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::view (forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::view (input_type)>));

        // empty
        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (random_access_type)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::empty (random_access_type, int)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            random_access_type const &, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            random_access_type, direction::back)>));

        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (bidirectional_type)>));
        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (const_bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            bidirectional_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            bidirectional_type, direction::back)>));

        BOOST_MPL_ASSERT ((
            range::has <range::callable::empty (forward_type &)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (forward_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (forward_type, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (input_type)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (input_type const &, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <
            range::callable::empty (input_type &&, direction::back)>));

        // size
        BOOST_MPL_ASSERT ((
            range::has <range::callable::size (random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::size (
            random_access_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::size (
            random_access_type, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (const_bidirectional_type)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            bidirectional_type, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            bidirectional_type const &, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (forward_type const)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            forward_type, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            forward_type, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::size (input_type const)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            input_type &, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            input_type, direction::back)>));

        // first
        BOOST_MPL_ASSERT ((
            range::has <range::callable::first (random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            random_access_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            random_access_type, direction::back)>));

        BOOST_MPL_ASSERT ((
            range::has <range::callable::first (bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            bidirectional_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            bidirectional_type, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::first (forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            forward_type, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            forward_type, direction::back)>));

        // Only for rvalue ranges.
        static_assert (
            std::is_constructible <input_type, input_type &&>::value, "");
        BOOST_MPL_ASSERT ((range::has <range::callable::first (input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            input_type &&, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            input_type, direction::back)>));

        // Not for lvalue ranges.
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            input_type const &, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::first (
            input_type &, direction::back)>));

        // drop without increment
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            bidirectional_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            bidirectional_type, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            forward_type, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, direction::back)>));

        // Only for rvalue input.
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            input_type &&, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type &, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type &, direction::back)>));

        // drop with increment of 1
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, rime::int_ <1>)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, rime::int_ <1>, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, rime::int_ <1>, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            bidirectional_type, rime::int_ <1>)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            bidirectional_type, rime::int_ <1>, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            bidirectional_type, rime::int_ <1>, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            forward_type, rime::int_ <1>)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            forward_type, rime::int_ <1>, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, rime::int_ <1>, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            input_type, rime::int_ <1>)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            input_type, rime::int_ <1>, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, rime::int_ <1>, direction::back)>));

        // drop with increment of 2
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, rime::int_ <2>)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, rime::int_ <2>, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, rime::int_ <2>, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            bidirectional_type, rime::int_ <2>)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            bidirectional_type, rime::int_ <2>, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            bidirectional_type, rime::int_ <2>, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, rime::int_ <2>)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, rime::int_ <2>, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, rime::int_ <2>, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, rime::int_ <2>)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, rime::int_ <2>, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, rime::int_ <2>, direction::back)>));

        // drop with increment of int
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, int)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, int, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            random_access_type, int, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            bidirectional_type, int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            bidirectional_type, int, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            bidirectional_type, int, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, int, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            forward_type, int, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, int)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, int, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::drop (
            input_type, int, direction::back)>));

        // chop.
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            random_access_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            random_access_type, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            bidirectional_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            bidirectional_type, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            bidirectional_type, direction::back)>));

        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            forward_type, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            forward_type, direction::back)>));

        // Only for rvalue input.
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            input_type &&, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            input_type, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            input_type &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            input_type &, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (
            input_type &, direction::back)>));

        // chop_in_place: only for lvalue references.
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            random_access_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            random_access_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            random_access_type &, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            random_access_type &, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            bidirectional_type &&)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            bidirectional_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            bidirectional_type &, direction::front)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            bidirectional_type &, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            forward_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            forward_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            forward_type &, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            forward_type &, direction::back)>));

        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            input_type)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            input_type &)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop_in_place (
            input_type &, direction::front)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop_in_place (
            input_type &, direction::back)>));
    }

    // Spot check result_of::
    {
        // view
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (random_access_type)>::type,
            random_access_type>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (
            bidirectional_type &, direction::back)>::type,
            bidirectional_type &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (
                forward_type const &, direction::front &)>::type,
            forward_type const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::view (
                input_type, direction::front &)>::type,
            input_type>));

        // forward_view.
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (
                random_access_type)>::type, random_access_type &&>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (
                bidirectional_type &, direction::back)>::type,
            bidirectional_type &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (
                forward_type const &, direction::front &)>::type,
            forward_type const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename
            range::result_of <range::callable::forward_view (
                input_type, direction::front &)>::type, input_type &&>));

        // empty
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (random_access_type)>::type, bool>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (bidirectional_type, direction::back)>::type,
            bool>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (forward_type, direction::front)>::type,
            bool>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::empty (input_type, direction::front)>::type,
            bool>));

        // size returns an unsigned integer.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::size (random_access_type)>::type, std::size_t>));

        // first
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (random_access_type)>::type, int &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (bidirectional_type, direction::back)>::type,
            double &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (const_bidirectional_type, direction::back
            )>::type, double const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (forward_type, direction::front)>::type,
            char &>));
        // For input ranges, "first" should return the value_type.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::first (input_type, direction::front)>::type,
            float>));

        // drop
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::drop (random_access_type)>::type,
            random_access_type>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::drop (bidirectional_type &, direction::back
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
            range::callable::chop (bidirectional_type &, direction::back
            )>::type,
            range::chopped <double &, bidirectional_type>>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop (const_bidirectional_type &, direction::back
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
                bidirectional_type &, direction::back)>::type,
            double &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (
                const_bidirectional_type &, direction::back
            )>::type, double const &>));
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (
                forward_type &, direction::front)>::type,
            char &>));
        // For input ranges, "chop_in_place" should return the value_type.
        BOOST_MPL_ASSERT ((std::is_same <typename range::result_of <
            range::callable::chop_in_place (
                input_type &, direction::front)>::type,
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
        BOOST_CHECK_EQUAL (first (r, back), 3);
        BOOST_CHECK (empty (drop (r)));
        BOOST_CHECK (empty (drop (r, front)));
        BOOST_CHECK (empty (drop (r, back)));
        BOOST_CHECK (empty (drop (r, 1, front)));
        BOOST_CHECK (empty (drop (r, 1, back)));
        {
            auto chopped = range::chop (r);
            BOOST_CHECK_EQUAL (chopped.first(), 3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto chopped = range::chop (r, back);
            BOOST_CHECK_EQUAL (chopped.first(), 3);
            BOOST_CHECK (empty (chopped.rest()));
        }
        {
            auto first = chop_in_place (r, back);
            BOOST_CHECK_EQUAL (first, 3);
            BOOST_CHECK (empty (r));
        }

        BOOST_CHECK (!empty (b));
        BOOST_CHECK_EQUAL (first (view (b, front)), 4.3);
        BOOST_CHECK_EQUAL (first (b, back), 4.3);
        BOOST_CHECK (empty (drop (b)));
        BOOST_CHECK (empty (drop (b, front)));
        BOOST_CHECK (empty (drop (b, back)));
        {
            auto chopped = range::chop (b, back);
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
        BOOST_CHECK_EQUAL (first (cb, back), 4.3);
        BOOST_CHECK (empty (drop (cb)));
        BOOST_CHECK (empty (drop (cb, front)));
        BOOST_CHECK (empty (drop (cb, back)));
        {
            auto chopped = range::chop (cb, back);
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
        BOOST_CHECK (empty (drop (view (f, back))));
        BOOST_CHECK (empty (drop (f, front)));
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
        BOOST_CHECK_EQUAL (first (r, back), 9);
        BOOST_CHECK (!empty (drop (r)));
        BOOST_CHECK (!empty (drop (r, back)));
        BOOST_CHECK (empty (drop (r, 4)));
        BOOST_CHECK (empty (drop (r, 4, back)));

        BOOST_CHECK_EQUAL (first (drop (r)), 5);
        BOOST_CHECK_EQUAL (first (drop (drop (r))), 7);
        BOOST_CHECK_EQUAL (first (drop (drop (drop (r)))), 9);
        BOOST_CHECK_EQUAL (first (drop (r, 1)), 5);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <1>())), 5);
        BOOST_CHECK_EQUAL (first (drop (r, 2)), 7);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <2>())), 7);
        BOOST_CHECK_EQUAL (first (drop (r, 3)), 9);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <3>())), 9);

        BOOST_CHECK_EQUAL (first (drop (r), back), 9);
        BOOST_CHECK_EQUAL (first (drop (drop (r)), back), 9);
        BOOST_CHECK_EQUAL (first (drop (drop (drop (r))), back), 9);
        BOOST_CHECK_EQUAL (first (drop (view (r, back), 1), back), 9);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <1>()), back), 9);
        BOOST_CHECK_EQUAL (first (drop (r, 2), back), 9);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <2>()), back), 9);
        BOOST_CHECK_EQUAL (first (drop (r, 3), back), 9);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <3>()), back), 9);

        BOOST_CHECK_EQUAL (first (drop (r, back), back), 7);
        BOOST_CHECK_EQUAL (first (drop (drop (r, back)), back), 7);
        BOOST_CHECK_EQUAL (first (drop (drop (drop (r, back))), back), 7);
        BOOST_CHECK_EQUAL (first (drop (r, 1, back), back), 7);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <1>(), back), back), 7);
        BOOST_CHECK_EQUAL (first (drop (r, 2, back), back), 5);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <2>(), back), back), 5);
        BOOST_CHECK_EQUAL (first (drop (r, 3, back), back), 3);
        BOOST_CHECK_EQUAL (first (drop (r, rime::int_ <3>(), back), back), 3);

        // Mutate original container.
        first (r) = 30;
        BOOST_CHECK_EQUAL (random_access_container.front(), 30);
        first (drop (r, back), back) = 70;
        BOOST_CHECK_EQUAL (random_access_container [2], 70);

        // 30 5 70 9
        {
            auto value = chop_in_place (r, back);
            BOOST_CHECK_EQUAL (value, 9);

            value = chop_in_place (r, front);
            BOOST_CHECK_EQUAL (value, 30);
            BOOST_CHECK_EQUAL (size (r), 2u);

            auto chopped = chop (r, back);
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

        BOOST_CHECK_EQUAL (first (cb, back), 4.9);
        BOOST_CHECK_EQUAL (first (drop (cb, back), back), 4.7);
        BOOST_CHECK_EQUAL (first (drop (drop (cb, back), back), back), 4.5);
        BOOST_CHECK_EQUAL (first (
            drop (drop (drop (cb, back), back), back), back), 4.3);

        BOOST_CHECK (empty (drop (drop (drop (drop (b))))));
        BOOST_CHECK (empty (drop (drop (drop (drop (cb), back)), back)));

        first (drop (b, back), back) = 123.4;
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

    // Silly arguments.
    static_assert (!range::has <
        range::callable::make_iterator_range (int)>::value, "");
    static_assert (!range::has <
        range::callable::make_iterator_range (int, int)>::value, "");
    static_assert (!range::has <
        range::callable::make_iterator_range (double, int)>::value, "");
    static_assert (!range::has <
        range::callable::make_iterator_range (double, int, int)>::value, "");

    std::vector <int> v;

    // Pass in a container.
    static_assert (range::has <
        range::callable::make_iterator_range (decltype (v))>::value, "");
    static_assert (std::is_same <range::result_of <
        range::callable::make_iterator_range (decltype ((v)))>::type,
        random_access_type>::value, "");

    // Pass in two iterators.
    static_assert (range::has <
        range::callable::make_iterator_range (
            decltype (v.begin()), decltype (v.end()))>::value, "");
    static_assert (std::is_same <range::result_of <
        range::callable::make_iterator_range (
            decltype (v.begin()), decltype (v.end()))>::type,
        random_access_type>::value, "");

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
    BOOST_CHECK_EQUAL (at (ir, 1), 7);

    v.push_back (27);

    auto ir2 = range::make_iterator_range (v.begin(), v.end());
    BOOST_CHECK (v.begin() == ir2.begin());
    BOOST_CHECK (v.end() == ir2.end());

    BOOST_CHECK_EQUAL (size (ir2), 3);
    BOOST_CHECK_EQUAL (first (ir2), 4);
    BOOST_CHECK_EQUAL (at (ir2, 1), 7);
    BOOST_CHECK_EQUAL (at (ir2, 2), 27);

#if !defined (BOOST_NO_INITIALIZER_LISTS)
    auto initializer_list = {6, 32};
    auto ilir = range::make_iterator_range (initializer_list);

    BOOST_CHECK_EQUAL (size (ilir), 2);
    BOOST_CHECK_EQUAL (first (ilir), 6);
    BOOST_CHECK_EQUAL (at (ilir, 1), 32);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
