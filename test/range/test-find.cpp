/*
Copyright 2013, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_find
#include "utility/test/boost_unit_test.hpp"

#include "range/find.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "range/core.hpp"
#include "range/std.hpp"
#include <vector>
#include <list>
#include <tuple>
#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_find)

template <class Type> class less_than {
    Type right;
public:
    less_than (Type const & right) : right (right) {}

    template <class Left>
        auto operator() (Left const & left) const
    RETURNS (left < right);
};

using range::front;
using range::back;

using range::empty;
using range::size;
using range::first;
using range::drop;
using range::view;

using range::find;

struct end_marker {};

struct return_end_marker {
    template <class T>
    end_marker operator() (T const &) const { return end_marker(); }
};

struct return_false {
    template <class T>
    rime::false_type operator() (T const &) const { return rime::false_; }
};

BOOST_AUTO_TEST_CASE (test_range_find_homogeneous) {
    std::vector <int> v;
    less_than <int> less_than_4 (4);

    {
        auto result = find (v, less_than_4);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (view (v)), decltype (result)>));
        BOOST_CHECK (empty (result));
    }
    // With actors.
    {
        auto result = find (v, less_than_4, size);
        RIME_CHECK_EQUAL (result, std::size_t (0));
    }
    {
        // Return the size if non-empty, or end_marker() if empty.
        auto result = find (v, back, less_than_4, size, return_end_marker());
        BOOST_CHECK (result.contains <end_marker>());
    }

    v.push_back (5);
    {
        auto result = find (v, front, less_than_4);
        BOOST_CHECK (empty (result));
    }
    {
        auto result = find (v, back, less_than_4);
        BOOST_CHECK (empty (result));
    }
    {
        auto result =
            find (v, back, less_than_4, size, range::callable::size());
        RIME_CHECK_EQUAL (result, std::size_t (0));
    }
    // With actors.
    {
        // Return the size if non-empty, or end_marker() if empty.
        auto result = find (v, back, less_than_4, size, return_end_marker());
        BOOST_CHECK (result.contains <end_marker>());
    }

    v.push_back (3);
    {
        auto result = find (v, less_than_4);
        BOOST_CHECK (!empty (result));
        BOOST_CHECK_EQUAL (first (result), 3);
        BOOST_CHECK (empty (drop (result)));
    }
    {
        auto result = find (v, back, less_than_4);
        BOOST_CHECK (!empty (result));
        BOOST_CHECK_EQUAL (size (result), 2);
        BOOST_CHECK_EQUAL (first (result, front), 5);
        BOOST_CHECK_EQUAL (first (result, back), 3);
    }
    // With actors.
    {
        // Return the size if non-empty, or end_marker() if empty.
        auto result = find (v, less_than_4, size, return_end_marker());
        BOOST_CHECK_EQUAL (rime::get <std::size_t> (result), 1);
    }
    {
        auto result = find (v, back, less_than_4, size, return_end_marker());
        BOOST_CHECK_EQUAL (rime::get <std::size_t> (result), 2);
    }

    // With actors.
    {
        auto result = find (v, back, less_than_4, size);
        RIME_CHECK_EQUAL (result, std::size_t (2));
    }
    {
        auto result = find (v, back, less_than_4, range::callable::size());
        RIME_CHECK_EQUAL (result, std::size_t (2));
    }

    {
        auto result =
            find (v, back, less_than_4, size, range::callable::size());
        RIME_CHECK_EQUAL (result, std::size_t (2));
    }
    {
        auto result =
            find (v, back, less_than_4, range::callable::size(), size);
        RIME_CHECK_EQUAL (result, std::size_t (2));
    }
    // Known at compile time that none of the elements are going to be true.
    {
        auto result = find (
            v, return_false(), range::callable::size(), return_end_marker());
        BOOST_MPL_ASSERT ((std::is_same <decltype (result), end_marker>));
    }

    // Known at compile-time that the predicate is false.
    std::vector <rime::int_ <7>> v_seven (20, rime::int_ <7>());
    less_than <rime::int_ <4>> less_than_four ((rime::int_ <4>()));

    {
        auto result = find (v_seven, less_than_four);
        BOOST_CHECK (empty (result));
    }

    // Known at compile-time that the predicate is true.
    std::vector <rime::int_ <-1>> v_minus_one (20, rime::int_ <-1>());

    {
        auto result = find (v_minus_one, less_than_four);
        BOOST_CHECK_EQUAL (size (result), 20);
        BOOST_CHECK_EQUAL (&first (result), &v_minus_one.front());
    }
    {
        auto result = find (v_minus_one, back, less_than_four);
        BOOST_CHECK_EQUAL (size (result, back), 20);
        BOOST_CHECK_EQUAL (&first (result, back), &v_minus_one.back());
    }
}

BOOST_AUTO_TEST_CASE (test_range_find_heterogeneous) {
    less_than <int> less_than_4 (4);
    less_than <rime::int_ <4>> less_than_four ((rime::int_ <4>()));

    std::tuple<> empty_tuple;

    {
        auto result = find (empty_tuple, less_than_4);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (view (empty_tuple)), decltype (result)>));
        RIME_CHECK_EQUAL (empty (result), rime::true_);
    }
    {
        // Return the size if non-empty, or end_marker() if empty.
        auto result =
            find (empty_tuple, less_than_four, size, return_end_marker());
        BOOST_MPL_ASSERT ((std::is_same <decltype (result), end_marker>));
    }

    std::tuple <rime::int_<3>> tuple_three;
    {
        auto result = find (tuple_three, less_than_four);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (view (tuple_three)), decltype (result)>));
        RIME_CHECK_EQUAL (empty (result), rime::false_);
        RIME_CHECK_EQUAL (first (result), rime::int_<3>());
    }
    {
        // Return the size.
        auto result = find (tuple_three, back, less_than_four, size);
        RIME_CHECK_EQUAL (result, rime::size_t <1>());
    }
    {
        // Return the size if non-empty, or end_marker() if empty.
        auto result =
            find (tuple_three, less_than_four, size, return_end_marker());
        RIME_CHECK_EQUAL (result, rime::size_t <1>());
    }

    std::tuple <rime::int_<4>> tuple_four;
    {
        auto result = find (tuple_four, less_than_four);
        RIME_CHECK_EQUAL (empty (result), rime::true_);
    }
    {
        // Return the size.
        auto result = find (tuple_four, back, less_than_four, size);
        RIME_CHECK_EQUAL (result, rime::size_t <0>());
    }
    {
        // Return the size if non-empty, or end_marker() if empty.
        auto result =
            find (tuple_four, less_than_four, size, return_end_marker());
        BOOST_MPL_ASSERT ((std::is_same <decltype (result), end_marker>));
    }

    std::tuple <rime::int_<3>, rime::int_<5>> tuple_three_five;
    {
        auto result = find (tuple_three_five, less_than_four);
        RIME_CHECK_EQUAL (empty (result), rime::false_);
        RIME_CHECK_EQUAL (size (result), rime::size_t <2>());
        RIME_CHECK_EQUAL (first (result), rime::int_<3>());
        RIME_CHECK_EQUAL (first (result, back), rime::int_<5>());
    }
    {
        auto result = find (tuple_three_five, back, less_than_four);
        RIME_CHECK_EQUAL (empty (result, back), rime::false_);
        RIME_CHECK_EQUAL (size (result, back), rime::size_t <1>());
        RIME_CHECK_EQUAL (first (result), rime::int_<3>());
        RIME_CHECK_EQUAL (first (result, back), rime::int_<3>());
    }

    std::tuple <rime::int_<6>, rime::int_<3>, rime::int_<5>>
        tuple_six_three_five;
    {
        auto result = find (tuple_six_three_five, less_than_four);
        RIME_CHECK_EQUAL (empty (result), rime::false_);
        RIME_CHECK_EQUAL (size (result), rime::size_t <2>());
        RIME_CHECK_EQUAL (first (result), rime::int_<3>());
        RIME_CHECK_EQUAL (first (result, back), rime::int_<5>());
    }
    {
        // Return the size if non-empty, or end_marker() if empty.
        auto result = find (
            tuple_six_three_five, less_than_four, size, return_end_marker());
        RIME_CHECK_EQUAL (result, rime::size_t <2>());
    }
}

/**
Class that only allows its operator() to be called a specific number of times.
It returns false every time except for the last.
*/
class count_down {
    int count;
public:
    count_down (int count) : count (count) {}

