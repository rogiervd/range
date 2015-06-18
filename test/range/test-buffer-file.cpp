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

#define BOOST_TEST_MODULE test_range_buffer_file
#include "utility/test/boost_unit_test.hpp"

#include "range/file_buffer.hpp"

using range::empty;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

BOOST_AUTO_TEST_SUITE(test_range_buffer_file)

BOOST_AUTO_TEST_CASE (file) {
    int argc = boost::unit_test::framework::master_test_suite().argc;
    char ** argv = boost::unit_test::framework::master_test_suite().argv;

    // Otherwise there are no files to test on.
    BOOST_REQUIRE_EQUAL (argc, 2);

    std::string file_name = argv [1];

    auto buffer = range::read_file <char> (file_name);

    BOOST_CHECK_EQUAL (first (buffer), 'S');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 'h');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 'o');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 'r');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 't');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), ' ');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 't');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 'e');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 'x');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), 't');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), '.');
    buffer = drop (buffer);
    BOOST_CHECK_EQUAL (first (buffer), '\n');
    buffer = drop (buffer);
    BOOST_CHECK (empty (buffer));
}

// \todo Test errors.

BOOST_AUTO_TEST_CASE (error) {
    BOOST_CHECK_THROW (range::read_file <char> ("non_existing_file_name.txt"),
        std::exception);
}

// \todo Also test on char16_t

BOOST_AUTO_TEST_SUITE_END()
