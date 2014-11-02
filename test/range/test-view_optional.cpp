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

#define BOOST_TEST_MODULE test_range_view_optional
#include "utility/test/boost_unit_test.hpp"

#include "range/std/view_optional.hpp"

#include <boost/optional.hpp>

BOOST_AUTO_TEST_SUITE(test_range_view_optional)

BOOST_AUTO_TEST_CASE (test_range_view_optional_properties) {
    BOOST_MPL_ASSERT_NOT ((range::is_range <boost::optional <int>>));

    // Only works on lvalues (const or non-const).
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::view_optional (boost::optional <int>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::view_optional (boost::optional <int> &&)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::view_optional (boost::optional <int> &)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::view_optional (boost::optional <int> const &)>));

    typedef typename range::result_of <range::callable::view_optional (
        boost::optional <int> &)>::type view_type;

    BOOST_MPL_ASSERT ((range::is_range <view_type>));
}


BOOST_AUTO_TEST_CASE (test_range_view_optional_empty) {
    {
        boost::optional <int> empty;

        auto empty_view = range::view_optional (empty);

        BOOST_CHECK (range::empty (empty_view));
        BOOST_CHECK_EQUAL (range::size (empty_view), 0u);
    }
    {
        boost::optional <int const> empty;

        auto empty_view = range::view_optional (empty);

        // Also test direction "back" every once in a while (throughout).
        BOOST_CHECK (range::empty (range::back, empty_view));
        BOOST_CHECK_EQUAL (range::size (empty_view), 0u);
    }
    {
        boost::optional <int> const empty;

        auto empty_view = range::view_optional (empty);

        BOOST_CHECK (range::empty (empty_view));
        BOOST_CHECK_EQUAL (range::size (empty_view), 0u);
    }
    {
        boost::optional <int const> const empty;

        auto empty_view = range::view_optional (empty);

        BOOST_CHECK (range::empty (range::back, empty_view));
        BOOST_CHECK_EQUAL (range::size (empty_view), 0u);
    }
}

BOOST_AUTO_TEST_CASE (test_range_view_optional_not_empty) {
    {
        boost::optional <int> five (5);

        auto five_view = range::view_optional (five);

        BOOST_CHECK (!range::empty (five_view));
        BOOST_CHECK_EQUAL (range::size (five_view), 1u);

        BOOST_CHECK_EQUAL (range::first (five_view), 5);
        range::first (five_view) = -5;
        BOOST_CHECK_EQUAL (five.get(), -5);

        auto empty = range::drop (five_view);
        BOOST_CHECK (range::empty (empty));

        auto five_and_empty = range::chop (five_view);
        BOOST_CHECK_EQUAL (five_and_empty.first(), -5);
        BOOST_CHECK (range::empty (five_and_empty.rest()));
    }
    {
        boost::optional <int const> five (5);

        auto five_view = range::view_optional (five);

        BOOST_CHECK (!range::empty (range::back, five_view));
        BOOST_CHECK_EQUAL (range::size (five_view), 1u);

        BOOST_CHECK_EQUAL (range::first (range::back, five_view), 5);

        auto empty = range::drop (range::back, five_view);
        BOOST_CHECK (range::empty (empty));
    }
    {
        boost::optional <int> const five (5);

        auto five_view = range::view_optional (five);

        BOOST_CHECK (!range::empty (range::back, five_view));
        BOOST_CHECK_EQUAL (range::size (range::back, five_view), 1u);

        BOOST_CHECK_EQUAL (range::first (five_view), 5);

        auto empty = range::drop (five_view);
        BOOST_CHECK (range::empty (empty));
    }
    {
        boost::optional <int const> const five (5);

        auto five_view = range::view_optional (five);

        BOOST_CHECK (!range::empty (five_view));
        BOOST_CHECK_EQUAL (range::size (range::back, five_view), 1u);

        BOOST_CHECK_EQUAL (range::first (range::back, five_view), 5);

        auto empty = range::drop (five_view);
        BOOST_CHECK (range::empty (empty));
    }
}

BOOST_AUTO_TEST_CASE (test_range_view_optional_reference) {
    {
        int i = 5;
        boost::optional <int &> five (i);

        auto five_view = range::view_optional (five);

        BOOST_CHECK (!range::empty (five_view));
        BOOST_CHECK_EQUAL (range::size (five_view), 1u);

        BOOST_CHECK_EQUAL (range::first (five_view), 5);
        range::first (five_view) = -5;
        BOOST_CHECK_EQUAL (i, -5);

        auto empty = range::drop (five_view);
        BOOST_CHECK (range::empty (empty));
    }
}

BOOST_AUTO_TEST_SUITE_END()
