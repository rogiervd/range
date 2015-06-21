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

// \todo Move testing of gzipped files to another test file so valgrind isn't necessary?
// Valgrind catches things in zlib.

#include "range/file_buffer.hpp"

using range::buffer;

using range::empty;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

BOOST_AUTO_TEST_SUITE(test_range_buffer_file)

void checkShortText (buffer <char> b) {
    BOOST_CHECK_EQUAL (first (b), 'S');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 'h');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 'o');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 'r');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 't');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), ' ');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 't');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 'e');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 'x');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), 't');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), '.');
    b = drop (b);
    BOOST_CHECK_EQUAL (first (b), '\n');
    b = drop (b);
    BOOST_CHECK (empty (b));
}

BOOST_AUTO_TEST_CASE (file) {
    int argc = boost::unit_test::framework::master_test_suite().argc;
    char ** argv = boost::unit_test::framework::master_test_suite().argv;

    // Otherwise there are no files to test on.
    BOOST_REQUIRE_EQUAL (argc, 2);

    std::string file_name = argv [1];

    auto buffer = range::read_file <char> (file_name);
    checkShortText (std::move (buffer));

    buffer = range::read_gzip_file (file_name + ".gz");
    checkShortText (std::move (buffer));
}

// \todo Test errors.

BOOST_AUTO_TEST_CASE (error) {
    BOOST_CHECK_THROW (range::read_file <char> ("non_existing_file_name.txt"),
        std::exception);
}

// \todo Also implement gzip.

// \todo Also Test on long files!

// \todo Also test on char16_t

BOOST_AUTO_TEST_SUITE_END()
