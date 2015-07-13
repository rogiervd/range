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

#include <fstream>

#include <boost/filesystem/operations.hpp>

#include "range/for_each_macro.hpp"
#include "range/count.hpp"

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

    auto buffer = range::read_file (file_name);
    checkShortText (std::move (buffer));

    buffer = range::read_gzip_file (file_name + ".gz");
    checkShortText (std::move (buffer));
}

BOOST_AUTO_TEST_CASE (error) {
    BOOST_CHECK_THROW (range::read_file ("non_existing_file_name.txt"),
        range::file_open_error);
    BOOST_CHECK_THROW (range::read_gzip_file ("non_existing_file_name.txt.gz"),
        range::file_open_error);

    // I do not know how to check for read errors.
}

BOOST_AUTO_TEST_CASE (long_file) {
    auto temporary_file_name = boost::filesystem::temp_directory_path() /
        boost::filesystem::unique_path();

    // Make 100k file with cycling chars.
    {
        std::ofstream f (temporary_file_name.native(), std::ios_base::binary);
        RANGE_FOR_EACH (i, range::count (100000)) {
            f << char (i);
        }
        f.flush();
    }

    // Read the file.
    {
        auto b = range::read_file (temporary_file_name.native());
        RANGE_FOR_EACH (i, range::count (100000)) {
            char c = chop_in_place (b);
            BOOST_CHECK_EQUAL (c, char (i));
        }
        BOOST_CHECK (empty (b));
    }

    boost::filesystem::remove (temporary_file_name);
}

BOOST_AUTO_TEST_SUITE_END()
