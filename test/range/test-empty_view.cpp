/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_empty_view
#include "utility/test/boost_unit_test.hpp"

#include "range/empty_view.hpp"

BOOST_AUTO_TEST_SUITE(test_range_empty_view)

BOOST_AUTO_TEST_CASE (test_range_empty_view) {
    range::empty_view v;

    typedef decltype (range::empty (v)) is_empty;
    static_assert (is_empty::value, "");

    typedef decltype (range::empty (range::back, v)) is_empty_back;
    static_assert (is_empty_back::value, "");

    typedef decltype (range::size (v)) size;
    static_assert (size::value == 0u, "");

    typedef decltype (range::size (range::back, v)) size_back;
    static_assert (size_back::value == 0u, "");
}

BOOST_AUTO_TEST_SUITE_END()
