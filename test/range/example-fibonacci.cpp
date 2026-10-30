// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE example_fibonacci
#include "boost/test/unit_test.hpp"

#include "range/core.hpp"
#include "range/count.hpp"
#include "range/for_each_macro.hpp"

#include <iostream>
#include <ostream>
#include <vector>

class fibonacci
{
    int previous_;
    int current_;

    fibonacci(int previous, int current)
    : previous_(previous), current_(current)
    {}

public:
    fibonacci() : previous_(0), current_(1) {}

    int first(direction::front) const { return current_; }

    fibonacci drop_one(direction::front) const
    {
        return fibonacci(current_, previous_ + current_);
    }
};

struct fibonacci_tag {};

namespace range {
template <> struct tag_of_qualified<fibonacci>
{
    typedef fibonacci_tag type;
};
}  // namespace range

using range::chop_in_place;
using range::count;
using range::drop;
using range::first;

BOOST_AUTO_TEST_SUITE(example_fibonacci)

BOOST_AUTO_TEST_CASE(straightforward_fibonacci)
{
    int previous = 0;
    int current = 1;
    RANGE_FOR_EACH(i, count(20))
    {
        std::cout << i << ' ' << current << std::endl;
        int new_current = previous + current;
        previous = current;
        current = new_current;
    }
}

BOOST_AUTO_TEST_CASE(test_fibonacci)
{
    fibonacci f;
    BOOST_CHECK_EQUAL(chop_in_place(f), 1);
    BOOST_CHECK_EQUAL(chop_in_place(f), 1);
    BOOST_CHECK_EQUAL(chop_in_place(f), 2);
    BOOST_CHECK_EQUAL(chop_in_place(f), 3);
    BOOST_CHECK_EQUAL(chop_in_place(f), 5);
    BOOST_CHECK_EQUAL(chop_in_place(f), 8);
    BOOST_CHECK_EQUAL(chop_in_place(f), 13);
}

BOOST_AUTO_TEST_CASE(print_fibonacci)
{
    fibonacci f;

    RANGE_FOR_EACH(i, count(20))
    {
        std::cout << i << ' ' << first(f) << std::endl;
        f = drop(f);
    }
}

BOOST_AUTO_TEST_SUITE_END()
