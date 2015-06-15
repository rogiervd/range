/*
Copyright 2013, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_reverse
#include "utility/test/boost_unit_test.hpp"

#include "range/reverse.hpp"

#include <vector>
#include <list>
#include <tuple>

#include <boost/mpl/assert.hpp>

#include "range/std/container.hpp"
#include "range/std/tuple.hpp"

#include "rime/check/check_equal.hpp"

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

BOOST_AUTO_TEST_CASE (test_range_reverse_static) {
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::reverse (int)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::reverse ()>));
    BOOST_MPL_ASSERT ((
        range::has <range::callable::reverse (std::vector <int> &)>));
}

BOOST_AUTO_TEST_CASE (test_range_reverse_homogeneous) {
    /* std::vector. */
    {
        std::vector <int> v;
        BOOST_CHECK (default_direction (reverse (v)) == default_direction (v));

        BOOST_CHECK (empty (reverse (v)));
        BOOST_CHECK (empty (reverse (v), front));
        BOOST_CHECK (empty (reverse (v), back));

        BOOST_CHECK_EQUAL (size (reverse (v)), 0u);
        BOOST_CHECK_EQUAL (size (reverse (v), front), 0u);
        BOOST_CHECK_EQUAL (size (reverse (v), back), 0u);


        v.push_back (7);
        // v == reverse (v) == { 7 }.

        BOOST_CHECK (!empty (reverse (v)));
        BOOST_CHECK (!empty (reverse (v), front));
        BOOST_CHECK (!empty (reverse (v), back));

        BOOST_CHECK_EQUAL (size (reverse (v)), 1u);
        BOOST_CHECK_EQUAL (size (reverse (v), front), 1u);
        BOOST_CHECK_EQUAL (size (reverse (v), back), 1u);

        BOOST_CHECK_EQUAL (first (reverse (v)), 7);
        BOOST_CHECK_EQUAL (first (reverse (v), front), 7);
        BOOST_CHECK_EQUAL (first (reverse (v), back), 7);

        BOOST_CHECK (empty (drop (reverse (v))));
        BOOST_CHECK (empty (drop (reverse (v), front)));
        BOOST_CHECK (empty (drop (reverse (v), back)));

        v.push_back (18);
        // v == {7, 18}; reverse (v) == { 18, 7 }.

        BOOST_CHECK (!empty (reverse (v)));
        BOOST_CHECK (!empty (reverse (v), front));
        BOOST_CHECK (!empty (reverse (v), back));

        BOOST_CHECK_EQUAL (size (reverse (v)), 2u);
        BOOST_CHECK_EQUAL (size (reverse (v), front), 2u);
        BOOST_CHECK_EQUAL (size (reverse (v), back), 2u);

        BOOST_CHECK_EQUAL (first (reverse (v)), 18);
        BOOST_CHECK_EQUAL (first (reverse (v), front), 18);
        BOOST_CHECK_EQUAL (first (reverse (v), back), 7);

        BOOST_CHECK (!empty (drop (reverse (v))));
        BOOST_CHECK (!empty (drop (reverse (v), front)));
        BOOST_CHECK (!empty (drop (reverse (v), back)));

        BOOST_CHECK_EQUAL (first (drop (reverse (v))), 7);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), front), front), 7);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), back), front), 18);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), front), back), 7);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), back), back), 18);

        BOOST_CHECK_EQUAL (first (drop (reverse (v), 1)), 7);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 1, front), front), 7);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 1, back), front), 18);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 1, front), back), 7);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 1, back), back), 18);

        BOOST_CHECK (empty (drop (reverse (v), 2)));
        BOOST_CHECK (empty (drop (reverse (v), 2, front)));
        BOOST_CHECK (empty (drop (reverse (v), 2, back)));

        v.push_back (29);
        // v == {7, 18, 29}; reverse (v) == { 29, 18, 7 }.

        BOOST_CHECK (!empty (reverse (v)));
        BOOST_CHECK (!empty (reverse (v), front));
        BOOST_CHECK (!empty (reverse (v), back));

        BOOST_CHECK_EQUAL (size (reverse (v)), 3u);
        BOOST_CHECK_EQUAL (size (reverse (v), front), 3u);
        BOOST_CHECK_EQUAL (size (reverse (v), back), 3u);

        BOOST_CHECK_EQUAL (first (reverse (v)), 29);
        BOOST_CHECK_EQUAL (first (reverse (v), front), 29);
        BOOST_CHECK_EQUAL (first (reverse (v), back), 7);

        BOOST_CHECK (!empty (drop (reverse (v))));
        BOOST_CHECK (!empty (drop (reverse (v), front)));
        BOOST_CHECK (!empty (drop (reverse (v), back)));

        BOOST_CHECK_EQUAL (size (drop (reverse (v))), 2u);
        BOOST_CHECK_EQUAL (first (drop (reverse (v))), 18);
        BOOST_CHECK_EQUAL (size (drop (reverse (v), 1)), 2u);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 1)), 18);
        BOOST_CHECK_EQUAL (size (drop (reverse (v), 2)), 1u);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 2)), 7);
        BOOST_CHECK_EQUAL (size (drop (reverse (v), 3)), 0u);
        BOOST_CHECK (empty (drop (reverse (v), 3)));

        BOOST_CHECK_EQUAL (size (drop (reverse (v), back), back), 2u);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), back), back), 18);
        BOOST_CHECK_EQUAL (size (drop (reverse (v), 1, back), back), 2u);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 1, back), back), 18);
        BOOST_CHECK_EQUAL (size (drop (reverse (v), 2, back), back), 1u);
        BOOST_CHECK_EQUAL (first (drop (reverse (v), 2, back), back), 29);
        BOOST_CHECK_EQUAL (size (drop (reverse (v), 3, back), back), 0u);
        BOOST_CHECK (empty (drop (reverse (v), 3, back), back));

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
        static_assert (
            !range::has <range::callable::size (decltype (r))>::value, "");
        static_assert (
            !range::has <range::callable::drop (int, decltype (r))>::value, "");

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

        BOOST_CHECK_EQUAL (first (r, back), 'H');
        BOOST_CHECK_EQUAL (first (drop (r, back), back), 'e');
        BOOST_CHECK (empty (drop (drop (r, back), back)));


        l.push_back ('l');
        r = reverse (l);

        BOOST_CHECK (!empty (r));
        BOOST_CHECK_EQUAL (first (r), 'l');
        BOOST_CHECK_EQUAL (first (drop (r)), 'e');
        BOOST_CHECK_EQUAL (first (drop (drop (r))), 'H');
        BOOST_CHECK (empty (drop (drop (drop (r)))));

        BOOST_CHECK_EQUAL (first (r, back), 'H');
        BOOST_CHECK_EQUAL (first (drop (r, back), back), 'e');
        BOOST_CHECK_EQUAL (first (drop (drop (r, back), back), back), 'l');
        BOOST_CHECK (empty (drop (drop (drop (r, back), back)), back));
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

        RIME_CHECK_EQUAL (empty (drop (r, rime::size_t <1u>())), rime::true_);
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

        RIME_CHECK_EQUAL (first (drop (r, rime::size_t <1u>(), back), back),
            12.25);
        RIME_CHECK_EQUAL (empty (drop (r, rime::size_t <2u>())), rime::true_);
    }

    {
        std::tuple <int, double, bool> t (4, 7.25, false);
        auto r = reverse (t);

        RIME_CHECK_EQUAL (empty (r), rime::false_);
        RIME_CHECK_EQUAL (size (r), rime::size_t <3u>());

        RIME_CHECK_EQUAL (first (r), false);
        RIME_CHECK_EQUAL (first (drop (r)), 7.25);
        RIME_CHECK_EQUAL (first (drop (r, rime::size_t <2u>())), 4);
        first (drop (r, rime::size_t <2u>())) -= 3;
        BOOST_CHECK_EQUAL (first (t), 1);

        RIME_CHECK_EQUAL (first (drop (r, rime::size_t <2u>(), back), back),
            false);

        RIME_CHECK_EQUAL (empty (drop (r, rime::size_t <3u>())), rime::true_);
    }

}

BOOST_AUTO_TEST_SUITE_END()
