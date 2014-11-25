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

#define BOOST_TEST_MODULE test_range_tuple_assign
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"

#include <string>
#include <tuple>
#include <vector>

#include <boost/mpl/assert.hpp>

#include "utility/is_assignable.hpp"
#include "utility/test/tracked.hpp"

#include "range/std.hpp"

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_assign)

using range::tuple;

using range::back;

using range::first;
using range::at_c;

using range::size_mismatch;

using utility::is_assignable;

using utility::tracked_registry;
using utility::tracked;

struct source {};

struct assignable {
    assignable & operator = (source const &) { return *this; }
};

struct unassignable {
    unassignable & operator = (unassignable const &) = delete;
};

BOOST_AUTO_TEST_CASE (tuple_assign_test_test) {
    // Check whether the tests make sense.
    static_assert (
        is_assignable <assignable, source>::value, "");
    static_assert (
        !is_assignable <unassignable, source>::value, "");
}

/* Default-assignion from elements. */

BOOST_AUTO_TEST_CASE (tuple_assign_zero) {
    static_assert (is_assignable <tuple<>, tuple<>>::value, "");

    // Wrong size.
    static_assert (!is_assignable <tuple<>, tuple <source>>::value, "");
    static_assert (!is_assignable <tuple<>, tuple <source, source>>::value, "");

    {
        tuple<> t1;
        tuple<> t2;
        t1 = t2;
    }
}

BOOST_AUTO_TEST_CASE (tuple_assign_from_range_zero) {
    // std::tuple.
    static_assert (is_assignable <tuple<>, std::tuple<>>::value, "");

    // Wrong size.
    static_assert (!is_assignable <tuple<>, std::tuple <source>>::value, "");
    static_assert (!is_assignable <
        tuple<>, std::tuple <source, source>>::value, "");

    // std::vector
    static_assert (is_assignable <tuple<>, std::vector <int>>::value, "");

    {
        std::tuple<> t1;
        tuple<> t2;
        t2 = t1;
    }
    {
        std::vector <int> v;
        tuple<> t;
        t = v;

        v.push_back (7);
        BOOST_CHECK_THROW (t = v, size_mismatch);
    }
}


BOOST_AUTO_TEST_CASE (tuple_assign_one) {
    static_assert (is_assignable <
        tuple <assignable>, tuple <source>>::value, "");
    static_assert (!is_assignable <
        tuple <unassignable>, tuple <source>>::value, "");

    // Wrong size.
    static_assert (!is_assignable <
        tuple <assignable>, tuple<>>::value, "");
    static_assert (!is_assignable <
        tuple <assignable>, tuple <source, source>>::value, "");

    {
        tuple <int> t (7);

        tuple <int> ti (21);
        ti = t;
        BOOST_CHECK_EQUAL (first (ti), 7);

        tuple <long> tl (31l);
        tl = t;
        BOOST_CHECK_EQUAL (first (tl), 7l);
    }
    {
        int i1 = 80;
        int i2 = 32;
        tuple <int &> t1 (i1);
        tuple <int &> t2 (i2);

        BOOST_CHECK_EQUAL (first (t1), i1);
        BOOST_CHECK_EQUAL (&first (t1), &i1);

        BOOST_CHECK_EQUAL (first (t2), i2);
        BOOST_CHECK_EQUAL (&first (t2), &i2);

        // Set i2 through the reference to it in i1.
        t2 = t1;
        BOOST_CHECK_EQUAL (i2, 80);
        BOOST_CHECK_EQUAL (first (t2), 80);

        // But i2 must be set.
        i1 = 73;
        BOOST_CHECK_EQUAL (first (t2), 80);
        BOOST_CHECK_EQUAL (&first (t2), &i2);
    }
}

