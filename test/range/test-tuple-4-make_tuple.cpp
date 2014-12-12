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

#define BOOST_TEST_MODULE test_range_tuple
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"

#include <type_traits>
#include <string>
#include <vector>
#include <list>

#include <boost/mpl/assert.hpp>

#include "range/std.hpp"
#include "range/take.hpp"

#include "utility/is_assignable.hpp"
#include "utility/test/tracked.hpp"

#include "check_equal.hpp"

using range::tuple;
using range::make_tuple;
using range::make_tuple_from;
using range::tie_from;
using range::swap;
using range::take;

using range::back;

using range::first;
using range::drop;
using range::at_c;
using range::second;
using range::third;

using utility::tracked_registry;
using utility::tracked;

BOOST_AUTO_TEST_SUITE(test_range_tuple)

BOOST_AUTO_TEST_CASE (test_range_make_tuple) {
    {
        auto t = make_tuple();
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple<>>));
    }
    // One element.
    {
        auto t = make_tuple (6);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int>>));
        BOOST_CHECK_EQUAL (first (t), 6);
    }
    {
        int i = 7;
        auto t = make_tuple (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int>>));
        BOOST_CHECK_EQUAL (first (t), 7);
    }
    {
        int const i = 7;
        auto t = make_tuple (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int>>));
        BOOST_CHECK_EQUAL (first (t), 7);
    }
    {
        int i = 9;
        auto t = make_tuple (std::move (i));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int>>));
        BOOST_CHECK_EQUAL (first (t), 9);
    }
    // More elements.
    {
        auto t = make_tuple (6, 'a');
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (t), range::tuple <int, char>>));
        BOOST_CHECK_EQUAL (first (t), 6);
        BOOST_CHECK_EQUAL (first (back, t), 'a');
    }
    {
        int i = 7;
        float f = 8.5f;
        double d = 12.75;
        auto t = make_tuple (i, f, d);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (t), range::tuple <int, float, double>>));
        BOOST_CHECK_EQUAL (first (t), 7);
        BOOST_CHECK_EQUAL (at_c <1> (t), 8.5f);
        BOOST_CHECK_EQUAL (at_c <2> (t), 12.75f);
    }
    {
        int i = 8;
        float f = 9.5f;
        double d = 13.75;
        auto t = make_tuple (
            std::move (i), std::move (f), std::move (d));
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (t), range::tuple <int, float, double>>));
        BOOST_CHECK_EQUAL (first (t), 8);
        BOOST_CHECK_EQUAL (at_c <1> (t), 9.5f);
        BOOST_CHECK_EQUAL (at_c <2> (t), 13.75f);
    }
}

BOOST_AUTO_TEST_CASE (test_range_make_tuple_from) {
    int i = 4;
    float f = 7.75;
    bool b = true;
    {
        std::tuple <int, float> st (i, f);

        auto copy = make_tuple_from (st);
        static_assert (std::is_same <
            decltype (copy), tuple <int, float>>::value, "");

        BOOST_CHECK_EQUAL (first (copy), 4);
        BOOST_CHECK_EQUAL (second (copy), 7.75);
    }
    {
        std::tuple <int &, float &&, bool const &> st (i, std::move (f), b);

        auto copy = make_tuple_from (st);
        static_assert (std::is_same <decltype (copy),
            tuple <int, float, bool>>::value, "");

        BOOST_CHECK_EQUAL (first (copy), 4);
        BOOST_CHECK_EQUAL (second (copy), 7.75);
        BOOST_CHECK_EQUAL (third (copy), true);
    }
    // From a list that is cut off at a point known at compile time.
    {
        std::list <int> l;

        // The list can turn out to be too short at run time.
        BOOST_CHECK_THROW (make_tuple_from (take (rime::size_t <3>(), l)),
            range::size_mismatch);

        l.push_back (3);
        BOOST_CHECK_THROW (make_tuple_from (take (rime::size_t <3>(), l)),
            range::size_mismatch);

        l.push_back (4);
        BOOST_CHECK_THROW (make_tuple_from (take (rime::size_t <3>(), l)),
            range::size_mismatch);

        l.push_back (6);
        {
            auto copy = make_tuple_from (take (rime::size_t <3>(), l));

            static_assert (std::is_same <decltype (copy),
                tuple <int, int, int>>::value, "");

            BOOST_CHECK_EQUAL (first (copy), 3);
            BOOST_CHECK_EQUAL (second (copy), 4);
            BOOST_CHECK_EQUAL (third (copy), 6);
        }

        l.push_back (9);
        {
            auto copy = make_tuple_from (take (rime::size_t <3>(), l));

            static_assert (std::is_same <decltype (copy),
                tuple <int, int, int>>::value, "");

            BOOST_CHECK_EQUAL (first (copy), 3);
            BOOST_CHECK_EQUAL (second (copy), 4);
            BOOST_CHECK_EQUAL (third (copy), 6);
        }
    }
    // This fails to compile.
    /*{
        std::vector <int> l;
        make_tuple_from (l);
    }*/
}

