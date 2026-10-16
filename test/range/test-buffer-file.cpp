// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_buffer_file
#include <boost/test/unit_test.hpp>

#include "range/file_buffer.hpp"

#include <fstream>

#include <boost/filesystem/operations.hpp>

#include "range/count.hpp"
#include "range/for_each_macro.hpp"

using range::buffer;

using range::chop;
using range::chop_in_place;
using range::drop;
using range::empty;
using range::first;

BOOST_AUTO_TEST_SUITE(test_range_buffer_file)

void checkShortText(buffer<char> b)
{
    BOOST_CHECK_EQUAL(first(b), 'S');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 'h');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 'o');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 'r');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 't');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), ' ');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 't');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 'e');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 'x');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), 't');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), '.');
    b = drop(b);
    BOOST_CHECK_EQUAL(first(b), '\n');
    b = drop(b);
    BOOST_CHECK(empty(b));
}

BOOST_AUTO_TEST_CASE(file)
{
    int argc = boost::unit_test::framework::master_test_suite().argc;
    char ** argv = boost::unit_test::framework::master_test_suite().argv;

    // Otherwise there are no files to test on.
    BOOST_REQUIRE_EQUAL(argc, 2);

    std::string file_name = argv[1];

    auto buffer = range::read_file(file_name);
    checkShortText(std::move(buffer));

    buffer = range::read_gzip_file(file_name + ".gz");
    checkShortText(std::move(buffer));
}

BOOST_AUTO_TEST_CASE(error)
{
    BOOST_CHECK_THROW(
        range::read_file("non_existing_file_name.txt"), range::file_open_error);
    BOOST_CHECK_THROW(
        range::read_gzip_file("non_existing_file_name.txt.gz"),
        range::file_open_error);

    // I do not know how to check for read errors.
}

BOOST_AUTO_TEST_CASE(long_file)
{
    auto temporary_file_name = boost::filesystem::temp_directory_path()
        / boost::filesystem::unique_path();

    // Make 100k file with cycling chars.
    {
        std::ofstream f(temporary_file_name.string(), std::ios_base::binary);
        RANGE_FOR_EACH(i, range::count(100000)) { f << char(i); }
        f.flush();
    }

    // Read the file.
    {
        auto b = range::read_file(temporary_file_name.string());
        RANGE_FOR_EACH(i, range::count(100000))
        {
            char c = chop_in_place(b);
            BOOST_CHECK_EQUAL(c, char(i));
        }
        BOOST_CHECK(empty(b));
    }

    boost::filesystem::remove(temporary_file_name);
}

BOOST_AUTO_TEST_SUITE_END()