BOOST_AUTO_TEST_CASE (tuple_assign_from_range_one) {
    static_assert (is_assignable <
        tuple <assignable>, std::tuple <source>>::value, "");
    static_assert (!is_assignable <
        tuple <unassignable>, std::tuple <source>>::value, "");

    // Wrong size.
    static_assert (!is_assignable <
        tuple <assignable>, std::tuple<>>::value, "");
    static_assert (!is_assignable <
        tuple <assignable>, std::tuple <source, source>>::value, "");

    // std::vector
    static_assert (is_assignable <
        tuple <assignable>, std::vector <source>>::value, "");
    static_assert (!is_assignable <
        tuple <unassignable>, std::vector <source>>::value, "");

    {
        std::tuple <int> t (7);

        tuple <int> ti (21);
        ti = t;
        BOOST_CHECK_EQUAL (first (ti), 7);

        tuple <long> tl (31l);
        tl = t;
        BOOST_CHECK_EQUAL (first (tl), 7l);
    }
    {
        int i1 = 80;
        int i2 = 32;
        std::tuple <int &> t1 (i1);
        tuple <int &> t2 (i2);

        BOOST_CHECK_EQUAL (first (t1), i1);
        BOOST_CHECK_EQUAL (&first (t1), &i1);

        BOOST_CHECK_EQUAL (first (t2), i2);
        BOOST_CHECK_EQUAL (&first (t2), &i2);

        // Set i2 through the reference to it in i1.
        t2 = t1;
        BOOST_CHECK_EQUAL (i2, 80);
        BOOST_CHECK_EQUAL (first (t2), 80);

        // But i2 must be set.
        i1 = 73;
        BOOST_CHECK_EQUAL (first (t2), 80);
        BOOST_CHECK_EQUAL (&first (t2), &i2);
    }
    {
        std::vector <int> v;
        tuple <int> t (6);
        BOOST_CHECK_EQUAL (first (t), 6);

        // Throw if the size is mismatched at run time.
        BOOST_CHECK_THROW (t = v, size_mismatch);

        v.push_back (7);
        t = v;
        BOOST_CHECK_EQUAL (first (t), 7);

        v.push_back (7);
        BOOST_CHECK_THROW (t = v, size_mismatch);
    }
}

BOOST_AUTO_TEST_CASE (tuple_assign_more) {
    static_assert (is_assignable <
        tuple <assignable, assignable>, tuple <source, source>>::value, "");
    static_assert (!is_assignable <
        tuple <assignable, unassignable>, tuple <source, source>>::value, "");
    static_assert (!is_assignable <
        tuple <unassignable, assignable>, tuple <source, source>>::value, "");
    static_assert (!is_assignable <
        tuple <unassignable, unassignable>, tuple <source, source>>::value, "");

    // Wrong size.
    static_assert (!is_assignable <
        tuple <assignable, assignable>, tuple<>>::value, "");
    static_assert (!is_assignable <
        tuple <assignable, assignable>, tuple <source>>::value, "");
    static_assert (!is_assignable <
        tuple <assignable, assignable>,
        tuple <source, source, source>>::value, "");

    {
        tracked_registry c;
        tracked <int> ci (c, 45);
        float f = 4.5f;
        tracked <double> cd (c, 6.7);
        c.check_counts (2, 0, 0, 0, 0, 0, 0, 0);

        tuple <tracked <int> &, float &, tracked <double> &>
            tied (ci, f, cd);
        tuple <tracked <int>, float, tracked <double>>
            saved (ci, f, cd);

        c.check_counts (2, 2, 0, 0, 0, 0, 0, 0);

        // Values in "saved".
        first (saved).content() = 56;
        BOOST_CHECK_EQUAL (first (saved).content(), 56);
        BOOST_CHECK_EQUAL (ci.content(), 45);

        at_c <1> (saved) = 7.6f;
        BOOST_CHECK_EQUAL (at_c <1> (saved), 7.6f);
        BOOST_CHECK_EQUAL (f, 4.5f);

        at_c <2> (saved).content() = 10.6;
        BOOST_CHECK_EQUAL (at_c <2> (saved).content(), 10.6);
        BOOST_CHECK_EQUAL (cd.content(), 6.7);

        c.check_counts (2, 2, 0, 0, 0, 0, 0, 0);

        // Assign values to the original objects.
        tied = saved;
        BOOST_CHECK_EQUAL (ci.content(), 56);
        BOOST_CHECK_EQUAL (f, 7.6f);
        BOOST_CHECK_EQUAL (cd.content(), 10.6);

        c.check_counts (2, 2, 0, 2, 0, 0, 0, 0);

        // Move.
        // First change the values in "saved" so that it's noticeable.
        at_c <0> (saved).content() = 78;
        at_c <1> (saved) = 17.45f;
        at_c <2> (saved).content() = 18.45;
        tied = std::move (saved);

        BOOST_CHECK_EQUAL (ci.content(), 78);
        BOOST_CHECK_EQUAL (f, 17.45f);
        BOOST_CHECK_EQUAL (cd.content(), 18.45);

        // If moveability is exploited, the result is:
        c.check_counts (2, 2, 0, 2, 2, 0, 0, 0);
        // Otherwise it would be:
        // c.check_counts (2, 2, 0, 4, 0, 0, 0, 0);
    }
    // Check whether the implementation is better than Visual C++'s original
    // implementation of std::pair in handling rvalue references.
    // http://connect.microsoft.com/VisualStudio/feedback/details/696109
    {
        tracked_registry r;
        {
            tracked <int> a1 (r, 7);
            tracked <double> a2 (r, 9);

            tracked <int> b1 (r, 8.25);
            tracked <double> b2 (r, 10.5);

            typedef tuple <tracked <int> &, tracked <double> &> pair;
            pair p (a1, a2);
            r.check_counts (4, 0, 0, 0, 0, 0, 0, 0);
            // This should copy-assign b1 into a1 and b2 into a2, not move it.
            p = pair (b1, b2);
            r.check_counts (4, 0, 0, 2, 0, 0, 0, 0);
        }
    }
}

