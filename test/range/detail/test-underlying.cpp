// SPDX-FileCopyrightText: Copyright 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_detail_underlying
#include <boost/test/unit_test.hpp>

#include "range/helper/underlying.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "utility/test/tracked.hpp"

BOOST_AUTO_TEST_SUITE(test_range_detail_underlying)

template <class Underlying> class with_underlying
{
public:
    typedef Underlying underlying_type;

    template <class... Arguments> with_underlying(Arguments &&... arguments)
    : underlying_(std::forward<Arguments>(arguments)...)
    {}

private:
    template <class Wrapper>
    friend class range::helper::callable::get_underlying;
    // Should be accessible from the tests.
public:
    underlying_type underlying_;
};

using range::helper::get_underlying;

using utility::tracked;
using utility::tracked_registry;

BOOST_AUTO_TEST_CASE(test_range_detail_underlying)
{
    // Reference.
    {
        typedef with_underlying<int> type;
        type u(5);
        BOOST_CHECK_EQUAL(get_underlying<type &>(u), 5);
        BOOST_MPL_ASSERT(
            (std::is_same<decltype(get_underlying<type &>(u)), int &>) );
        BOOST_CHECK_EQUAL(&get_underlying<type &>(u), &u.underlying_);
    }
    // Const reference.
    {
        typedef with_underlying<int> const type;
        type u(6);
        BOOST_CHECK_EQUAL(get_underlying<type &>(u), 6);
        BOOST_MPL_ASSERT(
            (std::is_same<decltype(get_underlying<type &>(u)), int const &>) );
        BOOST_CHECK_EQUAL(&get_underlying<type &>(u), &u.underlying_);
    }
    // Rvalue.
    {
        typedef with_underlying<int> type;
        type u(7);
        BOOST_CHECK_EQUAL(get_underlying<type>(u), 7);
        BOOST_MPL_ASSERT(
            (std::is_same<decltype(get_underlying<type>(u)), int &&>) );
        // BOOST_CHECK_EQUAL (&get_underlying <type> (u), &u.underlying_);
        u.underlying_ = 8;
        BOOST_CHECK_EQUAL(get_underlying<type>(u), 8);
    }

    // Test with tracked.
    {
        tracked_registry c;
        typedef with_underlying<tracked<int>> type;
        type u(c, 9);
        auto before = c.counts();
        tracked<int> object(get_underlying<type>(u));
        BOOST_CHECK_EQUAL(c.since(before), utility::move_count(1));
    }

    // Reference to const.
    {
        typedef with_underlying<int const> type;
        type u(6);
        BOOST_CHECK_EQUAL(get_underlying<type &>(u), 6);
        BOOST_MPL_ASSERT(
            (std::is_same<decltype(get_underlying<type &>(u)), int const &>) );
        BOOST_CHECK_EQUAL(&get_underlying<type &>(u), &u.underlying_);
    }
    // Const reference to const.
    {
        typedef with_underlying<int const> const type;
        type u(6);
        BOOST_CHECK_EQUAL(get_underlying<type &>(u), 6);
        BOOST_MPL_ASSERT(
            (std::is_same<decltype(get_underlying<type &>(u)), int const &>) );
        BOOST_CHECK_EQUAL(&get_underlying<type &>(u), &u.underlying_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
