/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_tuple_basic
#include "utility/test/boost_unit_test.hpp"

#include "range/tuple.hpp"

#include <tuple>
#include "range/std/tuple.hpp"

#include "rime/check/check_equal.hpp"

#include "tuple-basic_tests.hpp"

BOOST_AUTO_TEST_SUITE(test_range_tuple_basic)

BOOST_AUTO_TEST_CASE (tuple_basic) {
    check_empty (tuple<>());
    check_empty (view (tuple<>()));
    check_empty (view_once (tuple<>()));

    check_one_element (tuple <int> (5), 5);
    check_one_element (view (tuple <int> (5)), 5);
    check_one_element (view_once (tuple <int> (5)), 5);

    check_two_elements (tuple <int, char> (5, 'A'), 5, 'A');
    check_two_elements (view (tuple <int, char> (5, 'A')), 5, 'A');
    check_two_elements (view_once (tuple <int, char> (5, 'A')), 5, 'A');

    // Checks on three element is in the other file.
    // Otherwise, compilers start using more than 1GB of memory.
}

BOOST_AUTO_TEST_SUITE_END()
