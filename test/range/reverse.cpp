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

#define BOOST_TEST_MODULE test_range_reverse
#include "../boost_unit_test.hpp"

#include "range/reverse.hpp"

#include <vector>
#include <list>
#include <tuple>

#include "range/std.hpp"

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_reverse)

using range::reverse;
using range::front;
using range::back;

using range::default_direction;
using range::front;

using range::empty;
using range::size;
using range::first;
using range::drop;
using range::is_homogeneous;

BOOST_AUTO_TEST_CASE (test_range_reverse_homogeneous) {
    /* std::vector. */
    {
        std::vector <int> v;
        BOOST_CHECK (default_direction (reverse (v)) == default_direction (v));

        BOOST_CHECK (empty (reverse (v)));
        BOOST_CHECK (empty (front, reverse (v)));
        BOOST_CHECK (empty (back, reverse (v)));

        BOOST_CHECK_EQUAL (size (reverse (v)), 0u);
        BOOST_CHECK_EQUAL (size (front, reverse (v)), 0u);
        BOOST_CHECK_EQUAL (size (back, reverse (v)), 0u);


        v.push_back (7);
        // v == reverse (v) == { 7 }.

        BOOST_CHECK (!empty (reverse (v)));
        BOOST_CHECK (!empty (front, reverse (v)));
        BOOST_CHECK (!empty (back, reverse (v)));

        BOOST_CHECK_EQUAL (size (reverse (v)), 1u);
        BOOST_CHECK_EQUAL (size (front, reverse (v)), 1u);
        BOOST_CHECK_EQUAL (size (back, reverse (v)), 1u);

        BOOST_CHECK_EQUAL (first (reverse (v)), 7);
        BOOST_CHECK_EQUAL (first (front, reverse (v)), 7);
        BOOST_CHECK_EQUAL (first (back, reverse (v)), 7);

        BOOST_CHECK (empty (drop (reverse (v))));
        BOOST_CHECK (empty (drop (front, reverse (v))));
        BOOST_CHECK (empty (drop (back, reverse (v))));

        v.push_back (18);
        // v == {7, 18}; reverse (v) == { 18, 7 }.

        BOOST_CHECK (!empty (reverse (v)));
        BOOST_CHECK (!empty (front, reverse (v)));
        BOOST_CHECK (!empty (back, reverse (v)));

        BOOST_CHECK_EQUAL (size (reverse (v)), 2u);
        BOOST_CHECK_EQUAL (size (front, reverse (v)), 2u);
        BOOST_CHECK_EQUAL (size (back, reverse (v)), 2u);

        BOOST_CHECK_EQUAL (first (reverse (v)), 18);
        BOOST_CHECK_EQUAL (first (front, reverse (v)), 18);
        BOOST_CHECK_EQUAL (first (back, reverse (v)), 7);

        BOOST_CHECK (!empty (drop (reverse (v))));
        BOOST_CHECK (!empty (drop (front, reverse (v))));
        BOOST_CHECK (!empty (drop (back, reverse (v))));

        BOOST_CHECK_EQUAL (first (drop (reverse (v))), 7);
        BOOST_CHECK_EQUAL (first (front, drop (front, reverse (v))), 7);
        BOOST_CHECK_EQUAL (first (front, drop (back, reverse (v))), 18);
        BOOST_CHECK_EQUAL (first (back, drop (front, reverse (v))), 7);
        BOOST_CHECK_EQUAL (first (back, drop (back, reverse (v))), 18);

        BOOST_CHECK_EQUAL (first (drop (1, reverse (v))), 7);
        BOOST_CHECK_EQUAL (first (front, drop (front, 1, reverse (v))), 7);
        BOOST_CHECK_EQUAL (first (front, drop (back, 1, reverse (v))), 18);
        BOOST_CHECK_EQUAL (first (back, drop (front, 1, reverse (v))), 7);
        BOOST_CHECK_EQUAL (first (back, drop (back, 1, reverse (v))), 18);

        BOOST_CHECK (empty (drop (2, reverse (v))));
        BOOST_CHECK (empty (drop (front, 2, reverse (v))));
        BOOST_CHECK (empty (drop (back, 2, reverse (v))));

        v.push_back (29);
        // v == {7, 18, 29}; reverse (v) == { 29, 18, 7 }.

        BOOST_CHECK (!empty (reverse (v)));
        BOOST_CHECK (!empty (front, reverse (v)));
        BOOST_CHECK (!empty (back, reverse (v)));

        BOOST_CHECK_EQUAL (size (reverse (v)), 3u);
        BOOST_CHECK_EQUAL (size (front, reverse (v)), 3u);
        BOOST_CHECK_EQUAL (size (back, reverse (v)), 3u);

        BOOST_CHECK_EQUAL (first (reverse (v)), 29);
        BOOST_CHECK_EQUAL (first (front, reverse (v)), 29);
        BOOST_CHECK_EQUAL (first (back, reverse (v)), 7);

        BOOST_CHECK (!empty (drop (reverse (v))));
        BOOST_CHECK (!empty (drop (front, reverse (v))));
        BOOST_CHECK (!empty (drop (back, reverse (v))));

        BOOST_CHECK_EQUAL (size (drop (reverse (v))), 2u);
        BOOST_CHECK_EQUAL (first (drop (reverse (v))), 18);
        BOOST_CHECK_EQUAL (size (drop (1, reverse (v))), 2u);
        BOOST_CHECK_EQUAL (first (drop (1, reverse (v))), 18);
        BOOST_CHECK_EQUAL (size (drop (2, reverse (v))), 1u);
        BOOST_CHECK_EQUAL (first (drop (2, reverse (v))), 7);
        BOOST_CHECK_EQUAL (size (drop (3, reverse (v))), 0u);
        BOOST_CHECK (empty (drop (3, reverse (v))));

        BOOST_CHECK_EQUAL (size (back, drop (back, reverse (v))), 2u);
        BOOST_CHECK_EQUAL (first (back, drop (back, reverse (v))), 18);
        BOOST_CHECK_EQUAL (size (back, drop (back, 1, reverse (v))), 2u);
        BOOST_CHECK_EQUAL (first (back, drop (back, 1, reverse (v))), 18);
        BOOST_CHECK_EQUAL (size (back, drop (back, 2, reverse (v))), 1u);
        BOOST_CHECK_EQUAL (first (back, drop (back, 2, reverse (v))), 29);
        BOOST_CHECK_EQUAL (size (back, drop (back, 3, reverse (v))), 0u);
        BOOST_CHECK (empty (back, drop (back, 3, reverse (v))));

        // Mutability.
        first (reverse (v)) = 40;
        BOOST_CHECK_EQUAL (v [2], 40);
        first (drop (reverse (v))) = 50;
        BOOST_CHECK_EQUAL (v [1], 50);
    }

    /* std::list. */
    {
        std::list <char> l;

        auto r = reverse (l);
        static_assert (!range::has::size <decltype (r)>::value, "");
        static_assert (!range::has::drop <int, decltype (r)>::value, "");

        BOOST_CHECK (empty (r));

        l.push_back ('H');
        r = reverse (l);

        BOOST_CHECK (!empty (r));
        BOOST_CHECK_EQUAL (first (r), 'H');
        BOOST_CHECK (empty (drop (r)));


        l.push_back ('e');
        r = reverse (l);

        BOOST_CHECK (!empty (r));
        BOOST_CHECK_EQUAL (first (r), 'e');
        BOOST_CHECK_EQUAL (first (drop (r)), 'H');
        BOOST_CHECK (empty (drop (drop (r))));

        BOOST_CHECK_EQUAL (first (back, r), 'H');
        BOOST_CHECK_EQUAL (first (back, drop (back, r)), 'e');
        BOOST_CHECK (empty (drop (back, drop (back, r))));


        l.push_back ('l');
        r = reverse (l);

        BOOST_CHECK (!empty (r));
        BOOST_CHECK_EQUAL (first (r), 'l');
        BOOST_CHECK_EQUAL (first (drop (r)), 'e');
        BOOST_CHECK_EQUAL (first (drop (drop (r))), 'H');
        BOOST_CHECK (empty (drop (drop (drop (r)))));

        BOOST_CHECK_EQUAL (first (back, r), 'H');
        BOOST_CHECK_EQUAL (first (back, drop (back, r)), 'e');
        BOOST_CHECK_EQUAL (first (back, drop (back, drop (back, r))), 'l');
        BOOST_CHECK (empty (drop (back, drop (back, drop (back, r)))));
    }
}

