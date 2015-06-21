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

#define BOOST_TEST_MODULE test_container_test
#include "utility/test/boost_unit_test.hpp"

#include "container_test.hpp"

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
