/*
Copyright 2013 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_detail_underlying
#include "utility/test/boost_unit_test.hpp"

#include "range/detail/underlying.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "utility/counted.hpp"

BOOST_AUTO_TEST_SUITE(test_range_detail_underlying)

template <class Underlying> class has_underlying {
public:
    typedef Underlying underlying_type;

    template <class ... Arguments> has_underlying (Arguments && ... arguments)
    : underlying_ (std::forward <Arguments> (arguments)...) {}

private:
    friend class range::detail::callable::get_underlying;
    // Should be accessible from the tests.
public:
    underlying_type underlying_;
};

using range::detail::get_underlying;

using utility::counted;
using utility::object_counter;

BOOST_AUTO_TEST_CASE (test_range_detail_underlying) {
    // Reference.
    {
        has_underlying <int> u (5);
        BOOST_CHECK_EQUAL (get_underlying (u), 5);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (get_underlying (u)), int &>));
        BOOST_CHECK_EQUAL (&get_underlying (u), &u.underlying_);
    }
    // Const reference.
    {
        has_underlying <int> const u (6);
        BOOST_CHECK_EQUAL (get_underlying (u), 6);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (get_underlying (u)), int const &>));
        BOOST_CHECK_EQUAL (&get_underlying (u), &u.underlying_);
    }
    // Rvalue.
    {
        has_underlying <int> u (7);
        BOOST_CHECK_EQUAL (get_underlying (std::move (u)), 7);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (get_underlying (std::move (u))), int &&>));
        //BOOST_CHECK_EQUAL (&get_underlying (std::move (u)), &u.underlying_);
        u.underlying_ = 8;
        BOOST_CHECK_EQUAL (get_underlying (std::move (u)), 8);
    }

    // Test with counted_object.
    {
        object_counter c;
        has_underlying <counted <int>> u (c, 9);
        BOOST_CHECK_EQUAL (c.value_construct_count(), 1);
        counted <int> object (get_underlying (std::move (u)));
        BOOST_CHECK_EQUAL (c.move_count(), 1);
        c.expect (1, 0, 1, 0, 0, 0, 0, 0);
    }

    // Reference to const.
    {
        has_underlying <int const> u (6);
        BOOST_CHECK_EQUAL (get_underlying (u), 6);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (get_underlying (u)), int const &>));
        BOOST_CHECK_EQUAL (&get_underlying (u), &u.underlying_);
    }
    // Const reference to const.
    {
        has_underlying <int const> const u (6);
        BOOST_CHECK_EQUAL (get_underlying (u), 6);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (get_underlying (u)), int const &>));
        BOOST_CHECK_EQUAL (&get_underlying (u), &u.underlying_);
    }
}

BOOST_AUTO_TEST_CASE (test_range_detail_underlying_forward_to_underlying) {
    // Tested only implicitly, in transform.cpp.
}

BOOST_AUTO_TEST_SUITE_END()