BOOST_AUTO_TEST_CASE (test_range_reverse_heterogeneous) {
    {
        std::tuple<> t;

        RIME_CHECK_EQUAL (empty (reverse (t)), rime::true_);
        RIME_CHECK_EQUAL (size (reverse (t)), rime::size_t <0u>());
    }

    {
        std::tuple <int> t (4);
        auto r = reverse (t);

        RIME_CHECK_EQUAL (empty (r), rime::false_);
        RIME_CHECK_EQUAL (size (r), rime::size_t <1u>());

        BOOST_CHECK_EQUAL (first (r), 4);
        first (r) += 5;
        BOOST_CHECK_EQUAL (first (t), 9);

        RIME_CHECK_EQUAL (empty (drop (rime::size_t <1u>(), r)), rime::true_);
    }

    {
        std::tuple <int, double> t (4, 7.25);
        auto r = reverse (t);

        RIME_CHECK_EQUAL (empty (r), rime::false_);
        RIME_CHECK_EQUAL (size (r), rime::size_t <2u>());

        RIME_CHECK_EQUAL (first (r), 7.25);
        first (r) += 5;
        RIME_CHECK_EQUAL (first (drop (t)), 12.25);
        RIME_CHECK_EQUAL (first (drop (r)), 4);

        RIME_CHECK_EQUAL (first (back, drop (back, rime::size_t <1u>(), r)),
            12.25);
        RIME_CHECK_EQUAL (empty (drop (rime::size_t <2u>(), r)), rime::true_);
    }

    {
        std::tuple <int, double, bool> t (4, 7.25, false);
        auto r = reverse (t);

        RIME_CHECK_EQUAL (empty (r), rime::false_);
        RIME_CHECK_EQUAL (size (r), rime::size_t <3u>());

        RIME_CHECK_EQUAL (first (r), false);
        RIME_CHECK_EQUAL (first (drop (r)), 7.25);
        RIME_CHECK_EQUAL (first (drop (rime::size_t <2u>(), r)), 4);
        first (drop (rime::size_t <2u>(), r)) -= 3;
        BOOST_CHECK_EQUAL (first (t), 1);

        RIME_CHECK_EQUAL (first (back, drop (back, rime::size_t <2u>(), r)),
            false);

        RIME_CHECK_EQUAL (empty (drop (rime::size_t <3u>(), r)), rime::true_);
    }

}

BOOST_AUTO_TEST_SUITE_END()
