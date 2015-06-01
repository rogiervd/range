/*
Copyright 2012, 2013, 2014 Rogier van Dalen.

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

/*
\file Test "fold" on large ranges, to test for stack overflows.
*/

#define BOOST_TEST_MODULE test_range_fold_large
#include "utility/test/boost_unit_test.hpp"

#include "range/fold.hpp"

#include <tuple>
#include <vector>
#include <type_traits>

#include "rime/check/check_equal.hpp"
#include "utility/returns.hpp"

#include "range/std.hpp"

BOOST_AUTO_TEST_SUITE(test_range_fold_large)

struct plus {
    template <class Left, class Right>
    auto operator() (Left const & a, Right const & b) const RETURNS (a + b);
};

/**
Add two values but change types: char becomes float, float becomes double,
double becomes int, and then we stay with ints.
*/
struct settling_plus {
    float operator() (char l, int r) const { return l + r; }
    double operator() (float l, int r) const { return l + r; }
    int operator() (double l, int r) const { return int (l + r); }
    int operator() (int l, int r) const { return l + r; }
};

struct non_assignable {
    int i;
    non_assignable (int i) : i (i) {}
    non_assignable (non_assignable const &) = default;

    non_assignable & operator= (non_assignable const &) = delete;
};

non_assignable operator + (
    non_assignable const & left, non_assignable const & right)
{ return non_assignable (left.i + right.i); }

/**
One definition of fold uses recursion.
That runs the risk of causing stack overflows.
This can be prevented in various ways.
One is by evaluating the fold not in a linear tree, but in a binary tree.
The other is by re-using the memory for the state, and use a while-loop.
Whichever version is used, the below is a homogeneous fold, and the
implementation should deal with it.
*/
BOOST_AUTO_TEST_CASE (test_fold_homogeneous_stack_overflow) {
    using range::fold;

    std::size_t const element_num = 300000;

    std::vector <int> v (element_num, 1);
    BOOST_CHECK_EQUAL (fold (plus(), 0, v), element_num);

    std::list <int> l (element_num, 1);
    BOOST_CHECK_EQUAL (fold (plus(), 0, l), element_num);

    {
        auto result = fold (settling_plus(), char (0), v);
        BOOST_CHECK_EQUAL (rime::get <int> (result), element_num);
    }
}

/**
Non-assignable classes should be used in a homogeneous fold.
*/
BOOST_AUTO_TEST_CASE (test_fold_non_assignable) {
    using range::fold;
    std::size_t const element_num = 300000;

    std::vector <non_assignable> v (element_num, non_assignable (1));

    BOOST_CHECK_EQUAL (
        fold (plus(), non_assignable (0), v).i, element_num);
}

BOOST_AUTO_TEST_SUITE_END()
