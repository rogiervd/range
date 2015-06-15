/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE example_fibonacci
#include "utility/test/boost_unit_test.hpp"

#include "range/core.hpp"
#include "range/count.hpp"
#include "range/for_each_macro.hpp"

#include <vector>
#include <ostream>
#include <iostream>

class fibonacci {
    int previous_;
    int current_;

    fibonacci (int previous, int current)
    : previous_ (previous), current_ (current) {}

public:
    fibonacci() : previous_ (0), current_ (1) {}

    int first (direction::front) const { return current_; }

    fibonacci drop_one (direction::front) const
    { return fibonacci (current_, previous_ + current_); }
};

struct fibonacci_tag {};

namespace range {
    template<> struct tag_of_qualified <fibonacci>
    { typedef fibonacci_tag type; };
}

using range::first;
using range::drop;
using range::chop_in_place;
using range::count;

BOOST_AUTO_TEST_SUITE(example_fibonacci)

BOOST_AUTO_TEST_CASE (straightforward_fibonacci) {
    int previous = 0;
    int current = 1;
    RANGE_FOR_EACH (i, count (20)) {
        std::cout << i << ' ' << current << std::endl;
        int new_current = previous + current;
        previous = current;
        current = new_current;
    }
}

BOOST_AUTO_TEST_CASE (test_fibonacci) {
    fibonacci f;
    BOOST_CHECK_EQUAL (chop_in_place (f), 1);
    BOOST_CHECK_EQUAL (chop_in_place (f), 1);
    BOOST_CHECK_EQUAL (chop_in_place (f), 2);
    BOOST_CHECK_EQUAL (chop_in_place (f), 3);
    BOOST_CHECK_EQUAL (chop_in_place (f), 5);
    BOOST_CHECK_EQUAL (chop_in_place (f), 8);
    BOOST_CHECK_EQUAL (chop_in_place (f), 13);
}

BOOST_AUTO_TEST_CASE (print_fibonacci) {
    fibonacci f;

    RANGE_FOR_EACH (i, count (20)) {
        std::cout << i << ' ' << first (f) << std::endl;
        f = drop (f);
    }
}

BOOST_AUTO_TEST_SUITE_END()
