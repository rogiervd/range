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

/* \file
Example to be used in the documentation.
*/

#define BOOST_TEST_MODULE example_std_adaptor
#include "utility/test/boost_unit_test.hpp"

#include "range/std.hpp"

#include <iostream>
#include <ostream>
#include <vector>
#include <string>
#include <tuple>

#include <boost/optional.hpp>

#include "range/fold.hpp"

BOOST_AUTO_TEST_SUITE(example_std_adaptor)

BOOST_AUTO_TEST_CASE (example_std_adaptor) {
    using namespace range;

    // Initialise vector and tuple.
#if defined(BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX)
    int elements[] {5, 7, 9};
    std::vector <int> integers (&elements [0], &elements [3]);
    std::tuple <int, std::string, float> various (6, "Hello", 7.5);
#else
    std::vector <int> integers {5, 7, 9};
    std::tuple <int, std::string, float> various {6, "Hello", 7.5};
#endif

    // Print the first and second element.
    std::cout << first (integers) << ' ' << at_c <1> (integers) << std::endl;
    std::cout << first (various) << ' ' << at_c <1> (various) << std::endl;

    // Is the range empty?
    assert (!empty (integers));
    assert (!empty (various));

    // Find the size.
    assert (size (integers) == 3);
    assert (size (various) == 3);

    // Retrieve the first element.
    assert (first (integers) == 5);
    assert (first (various, back) == 7.5);
    assert (first (various) == 6);

    // Remove one element from the back and then retrieve the next element.
    assert (first (drop (integers)) == 7);
    assert (first (drop (integers, back), back) == 7);
    assert (first (drop (various, back), back) == "Hello");

    // Retrieve the element at position 2.
    assert (at_c <2> (integers) == 9);
    assert (at (integers, 2) == 9);
    assert (at_c <2> (various) == 7.5);
}

BOOST_AUTO_TEST_CASE (example_view_optional) {
    using namespace range;

    boost::optional <int> none;
    boost::optional <int> three (3);

    assert (empty (view_optional (none)));

    auto three_view = view_optional (three);
    assert (!empty (three_view));
    assert (size (three_view) == 1);
    assert (first (three_view) == 3);
    assert (empty (drop (three_view)));
    (void) three_view;
}

BOOST_AUTO_TEST_SUITE_END()
