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

#define BOOST_TEST_MODULE test_range_count
#include "utility/test/boost_unit_test.hpp"

#include "range/count.hpp"

#include <type_traits>
#include <boost/mpl/int.hpp>

#include "range/core.hpp"

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_count)

using range::count;

using range::default_direction;
using range::front;

using range::empty;
using range::first;
using range::drop;
using range::is_homogeneous;

BOOST_AUTO_TEST_CASE (test_range_count_homogeneous) {
    {
        auto c = count (-1);

        static_assert (is_homogeneous <decltype (c)>::value, "");

        BOOST_CHECK (default_direction (c) == front);

        BOOST_CHECK (!empty (c));
        BOOST_CHECK (!empty (front, c));

        static_assert (!range::has <
            range::callable::size (decltype (c))>::value, "");
        static_assert (!range::has <
            range::callable::size (direction::front, decltype (c))>::value, "");

        static_assert (!range::has <
            range::callable::first (direction::back, decltype (c))>::value, "");

        RIME_CHECK_EQUAL (first (c), -1);
        c = drop (c);
        RIME_CHECK_EQUAL (first (c), 0);
        c = drop (5, c);
        RIME_CHECK_EQUAL (first (c), 5);
        c = drop (rime::int_ <17>(), c);
        RIME_CHECK_EQUAL (first (c), 22);
    }
    {
        // Start at std::size_t (0).
        auto c = count();

        static_assert (is_homogeneous <decltype (c)>::value, "");

        BOOST_CHECK (default_direction (c) == front);

        BOOST_CHECK (!empty (c));
        BOOST_CHECK (!empty (front, c));

        RIME_CHECK_EQUAL (first (c), size_t (0));
        c = drop (c);
        RIME_CHECK_EQUAL (first (c), size_t (1));
        c = drop (size_t (5), c);
        RIME_CHECK_EQUAL (first (c), size_t (6));
        c = drop (rime::size_t <17>(), c);
        RIME_CHECK_EQUAL (first (c), size_t (23));
    }
    {
        // Different types for current value and increment: should remain short.
        auto c = count (short (5));

        static_assert (is_homogeneous <decltype (c)>::value, "");
        RIME_CHECK_EQUAL (first (c), short (5));

        auto c2 = drop (c);
        RIME_CHECK_EQUAL (first (c2), short (6));
        auto c3 = drop (5, c2);
        RIME_CHECK_EQUAL (first (c3), short (11));
        auto c4 = drop (std::size_t (7), c3);
        RIME_CHECK_EQUAL (first (c4), short (18));
    }
}

BOOST_AUTO_TEST_CASE (test_range_count_heterogeneous) {
    {
        auto c = count (rime::int_ <-3>());

        static_assert (!is_homogeneous <decltype (c)>::value, "");

        BOOST_CHECK (default_direction (c) == front);

        BOOST_CHECK (!empty (c));
        BOOST_CHECK (!empty (front, c));

        static_assert (!range::has <
            range::callable::size (decltype (c))>::value, "");
        static_assert (!range::has <
            range::callable::size (direction::front, decltype (c))>::value, "");

        static_assert (!range::has <
            range::callable::first (direction::back, decltype (c))>::value, "");

        RIME_CHECK_EQUAL (first (c), rime::int_ <-3>());
        auto c2 = drop (c);
        RIME_CHECK_EQUAL (first (c2), rime::int_ <-2>());
        // Continue as constant.
        {
            auto c3 = drop (rime::int_ <5>(), c2);
            RIME_CHECK_EQUAL (first (c3), (std::integral_constant <int, 3>()));
            auto c4 = drop (rime::size_t <17>(), c3);
            RIME_CHECK_EQUAL (first (c4), boost::mpl::int_ <20>());
        }
        // Continue as run-time value.
        {
            auto c3 = drop (5, c2);
            RIME_CHECK_EQUAL (first (c3), 3);
            auto c4 = drop (rime::constant <short, 17>(), c3);
            RIME_CHECK_EQUAL (first (c4), 20);
            auto c5 = drop (std::size_t (3), c4);
            RIME_CHECK_EQUAL (first (c5), 23);
        }
    }

    {
        auto c = count (rime::size_t <3>());

        // Keep type (i.e. do not turn into a signed type).
        {
            // Heterogeneous.
            auto c2 = drop (rime::int_ <2>(), c);
            RIME_CHECK_EQUAL (first (c2), rime::size_t <5>());
        }
        {
            // Homogeneous.
            auto c2 = drop (2, c);
            RIME_CHECK_EQUAL (first (c2), std::size_t (5));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

