// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_subrange
#include "boost/test/unit_test.hpp"

#include "range/subrange.hpp"

#include <vector>

#include "check_equal_behaviour.hpp"
#include "rime/check/check_equal.hpp"

#include "range/count.hpp"
#include "range/for_each_macro.hpp"
#include "range/std/vector.hpp"
#include "range/tuple.hpp"

using range::count;
using range::make_subrange;
using range::tuple;

using range::drop;
using range::empty;
using range::first;
using range::second;
using range::size;

using range::back;
using range::front;

using rime::false_;
using rime::false_type;
using rime::true_;
using rime::true_type;

template <class Type> struct show_type;

BOOST_AUTO_TEST_SUITE(test_range_subrange)

/**
\param original A range that has values 0, 1, ..., 9.
*/
template <class Count> void check_with_count(Count const & original)
{
    {
        auto sub = make_subrange(original);
        BOOST_CHECK(!empty(sub));
        BOOST_CHECK(!empty(sub, front));
        BOOST_CHECK(!empty(sub, back));

        BOOST_CHECK_EQUAL(size(sub), 10);
        BOOST_CHECK_EQUAL(size(sub, front), 10);
        BOOST_CHECK_EQUAL(size(sub, back), 10);

        BOOST_CHECK_EQUAL(first(sub), 0);
        BOOST_CHECK_EQUAL(first(sub, front), 0);
        BOOST_CHECK_EQUAL(first(sub, back), 9);

        auto sub2 = drop(sub, 3);
        BOOST_CHECK_EQUAL(first(sub2), 3);

        {
            auto first_half = sub2.before();
            BOOST_CHECK_EQUAL(first(first_half), 0);
            BOOST_CHECK_EQUAL(first(first_half, back), 2);

            auto second_half = sub2.underlying();
            BOOST_CHECK_EQUAL(first(second_half), 3);
            BOOST_CHECK_EQUAL(first(second_half, back), 9);
        }

        auto sub3 = drop(sub2, 3, back);
        {
            BOOST_CHECK_EQUAL(first(sub3), 3);
            BOOST_CHECK_EQUAL(first(sub3, back), 6);

            auto first_half = sub3.before();
            BOOST_CHECK_EQUAL(first(first_half), 0);
            BOOST_CHECK_EQUAL(first(first_half, back), 2);

            auto second_half = sub3.underlying();
            BOOST_CHECK_EQUAL(first(second_half), 3);
            BOOST_CHECK_EQUAL(first(second_half, back), 6);
        }

        auto sub4 = make_subrange(sub, back);

        {
            auto middle = drop(drop(sub4, 2, back), 4, front);
            BOOST_CHECK_EQUAL(first(middle), 4);
            BOOST_CHECK_EQUAL(first(middle, back), 7);

            // show_type <decltype (middle)> s;

            auto before = middle.underlying().before();
            BOOST_CHECK_EQUAL(first(before), 0);
            BOOST_CHECK_EQUAL(first(before, back), 3);

            auto after = middle.before();
            BOOST_CHECK_EQUAL(first(after), 8);
            BOOST_CHECK_EQUAL(first(after, back), 9);
        }
    }
    {
        auto sub = make_subrange(make_subrange(original, back));
        auto middle = drop(drop(sub, 2, back), 4, front);
        BOOST_CHECK_EQUAL(first(middle), 4);
        BOOST_CHECK_EQUAL(first(middle, back), 7);

        // show_type <decltype (middle)> s;

        auto before = middle.before();
        BOOST_CHECK_EQUAL(first(before), 0);
        BOOST_CHECK_EQUAL(first(before, back), 3);

        auto after = middle.underlying().before();
        BOOST_CHECK_EQUAL(first(after), 8);
        BOOST_CHECK_EQUAL(first(after, back), 9);
    }
    // \todo Nested subranges in various directions.
    {}
}

BOOST_AUTO_TEST_CASE(test_range_subrange_basic)
{
    // Type that does not have a specialisation.
    check_with_count(count(10));

    // Type that does have a specialisation.
    std::vector<int> v;
    RANGE_FOR_EACH(i, count(10))
    v.push_back(i);
    check_with_count(v);

    // Type that does not have a size.
    // \todo std::list.
}