BOOST_AUTO_TEST_CASE (tuple_assign_from_range_more) {
    // std::tuple.
    static_assert (is_assignable <tuple <assignable, assignable>,
        std::tuple <source, source>>::value, "");
    static_assert (!is_assignable <tuple <assignable, unassignable>,
        std::tuple <source, source>>::value, "");
    static_assert (!is_assignable <tuple <unassignable, assignable>,
        std::tuple <source, source>>::value, "");
    static_assert (!is_assignable <tuple <unassignable, unassignable>,
        std::tuple <source, source>>::value, "");

    // std::pair.
    static_assert (is_assignable <tuple <assignable, assignable>,
        std::pair <source, source>>::value, "");
    static_assert (!is_assignable <tuple <assignable, unassignable>,
        std::pair <source, source>>::value, "");
    static_assert (!is_assignable <tuple <unassignable, assignable>,
        std::pair <source, source>>::value, "");
    static_assert (!is_assignable <tuple <unassignable, unassignable>,
        std::pair <source, source>>::value, "");

    // Wrong size.
    static_assert (!is_assignable <
        tuple <assignable, assignable>, std::tuple<>>::value, "");
    static_assert (!is_assignable <
        tuple <assignable, assignable>, std::tuple <source>>::value, "");
    static_assert (!is_assignable <
        tuple <assignable, assignable>,
        std::tuple <source, source, source>>::value, "");

    static_assert (is_assignable <tuple <assignable, assignable>,
        std::vector <source>>::value, "");
    static_assert (!is_assignable <tuple <assignable, unassignable>,
        std::vector <source>>::value, "");
    static_assert (!is_assignable <tuple <unassignable, assignable>,
        std::vector <source>>::value, "");
    static_assert (!is_assignable <tuple <unassignable, unassignable>,
        std::vector <source>>::value, "");

    {
        int i = 89;
        float f = 5.5;
        tuple <int &, float &> tied (i, f);

        std::tuple <int, float> original (101, 77.5);
        tied = original;
        BOOST_CHECK_EQUAL (i, 101);
        BOOST_CHECK_EQUAL (f, 77.5);

        std::pair <int, float> original2 (720, -3.5);
        tied = original2;
        BOOST_CHECK_EQUAL (i, 720);
        BOOST_CHECK_EQUAL (f, -3.5);
    }
    {
        std::vector <float> v;
        BOOST_CHECK_THROW ((tuple <float, double> (v)), size_mismatch);

        v.push_back (7.5);
        BOOST_CHECK_THROW ((tuple <float, double> (v)), size_mismatch);

        v.push_back (9.5);

        tuple <float, double> t (v);
        BOOST_CHECK_EQUAL (first (t), 7.5);
        BOOST_CHECK_EQUAL (first (back, t), 9.5);

        // Change the original vector.
        // This is not in general recommended: there is the risk of dangling
        // references if the vector changes.
        {
            tuple <float &, float &> tied (v);
            first (tied) = 3456;
            BOOST_CHECK_EQUAL (first (v), 3456);
        }

        v.push_back (12.25);
        BOOST_CHECK_THROW ((tuple <float, double> (v)), size_mismatch);
    }
}

// Type that contains a tuple, and default-generated assignment operators.
struct product_type {
    tuple <int, float> t;

    product_type (int i, float f) : t (i, f) {}

    product_type & operator = (product_type const &) = default;
    product_type & operator = (product_type &&) = default;
};

BOOST_AUTO_TEST_CASE (contained_assign) {
    product_type p1 (5, 6.7);

    product_type p2 (12, 13.5);
    product_type p3 (17, 18.5);

    p3 = p1;
    p3 = std::move (p2);
}

BOOST_AUTO_TEST_SUITE_END()