    template <class Type> bool operator() (Type const &) {
        BOOST_CHECK (count > 0);
        -- count;
        return (count == 0);
    }

    int current() const { return count; }
};

struct is_floating_point {
    void operator() (bool) const; // Not defined.

    auto operator() (int) const RETURNS (rime::false_);
    auto operator() (short) const RETURNS (rime::false_);
    auto operator() (float) const RETURNS (rime::true_);
    auto operator() (double) const RETURNS (rime::true_);
};

/**
Test short-circuiting at run time and compile time.
*/
BOOST_AUTO_TEST_CASE (test_range_find_short_circuit) {
    count_down c (3);

    std::vector <int> v (4, 5);
    {
        auto result = find (v, c);
        BOOST_CHECK_EQUAL (size (result), 2);
        BOOST_CHECK_EQUAL (c.current(), 0);
    }

    // This does not compile, because operator() is called with a bool.
    /*std::tuple <int, bool> t1;
    find (t1, is_floating_point());*/

    // This should short-circuit at compile time: the third element returns
    // rime::true_, so the predicate is never called on the fourth element.
    std::tuple <int, short, double, bool> t2;
    {
        auto result = find (t2, is_floating_point());
        RIME_CHECK_EQUAL (size (result), rime::size_t <2>());
    }
}

/**
Check that the implementation uses recursion instead of iteration when the range
is homogeneous, by causing a stack overflow if not.
*/
BOOST_AUTO_TEST_CASE (test_range_find_stack_overflow) {
    std::vector <int> v (100000, 7);
    // Cause stack overflow if the implementation uses recursion.
    find (v, less_than <int> (4));
}

BOOST_AUTO_TEST_SUITE_END()

