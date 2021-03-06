/*
Copyright 2013-2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_count
#include "utility/test/boost_unit_test.hpp"

#include "range/count.hpp"

#include <type_traits>

#include "range/core.hpp"
#include "range/for_each_macro.hpp"
#include "range/reverse.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_range_count)

using range::count;
using range::count_from;

using range::default_direction;
using range::empty;
using range::first;
using range::size;
using range::drop;
using range::at;
using range::chop;

using range::front;
using range::back;

using range::is_homogeneous;
using range::reverse;

BOOST_AUTO_TEST_CASE (straightforward) {
    {
        int total = 0;
        RANGE_FOR_EACH (n, count (10))
            total += n;
        BOOST_CHECK_EQUAL (total, 45);
    }
    {
        int total = 0;
        RANGE_FOR_EACH (n, count (5, 8))
            total += n;
        BOOST_CHECK_EQUAL (total, 18);
    }
}

BOOST_AUTO_TEST_CASE (example_decrement_unsigned) {
    // The Google style guide gives an example of looping through a sequence of
    // values from the end to the beginning.
    // If you use an unsigned type and are not careful, this goes wrong.
    // http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
    // #Integer%5FTypes

    std::vector <int> values;
    values.push_back (339);
    values.push_back (17);
    values.push_back (6);
    values.push_back (24);

    // This is a correct implementation.
    {
        int current = 4;
        for (int i = values.size() - 1; i >= 0; -- i) {
            current = values [i] / current;
        }
        BOOST_CHECK_EQUAL (current, 19);
    }
    // This uses "unsigned".
    // This is an incorrect implementation, because "--i" when i == 0 will fail
    // to do what is intended.
    /*{
        int current = 4;
        for (unsigned i = values.size() - 1; i >= 0; -- i) {
            current = values [i] / current;
        }
        BOOST_CHECK_EQUAL (current, 19);
    }*/
    // The correct version using "unsigned" offsets i by one and then says "i-1"
    // every time:
    {
        int current = 4;
        for (unsigned i = values.size(); i > 0; -- i) {
            current = values [i-1] / current;
        }
        BOOST_CHECK_EQUAL (current, 19);
    }
    {
        int current = 4;
        RANGE_FOR_EACH (i, reverse (count (values.size()))) {
            static_assert (std::is_same <decltype (i), std::size_t &&>::value,
                "Should be the same type as values.size(), with &&.");
            current = values [i] / current;
        }
        BOOST_CHECK_EQUAL (current, 19);
    }
}

template <class True, class Zero, class One, class Two, class Three,
    class Four, class Five, class Six>
void check_finite (True true_,
    Zero zero, One one, Two two, Three three, Four four, Five five, Six six)
{
    {
        auto c = count (zero);
        RIME_CHECK_EQUAL (empty (c), true_);
        RIME_CHECK_EQUAL (size (c), zero);
    }
    {
        auto c = count (one);
        RIME_CHECK_EQUAL (empty (c), !true_);
        RIME_CHECK_EQUAL (size (c), one);

        RIME_CHECK_EQUAL (first (c), zero);
        RIME_CHECK_EQUAL (first (c, front), zero);
        RIME_CHECK_EQUAL (first (c, back), zero);

        auto chopped = chop (c);
        RIME_CHECK_EQUAL (chopped.first(), zero);
        BOOST_CHECK (empty (chopped.rest()));
    }
    {
        auto c = count (three);
        RIME_CHECK_EQUAL (empty (c), !true_);
        RIME_CHECK_EQUAL (size (c), three);

        RIME_CHECK_EQUAL (first (c), zero);
        RIME_CHECK_EQUAL (first (c, front), zero);
        RIME_CHECK_EQUAL (first (c, back), two);

        RIME_CHECK_EQUAL (at (c, one), one);
        RIME_CHECK_EQUAL (at (c, one, front), one);
        RIME_CHECK_EQUAL (at (c, two), two);
        RIME_CHECK_EQUAL (at (c, two, front), two);

        RIME_CHECK_EQUAL (at (c, one, back), one);
        RIME_CHECK_EQUAL (at (c, two, back), zero);

        auto c2 = drop (c);
        RIME_CHECK_EQUAL (first (c2), one);
        RIME_CHECK_EQUAL (first (c2, front), one);
        RIME_CHECK_EQUAL (first (c2, back), two);

        auto c3 = drop (c, two);
        RIME_CHECK_EQUAL (first (c3), two);
        RIME_CHECK_EQUAL (first (c3, front), two);
        RIME_CHECK_EQUAL (first (c3, back), two);

        auto c4 = drop (c, 3);
        BOOST_CHECK (empty (c4));

        auto c5 = drop (c, back);
        RIME_CHECK_EQUAL (first (c5), zero);
        RIME_CHECK_EQUAL (first (c5, front), zero);
        RIME_CHECK_EQUAL (first (c5, back), one);

        auto c6 = drop (c, two, back);
        RIME_CHECK_EQUAL (first (c6), zero);
        RIME_CHECK_EQUAL (first (c6, front), zero);
        RIME_CHECK_EQUAL (first (c6, back), zero);

        auto chopped = chop (c, back);
        RIME_CHECK_EQUAL (chopped.first(), two);
        RIME_CHECK_EQUAL (first (chopped.rest()), zero);
        RIME_CHECK_EQUAL (first (chopped.rest(), back), one);
    }
    // Nonzero begin.
    {
        auto c = count (one, one);
        RIME_CHECK_EQUAL (empty (c), true_);
    }
    {
        auto c = count (two, three);
        RIME_CHECK_EQUAL (empty (c), !true_);
        RIME_CHECK_EQUAL (size (c), one);

        RIME_CHECK_EQUAL (first (c), two);
        RIME_CHECK_EQUAL (first (c, back), two);
    }
    {
        auto c = count (three, six);
        auto c2 = drop (count (six), three);

        // This does not have to be true:
        // rime::size_t<1> vs rime::constant <size_t, 1>.
        // BOOST_MPL_ASSERT ((std::is_same <decltype (c), decltype (c2)>));

        RIME_CHECK_EQUAL (first (c), three);
        RIME_CHECK_EQUAL (first (c2), three);
        RIME_CHECK_EQUAL (first (c, back), five);
        RIME_CHECK_EQUAL (first (c2, back), five);

        // If we trust c2, then we trust c.
    }
}

