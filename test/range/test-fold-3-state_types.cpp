/*
Copyright 2014, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_fold_state_types
#include "utility/test/boost_unit_test.hpp"

#include "range/fold.hpp"

#include <vector>
#include <tuple>

#include <boost/mpl/assert.hpp>

#include "range/std.hpp"

using range::fold;
using range::at_c;

struct int_holder {
    int i;
    int_holder() : i(0) {}
    int_holder (int i) : i (i) {}
};

struct add_return_const {
    // "const" is only retained in return types for class types, so return
    // "int_holder const" and not just "int const".
    int_holder const operator() (int_holder i, int j) const
    { return int_holder (i.i + j); }
};

BOOST_AUTO_TEST_SUITE(test_range_fold_state_types)

BOOST_AUTO_TEST_CASE (test_fold_const_state) {
    {
        std::vector <int> v;
        v.push_back (1);
        v.push_back (2);
        v.push_back (3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (int_holder(), v, add_return_const())),
            int_holder const>));
        BOOST_CHECK_EQUAL (fold (int_holder(), v, add_return_const()).i, 6);
    }
    {
        std::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (int_holder(), v, add_return_const())),
            int_holder const>));
        BOOST_CHECK_EQUAL (fold (int_holder(), v, add_return_const()).i, 6);
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
        BOOST_CHECK (fold (unassignable (0), v, add_unassignable()).value == 6);
    }
    {
        std::tuple <int, short, int> v (1,2,3);
        BOOST_CHECK (fold (unassignable (0), v, add_unassignable()).value == 6);
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
            decltype (fold (i, v, return_right())), int &>));
        BOOST_CHECK_EQUAL (&fold (i, v, return_right()), &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL (i, 7);
        BOOST_CHECK_EQUAL (at_c <0> (v), 1);
        BOOST_CHECK_EQUAL (at_c <1> (v), 2);
        BOOST_CHECK_EQUAL (at_c <2> (v), 3);
    }
    {
        std::tuple <int, short, int> v (1,2,3);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (fold (i, v, return_right())), int &>));
        BOOST_CHECK_EQUAL (&fold (i, v, return_right()), &at_c <2> (v));

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
            decltype (fold (5.5, std::move (v), return_right())),
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
            decltype (fold (i, std::move (v), return_right())), int &&>));
        auto && result = fold (i, std::move (v), return_right());
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

        BOOST_CHECK_EQUAL (fold (true, v, f), false);
        BOOST_CHECK_EQUAL (fold (true, v, std::move (f)), false);
        BOOST_CHECK_EQUAL (fold (true, v, f_const), true);
    }
    {
        std::tuple <int> v (1);

        BOOST_CHECK_EQUAL (fold (true, v, f), false);
        BOOST_CHECK_EQUAL (fold (true, v, std::move (f)), false);
        BOOST_CHECK_EQUAL (fold (true, v, f_const), true);
    }
}

BOOST_AUTO_TEST_SUITE_END()