// Check tie.
BOOST_AUTO_TEST_CASE (test_range_tie) {
    {
        auto t = range::tie();
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple<>>));
    }
    // One element.
    // Does not work:
    /*{
        auto t = range::tie (6);
    }*/
    {
        int i = 7;
        auto t = range::tie (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int &>>));
        BOOST_CHECK_EQUAL (first (t), 7);

        // The tuple's elements must reference the original memory location.
        first (t) = 17;
        BOOST_CHECK_EQUAL (i, 17);
    }
    {
        int const i = 8;
        auto t = range::tie (i);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (t), range::tuple <int const &>>));
        BOOST_CHECK_EQUAL (first (t), 8);
        const_cast <int &> (i) = 18;
        BOOST_CHECK_EQUAL (first (t), 18);
    }
    // This does not work:
    /*{
        int i = 9;
        auto t = range::tie (std::move (i));
    }*/
    // More elements.
    {
        int i = 7;
        float f = 8.5f;
        double d = 12.75;
        auto t = range::tie (i, f, d);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), range::tuple <int &, float &, double &>>));
        BOOST_CHECK_EQUAL (first (t), 7);
        BOOST_CHECK_EQUAL (at_c <1> (t), 8.5f);
        BOOST_CHECK_EQUAL (at_c <2> (t), 12.75);

        // The tuple's elements must reference the original memory location.
        first (t) = 17;
        BOOST_CHECK_EQUAL (i, 17);
        at_c <1> (t) = 18.5f;
        BOOST_CHECK_EQUAL (f, 18.5f);
        at_c <2> (t) = 22.75;
        BOOST_CHECK_EQUAL (d, 22.75);
    }
    {
        int const i = 8;
        float const f = 9.5f;
        double const d = 13.75;
        auto t = range::tie (i, f, d);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t),
            range::tuple <int const &, float const &, double const &>>));
        BOOST_CHECK_EQUAL (first (t), 8);
        BOOST_CHECK_EQUAL (at_c <1> (t), 9.5f);
        BOOST_CHECK_EQUAL (at_c <2> (t), 13.75f);
    }

    // Check interaction with std::ignore and range::ignore.
    {
        range::tie (std::ignore) = make_tuple (5);
    }
    {
        range::tie (range::ignore) = make_tuple (5);
    }

    {
        int i = 7;
        range::tie (std::ignore, i) = make_tuple (5, 9);
        BOOST_CHECK_EQUAL (i, 9);
    }
    {
        int i = 7;
        range::tie (range::ignore, i) = make_tuple (5, 9);
        BOOST_CHECK_EQUAL (i, 9);
    }

    {
        char c = 'a';
        int i = 7;
        range::tie (c, std::ignore, i) = make_tuple ('b', 5l, 9);
        BOOST_CHECK_EQUAL (c, 'b');
        BOOST_CHECK_EQUAL (i, 9);
    }
    {
        char c = 'a';
        int i = 7;
        range::tie (c, range::ignore, i) = make_tuple ('b', 5l, 9);
        BOOST_CHECK_EQUAL (c, 'b');
        BOOST_CHECK_EQUAL (i, 9);
    }
}