template <class True,
    class MinusTwo, class MinusOne, class Zero, class One, class Two>
void check_finite_negative (True true_,
    MinusTwo minus_two, MinusOne minus_one, Zero zero, One one, Two two)
{
    {
        auto c = count (minus_one, one);
        RIME_CHECK_EQUAL (empty (c), !true_);
        RIME_CHECK_EQUAL (size (c), two);
        RIME_CHECK_EQUAL (first (c), minus_one);

        auto c2 = drop (c);
        RIME_CHECK_EQUAL (first (c2), zero);
        RIME_CHECK_EQUAL (first (drop (c)), zero);
    }
    {
        auto c = count (minus_two, minus_two);
        RIME_CHECK_EQUAL (empty (c), true_);
        RIME_CHECK_EQUAL (size (c), zero);
    }
}

template <class Zero, class One, class Two, class Three, class Four>
    void check_infinite (Zero zero, One one, Two two, Three three, Four four)
{
    auto c = count_from (one);

    RIME_CHECK_EQUAL (empty (c), rime::false_);
    RIME_CHECK_EQUAL (empty (c, front), rime::false_);

    RIME_CHECK_EQUAL (first (c), one);

    auto c2 = drop (c);
    RIME_CHECK_EQUAL (first (c2), two);
    auto c3 = drop (c2, two);
    RIME_CHECK_EQUAL (first (c3), four);

    auto chopped = chop (c2);
    RIME_CHECK_EQUAL (chopped.first(), two);
    RIME_CHECK_EQUAL (first (chopped.rest()), three);
}

template <class MinusTwo, class MinusOne, class Zero, class One, class Two>
    void check_infinite_negative (
        MinusTwo minus_two, MinusOne minus_one, Zero zero, One one, Two two)
{
    auto c = count_from (minus_two);

    BOOST_CHECK (default_direction (c) == front);

    RIME_CHECK_EQUAL (empty (c), rime::false_);
    RIME_CHECK_EQUAL (empty (c, front), rime::false_);

    static_assert (!range::has <
        range::callable::size (decltype (c))>::value, "");
    static_assert (!range::has <
        range::callable::size (direction::front, decltype (c))>::value, "");
    static_assert (!range::has <
        range::callable::first (direction::back, decltype (c))>::value, "");

    RIME_CHECK_EQUAL (first (c), minus_two);
    auto c2 = drop (c);
    RIME_CHECK_EQUAL (first (c2), minus_one);
    auto c3 = drop (c2, two);
    RIME_CHECK_EQUAL (first (c3), one);

    auto chopped = chop (c2);
    RIME_CHECK_EQUAL (chopped.first(), minus_one);
    RIME_CHECK_EQUAL (first (chopped.rest()), zero);
}