// \todo Check editing homogeneous subrange.

// \todo Heterogeneous ranges!

BOOST_AUTO_TEST_CASE(test_range_subrange_heterogeneous)
{
    // Spot checks.
    {
        tuple<> t;
        auto s1 = make_subrange(t);
        RIME_CHECK_EQUAL(empty(s1), true_);
        RIME_CHECK_EQUAL(empty(s1.before()), true_);
        auto s2 = make_subrange(t, back);
        RIME_CHECK_EQUAL(empty(s2), true_);
        RIME_CHECK_EQUAL(empty(s2.before()), true_);
    }
    {
        tuple<char> t('q');
        auto s1 = make_subrange(t);
        RIME_CHECK_EQUAL(empty(s1), false_);
        RIME_CHECK_EQUAL(size(s1), rime::size_t<1>());
        BOOST_CHECK_EQUAL(first(s1), 'q');

        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s1, t);

        auto s2 = drop(s1);
        RIME_CHECK_EQUAL(empty(s2), true_);
        RIME_CHECK_EQUAL(empty(s2.before()), false_);
        RIME_CHECK_EQUAL(size(s2.before()), rime::size_t<1>());
        BOOST_CHECK_EQUAL(first(s2.before()), 'q');

        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s2, tuple<>());
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s2.before(), t);

        auto s3 = drop(make_subrange(t, back), back);
        // show_type <decltype (s3)> st;
        RIME_CHECK_EQUAL(empty(s3), true_);
        RIME_CHECK_EQUAL(empty(s3.before()), false_);
        RIME_CHECK_EQUAL(size(s3.before()), rime::size_t<1>());
        BOOST_CHECK_EQUAL(first(s3.before()), 'q');

        // This drops the last element both from the subrange and from the
        // underlying range.
        auto s4 = drop(s1, back);
        // show_type <decltype (s4)> st;
        RIME_CHECK_EQUAL(empty(s4), true_);
        RIME_CHECK_EQUAL(empty(s4.before()), true_);
    }
    {
        tuple<char, int, double> t('q', 27, 3.5);

        auto s1 = make_subrange(t, front);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s1, t);

        auto s2 = drop(s1, rime::size_t<2>());
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s2, tuple<double>(3.5));
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s2.before(), tuple<char, int>('q', 27));

        auto s3 = drop(s1, back);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s3, tuple<char, int>('q', 27));

        auto s4 = drop(s1, rime::size_t<2>(), back);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s4, tuple<char>('q'));
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s4.before(), tuple<>());

        // subrange from the back.
        auto s5 = make_subrange(t, back);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s5, t);

        // Drop from the front.
        auto s6 = drop(s5);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s6, tuple<int, double>(27, 3.5));

        auto s7 = drop(s5, back);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s7, tuple<char, int>('q', 27));
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s7.before(), tuple<double>(3.5));

        auto s8 = drop(s5, rime::size_t<2>(), back);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s8, tuple<char>('q'));
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s8.before(), tuple<int, double>(27, 3.5));

        // Drop from the back and the front.
        auto s9 = make_subrange(s5, front);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s9, t);

        auto s10 = drop(s9);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s10, tuple<int, double>(27, 3.5));
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s10.before(), tuple<char>('q'));

        auto s11 = drop(s10, back);
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s11, tuple<int>(27));
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s11.before(), tuple<char>('q'));
        check_equal_behaviour<true_type, true_type, true_type, false_type>(
            s11.underlying().before(), tuple<double>(3.5));

        // Edit elements in the subrange.
        first(s11) = 45;
        BOOST_CHECK_EQUAL(second(t), 45);
    }
}

// \todo Check editing heterogeneous subrange.

// \todo Good idea?
// The error messages are horrendous but seem to refer to using non-constant
// lengths on a heterogeneous sequence.
/*BOOST_AUTO_TEST_CASE (test_range_subrange_tuple) {
    check_with_count (range::make_tuple(
        0, 1u, short(2), 3, 4, 5, 6, 7, 8, 9
        ));
}*/

// \todo Test with weird_range.

// \todo Test with directions that can have different values...

BOOST_AUTO_TEST_SUITE_END()