BOOST_AUTO_TEST_CASE (test_range_tie_from) {
    int i = 4;
    float f = 7.75;
    bool b = true;
    {
        std::tuple <int, float> st (i, f);

        auto copy = tie_from (st);
        static_assert (std::is_same <
            decltype (copy), tuple <int &, float &>>::value, "");

        BOOST_CHECK_EQUAL (first (copy), 4);
        BOOST_CHECK_EQUAL (second (copy), 7.75);

        first (copy) = 67;
        BOOST_CHECK_EQUAL (first (st), 67);
        second (copy) = 42.25;
        BOOST_CHECK_EQUAL (second (st), 42.25);
    }
    {
        std::tuple <int &, float &&, bool const &> st (i, std::move (f), b);

        auto copy = tie_from (st);
        static_assert (std::is_same <decltype (copy),
            tuple <int &, float &, bool const &>>::value, "");

        BOOST_CHECK_EQUAL (first (copy), 4);
        BOOST_CHECK_EQUAL (second (copy), 7.75);
        BOOST_CHECK_EQUAL (third (copy), true);

        first (copy) = 14;
        BOOST_CHECK_EQUAL (i, 14);
        second (copy) = 17.75;
        BOOST_CHECK_EQUAL (f, 17.75);
    }
    // From a list that is cut off at a point known at compile time.
    {
        std::list <int> l;

        // The list can turn out to be too short at run time.
        BOOST_CHECK_THROW (tie_from (take (rime::size_t <3>(), l)),
            range::size_mismatch);

        l.push_back (3);
        BOOST_CHECK_THROW (tie_from (take (rime::size_t <3>(), l)),
            range::size_mismatch);

        l.push_back (4);
        BOOST_CHECK_THROW (tie_from (take (rime::size_t <3>(), l)),
            range::size_mismatch);

        l.push_back (6);
        {
            auto copy = tie_from (take (rime::size_t <3>(), l));

            static_assert (std::is_same <decltype (copy),
                tuple <int &, int &, int &>>::value, "");

            BOOST_CHECK_EQUAL (first (copy), 3);
            BOOST_CHECK_EQUAL (second (copy), 4);
            BOOST_CHECK_EQUAL (third (copy), 6);

            first (copy) += 10;
            BOOST_CHECK_EQUAL (l.front(), 13);
            second (copy) += 20;
            BOOST_CHECK_EQUAL (second (l), 24);
            third (copy) += 30;
            BOOST_CHECK_EQUAL (second (drop (l)), 36);
        }

        l.push_back (9);
        {
            auto copy = tie_from (take (rime::size_t <3>(), l));

            static_assert (std::is_same <decltype (copy),
                tuple <int &, int &, int &>>::value, "");

            BOOST_CHECK_EQUAL (first (copy), 13);
            BOOST_CHECK_EQUAL (second (copy), 24);
            BOOST_CHECK_EQUAL (third (copy), 36);

            first (copy) += 10;
            BOOST_CHECK_EQUAL (l.front(), 23);
            second (copy) += 20;
            BOOST_CHECK_EQUAL (second (l), 44);
            third (copy) += 30;
            BOOST_CHECK_EQUAL (second (drop (l)), 66);
        }
    }
    // This fails to compile.
    /*{
        std::vector <int> l;
        tie_from (l);
    }*/
}

// Check forward_as_tuple.
BOOST_AUTO_TEST_CASE (test_range_forward_as_tuple) {
    {
        auto t = range::forward_as_tuple();
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple<>>));
    }
    // One element.
    {
        auto t = range::forward_as_tuple (6);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int &&>>));
        // We can't check first (t) because the temporary has been moved.
        BOOST_CHECK_EQUAL (first (range::forward_as_tuple (66)), 66);
    }
    {
        int i = 7;
        auto t = range::forward_as_tuple (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int &>>));
        BOOST_CHECK_EQUAL (first (t), 7);
        first (t) = 17;
        BOOST_CHECK_EQUAL (i, 17);
    }
    {
        int const i = 8;
        auto t = range::forward_as_tuple (i);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (t), range::tuple <int const &>>));
        BOOST_CHECK_EQUAL (first (t), 8);
        BOOST_CHECK_EQUAL (&first (t), &i);
    }
    {
        int i = 9;
        auto t = range::forward_as_tuple (std::move (i));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), range::tuple <int &&>>));
        BOOST_CHECK_EQUAL (first (t), 9);
        i =19;
        BOOST_CHECK_EQUAL (first (t), 19);
    }
    // More elements.
    {
        auto t = range::forward_as_tuple (6, 'a');
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (t), range::tuple <int &&, char &&>>));
    }
    {
        int i = 7;
        float const f = 8.5f;
        double d = 12.75;
        auto t = range::forward_as_tuple (i, f, d);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t),
            range::tuple <int &, float const &, double &>>));
        BOOST_CHECK_EQUAL (first (t), 7);
        BOOST_CHECK_EQUAL (at_c <1> (t), 8.5f);
        BOOST_CHECK_EQUAL (at_c <2> (t), 12.75);

        first (t) = 17;
        BOOST_CHECK_EQUAL (i, 17);
        at_c <2> (t) = 22.75;
        BOOST_CHECK_EQUAL (d, 22.75);
    }
    {
        int i = 8;
        float f = 9.5f;
        double d = 13.75;
        auto t = range::forward_as_tuple (
            std::move (i), std::move (f), std::move (d));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t),
            range::tuple <int &&, float &&, double &&>>));
        BOOST_MPL_ASSERT ((std::is_same <decltype (first (t)), int &&>));
        BOOST_CHECK_EQUAL (first (t), 8);
        BOOST_CHECK_EQUAL (at_c <1> (t), 9.5f);
        BOOST_CHECK_EQUAL (at_c <2> (t), 13.75);

        i = 17;
        BOOST_CHECK_EQUAL (first (t), 17);
        f = 18.5f;
        BOOST_CHECK_EQUAL (at_c <1> (t), 18.5f);
        d = 22.75;
        BOOST_CHECK_EQUAL (at_c <2> (t), 22.75);
    }
}

