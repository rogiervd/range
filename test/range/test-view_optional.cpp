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