BOOST_AUTO_TEST_CASE (heterogeneous) {
    // size_t.
    check_finite (rime::true_,
        rime::size_t <0>(), rime::size_t <1>(), rime::size_t <2>(),
        rime::size_t <3>(), rime::size_t <4>(), rime::size_t <5>(),
        rime::size_t <6>());

    // int.
    check_finite (rime::true_,
        rime::int_<0>(), rime::int_<1>(), rime::int_<2>(),
        rime::int_<3>(), rime::int_<4>(), rime::int_<5>(), rime::int_<6>());
    check_finite_negative (rime::true_,
        rime::int_<-2>(), rime::int_<-1>(),
        rime::int_<0>(), rime::int_<1>(), rime::int_<2>());

    // short.
    check_finite (rime::true_,
        rime::constant <short, 0>(), rime::constant <short, 1>(),
        rime::constant <short, 2>(), rime::constant <short, 3>(),
        rime::constant <short, 4>(), rime::constant <short, 5>(),
        rime::constant <short, 6>());
    check_finite_negative (rime::true_,
        rime::constant <short, -2>(), rime::constant <short, -1>(),
        rime::constant <short, 0>(), rime::constant <short, 1>(),
        rime::constant <short, 2>());

    // Infinite.
    check_infinite (rime::size_t <0>(), rime::size_t <1>(), rime::size_t <2>(),
        rime::size_t <3>(), rime::size_t <4>());

    check_infinite (rime::int_ <0>(), rime::int_ <1>(), rime::int_ <2>(),
        rime::int_ <3>(), rime::int_ <4>());
    check_infinite_negative (rime::int_ <-2>(), rime::int_ <-1>(),
        rime::int_ <0>(), rime::int_ <1>(), rime::int_ <2>());

    check_infinite (rime::constant <short, 0>(), rime::constant <short, 1>(),
        rime::constant <short, 2>(), rime::constant <short, 3>(),
        rime::constant <short, 4>());
    check_infinite_negative (
        rime::constant <short, -2>(), rime::constant <short, -1>(),
        rime::constant <short, 0>(),
        rime::constant <short, 1>(), rime::constant <short, 2>());

    // This causes compiler errors, because the ranges would have negative
    // length:
    // count (rime::int_ <-1>());
    // count (rime::int_ <1>(), rime::int_ <-1>());
    // count (rime::size_t <3>(), rime::size_t <2>());
}

BOOST_AUTO_TEST_CASE (homogeneous) {
    // size_t.
    check_finite (true,
        std::size_t (0), std::size_t (1), std::size_t (2),
        std::size_t (3), std::size_t (4), std::size_t (5), std::size_t (6));

    // int.
    check_finite (true, 0, 1, 2, 3, 4, 5, 6);
    check_finite_negative (true, -2, -1, 0, 1, 2);

    // short.
    check_finite (true, short (0), short (1), short (2),
        short (3), short (4), short (5), short (6));
    check_finite_negative (true, short (-2), short (-1),
        short (0), short (1), short (2));

    // Check that chop works properly.
    {
        auto c = count (3);
        BOOST_MPL_ASSERT ((is_homogeneous <decltype (c)>));

        auto chopped = chop (c, back);
        BOOST_CHECK_EQUAL (chopped.first(), 2);
        BOOST_CHECK_EQUAL (first (chopped.rest()), 0);
        BOOST_CHECK_EQUAL (first (chopped.rest(), back), 1);

        c = drop (chopped.rest());

        BOOST_CHECK_EQUAL (size (c), 1);
        BOOST_CHECK_EQUAL (first (c), 1);

        // chopped has not changed.
        BOOST_CHECK_EQUAL (chopped.first(), 2);
        BOOST_CHECK_EQUAL (first (chopped.rest()), 0);
        BOOST_CHECK_EQUAL (first (chopped.rest(), back), 1);
    }

    // Infinite.
    check_infinite (std::size_t (0), std::size_t (1), std::size_t (2),
        std::size_t (3), std::size_t (4));

    check_infinite (0, 1, 2, 3, 4);
    check_infinite_negative (-2, -1, 0, 1, 2);

    check_infinite (short (0), short (1), short (2), short (3), short (4));
    check_infinite_negative (short (-2), short (-1),
        short (0), short (1), short (2));

    {
        auto c = count (3);
        BOOST_MPL_ASSERT ((is_homogeneous <decltype (c)>));
    }
    {
        // Without arguments: start at std::size_t (0).
        auto c = count();

        BOOST_MPL_ASSERT ((is_homogeneous <decltype (c)>));

        BOOST_CHECK (default_direction (c) == front);

        RIME_CHECK_EQUAL (empty (c), rime::false_);
        RIME_CHECK_EQUAL (empty (c, front), rime::false_);

        RIME_CHECK_EQUAL (first (c), size_t (0));
        c = drop (c);
        RIME_CHECK_EQUAL (first (c), size_t (1));
        c = drop (c, size_t (5));
        RIME_CHECK_EQUAL (first (c), size_t (6));
        c = drop (c, rime::size_t <17>());
        RIME_CHECK_EQUAL (first (c), size_t (23));
    }

    // This causes assertions to fail, because the ranges would have negative
    // length:
    // count (-1);
    // count (1, -1);
    // count (std::size_t (3), std::size_t (2));
}

BOOST_AUTO_TEST_SUITE_END()