/** Check swap member function and free function. */
BOOST_AUTO_TEST_CASE (test_range_tuple_swap) {
    {
        tuple <> t1;
        tuple <> t2;
        swap (t1, t2);
    }
    {
        tuple <> t1;
        tuple <> t2;
        t1.swap(t2);
    }

    {
        tuple <int> t1 (7);
        tuple <int> t2 (10);
        swap (t1, t2);
        BOOST_CHECK_EQUAL (first (t1), 10);
        BOOST_CHECK_EQUAL (first (t2), 7);
    }
    {
        tuple <int> t1 (7);
        tuple <int> t2 (10);
        t1.swap (t2);
        BOOST_CHECK_EQUAL (first (t1), 10);
        BOOST_CHECK_EQUAL (first (t2), 7);
    }

    {
        tuple <int, char, float> t1 (7, 'A', 86.5f);
        tuple <int, char, float> t2 (10, 'b', 98.125f);
        swap (t1, t2);

        BOOST_CHECK_EQUAL (first (t1), 10);
        BOOST_CHECK_EQUAL (at_c <1> (t1), 'b');
        BOOST_CHECK_EQUAL (at_c <2> (t1), 98.125f);

        BOOST_CHECK_EQUAL (first (t2), 7);
        BOOST_CHECK_EQUAL (at_c <1> (t2), 'A');
        BOOST_CHECK_EQUAL (at_c <2> (t2), 86.5f);
    }
    {
        tuple <int, char, float> t1 (7, 'A', 86.5f);
        tuple <int, char, float> t2 (10, 'b', 98.125f);
        t1.swap (t2);

        BOOST_CHECK_EQUAL (first (t1), 10);
        BOOST_CHECK_EQUAL (at_c <1> (t1), 'b');
        BOOST_CHECK_EQUAL (at_c <2> (t1), 98.125f);

        BOOST_CHECK_EQUAL (first (t2), 7);
        BOOST_CHECK_EQUAL (at_c <1> (t2), 'A');
        BOOST_CHECK_EQUAL (at_c <2> (t2), 86.5f);
    }

    {
        tracked_registry c;
        tuple <tracked <int>, char, tracked <float>> t1 (
            tracked <int> (c, 7), 'A', tracked <float> (c, 8.5f));
        tuple <tracked <int>, char, tracked <float>> t2 (
            tracked <int> (c, 10), 'b',
            tracked <float> (c, 12.5f));

        c.check_counts (4, 0, 4, 0, 0, 0, 0, 4);

        swap (t1, t2);
        c.check_counts (4, 0, 4, 0, 0, 2, 0, 4);

        BOOST_CHECK_EQUAL (first (t1).content(), 10);
        BOOST_CHECK_EQUAL (at_c <1> (t1), 'b');
        BOOST_CHECK_EQUAL (at_c <2> (t1).content(), 12.5f);

        BOOST_CHECK_EQUAL (first (t2).content(), 7);
        BOOST_CHECK_EQUAL (at_c <1> (t2), 'A');
        BOOST_CHECK_EQUAL (at_c <2> (t2).content(), 8.5f);
    }
}

BOOST_AUTO_TEST_SUITE_END()
