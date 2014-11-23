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

#define BOOST_TEST_MODULE test_range_fold_state_types
#include "utility/test/boost_unit_test.hpp"

#include "range/fold.hpp"

#include <vector>
#include <tuple>

#include <boost/mpl/assert.hpp>

#include "range/std.hpp"
#include "range/tuple.hpp"

using range::fold;
using range::at_c;

struct add_return_const {
    int const operator() (int i, int j) const { return i + j; }
};

BOOST_AUTO_TEST_SUITE(test_range_fold_state_types)

BOOST_AUTO_TEST_CASE (test_fold_const_state) {
    {
        std::vector <int> v;
        v.push_back (1);
        v.push_back (2);
        v.push_back (3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (add_return_const(), 0, v)), int const>));
        BOOST_CHECK_EQUAL (fold (add_return_const(), 0, v), 6);
    }
    {
        std::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (add_return_const(), 0, v)), int const>));
        BOOST_CHECK_EQUAL (fold (add_return_const(), 0, v), 6);
    }
    {
        range::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (add_return_const(), 0, v)), int const>));
        BOOST_CHECK_EQUAL (fold (add_return_const(), 0, v), 6);
    }
}

struct unassignable {
    int value;

    explicit unassignable (int value) : value (value) {}

    unassignable (unassignable const &) = default;

    unassignable & operator = (unassignable const &) = delete;
    unassignable & operator = (unassignable &&) = delete;

    unassignable operator + (unassignable const & that) const
    { return unassignable (this->value + that.value); }

    unassignable operator + (int that) const
    { return unassignable (this->value + that); }
};

struct add_unassignable {
    unassignable operator() (unassignable const & i, int j) const
    { return unassignable (i.value + j); }
};

BOOST_AUTO_TEST_CASE (test_fold_unassignable_state) {
    {
        std::vector <int> v;
        v.push_back (1);
        v.push_back (2);
        v.push_back (3);
        BOOST_CHECK (fold (add_unassignable(), unassignable (0), v).value == 6);
    }
    {
        std::tuple <int, short, int> v (1,2,3);
        BOOST_CHECK (fold (add_unassignable(), unassignable (0), v).value == 6);
    }
    {
        range::tuple <int, short, int> v (1,2,3);
        BOOST_CHECK (fold (add_unassignable(), unassignable (0), v).value == 6);
    }
}

struct return_right {
    template <class Left, class Right>
        Right && operator() (Left && left, Right && right) const
    { return static_cast <Right &&> (right); }
};

BOOST_AUTO_TEST_CASE (test_fold_lvalue_state) {
    int i = 7;
    {
        std::vector <int> v;
        v.push_back (1);
        v.push_back (2);
        v.push_back (3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (return_right(), i, v)), int &>));
        BOOST_CHECK_EQUAL (&fold (return_right(), i, v), &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL (i, 7);
        BOOST_CHECK_EQUAL (at_c <0> (v), 1);
        BOOST_CHECK_EQUAL (at_c <1> (v), 2);
        BOOST_CHECK_EQUAL (at_c <2> (v), 3);
    }
    {
        std::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (return_right(), i, v)), int &>));
        BOOST_CHECK_EQUAL (&fold (return_right(), i, v), &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL (i, 7);
        BOOST_CHECK_EQUAL (at_c <0> (v), 1);
        BOOST_CHECK_EQUAL (at_c <1> (v), 2);
        BOOST_CHECK_EQUAL (at_c <2> (v), 3);
    }
    {
        range::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (return_right(), i, v)), int &>));
        BOOST_CHECK_EQUAL (&fold (return_right(), i, v), &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL (i, 7);
        BOOST_CHECK_EQUAL (at_c <0> (v), 1);
        BOOST_CHECK_EQUAL (at_c <1> (v), 2);
        BOOST_CHECK_EQUAL (at_c <2> (v), 3);
    }
}

BOOST_AUTO_TEST_CASE (test_fold_rvalue_state) {
    int i = 7;
    {
        std::vector <int> v;
        v.push_back (1);
        v.push_back (2);
        v.push_back (3);
        // If 5.5 were an int &&, this should return an rvalue reference.
        // But it doesn't, because rvalue references are (sensibly) stripped
        // from the state argument.
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (return_right(), 5.5, std::move (v))),
            rime::variant <double, int &&>>));
        // BOOST_CHECK_EQUAL (
        //     &fold (return_right(), std::move (i), std::move (v)),
        //     &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL (i, 7);
        BOOST_CHECK_EQUAL (at_c <0> (v), 1);
        BOOST_CHECK_EQUAL (at_c <1> (v), 2);
        BOOST_CHECK_EQUAL (at_c <2> (v), 3);
    }
    {
        std::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (return_right(), i, std::move (v))), int &&>));
        auto && result = fold (return_right(), i, std::move (v));
        BOOST_CHECK_EQUAL (&result, &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL (i, 7);
        BOOST_CHECK_EQUAL (at_c <0> (v), 1);
        BOOST_CHECK_EQUAL (at_c <1> (v), 2);
        BOOST_CHECK_EQUAL (at_c <2> (v), 3);
    }
    {
        range::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (return_right(), i, std::move (v))), int &&>));
        auto && result = fold (return_right(), i, std::move (v));
        BOOST_CHECK_EQUAL (&result, &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL (i, 7);
        BOOST_CHECK_EQUAL (at_c <0> (v), 1);
        BOOST_CHECK_EQUAL (at_c <1> (v), 2);
        BOOST_CHECK_EQUAL (at_c <2> (v), 3);
    }
}

struct const_or_not {
    const_or_not() {}

    bool operator() (int, int) const { return true; }
    bool operator() (int, int) { return false; }
};

BOOST_AUTO_TEST_CASE (test_fold_function_lvalue_rvalue) {
    const_or_not f;
    const_or_not const f_const;

    BOOST_CHECK_EQUAL (f (1, 2), false);
    BOOST_CHECK_EQUAL (std::move (f) (1, 2), false);
    BOOST_CHECK_EQUAL (f_const (1, 2), true);

    {
        std::vector <int> v;
        v.push_back (1);

        BOOST_CHECK_EQUAL (fold (f, true, v), false);
        BOOST_CHECK_EQUAL (fold (std::move (f), true, v), false);
        BOOST_CHECK_EQUAL (fold (f_const, true, v), true);
    }
    {
        std::tuple <int> v (1);

        BOOST_CHECK_EQUAL (fold (f, true, v), false);
        BOOST_CHECK_EQUAL (fold (std::move (f), true, v), false);
        BOOST_CHECK_EQUAL (fold (f_const, true, v), true);
    }
    {
        range::tuple <int> v (1);

        BOOST_CHECK_EQUAL (fold (f, true, v), false);
        BOOST_CHECK_EQUAL (fold (std::move (f), true, v), false);
        BOOST_CHECK_EQUAL (fold (f_const, true, v), true);
    }
}

BOOST_AUTO_TEST_SUITE_END()
