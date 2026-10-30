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

#define BOOST_TEST_MODULE test_container_test
#include "utility/test/boost_unit_test.hpp"

#include "container/test/container_test.hpp"

BOOST_AUTO_TEST_SUITE (test_container_test)

BOOST_AUTO_TEST_CASE (test_container_test_choices) {
    range::test::all_choices c;
    bool done = false;
    for (int i = 0; i != 6; ++ i) {
        for (int j = 0; j != 8-i; ++ j) {
            BOOST_CHECK (!done);
            c.start();
            int n = c (6);
            BOOST_CHECK_EQUAL (n, i);
            n = c (8-i);
            BOOST_CHECK_EQUAL (n, j);

            done = !c.next();
        }
    }
    BOOST_CHECK (done);
}

BOOST_AUTO_TEST_SUITE_END()
