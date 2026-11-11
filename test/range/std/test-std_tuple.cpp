// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_container_std_tuple
#include "boost/test/unit_test.hpp"

#include "range/std/tuple.hpp"

#include <tuple>
#include <type_traits>
#include <utility>

#include "utility/test/tracked.hpp"

#include "../check_equal_behaviour.hpp"
#include "rime/check/check_equal.hpp"
#include "rime/core.hpp"

#include "range/core.hpp"
#include "range/direction.hpp"

BOOST_AUTO_TEST_SUITE(test_container_std_tuple)

using range::at;
using range::at_c;
using range::chop;
using range::drop;
using range::empty;
using range::first;
using range::size;

namespace callable = range::callable;

using range::back;
using range::front;

using range::view;
using range::view_once;

using range::always_empty;
using range::has;
using range::is_homogeneous;
using range::is_view;
using range::never_empty;

typedef rime::size_t<0> zero_type;
rime::size_t<1> one;
rime::size_t<2> two;
rime::size_t<3> three;

using rime::false_;
using rime::true_;

using utility::tracked;
using utility::tracked_counts;

BOOST_AUTO_TEST_CASE(test_std_tuple_types)
{
    {
        static_assert(!is_view<std::tuple<>>::value);
        static_assert(!is_view<std::tuple<>>::value);
        static_assert(!is_homogeneous<std::tuple<>>::value);
        std::tuple<> t;
        static_assert(is_view<decltype(view(t))>::value);
        static_assert(!is_homogeneous<decltype(view(t))>::value);
    }
    {
        static_assert(!is_view<std::tuple<int>>::value);
        static_assert(!is_homogeneous<std::tuple<int>>::value);
        std::tuple<int> t(4);
        static_assert(is_view<decltype(view(t)) &>::value);
        static_assert(!is_homogeneous<decltype(view(t))>::value);
    }
    {
        static_assert(!is_view<std::tuple<int, double> &>::value);
        static_assert(!is_homogeneous<std::tuple<int, double>>::value);
        std::tuple<int, double> t(4, 5.);
        static_assert(is_view<decltype(view(t)) const>::value);
        static_assert(!is_homogeneous<decltype(view(t))>::value);
    }

    {
        std::tuple<int> t;
        static_assert(std::is_same_v<decltype(first(t)), int &>);
    }
    {
        std::tuple<int> const t;
        static_assert(std::is_same_v<decltype(first(t)), int const &>);
    }
    {
        std::tuple<int> t;
        static_assert(std::is_same_v<decltype(first(std::move(t))), int &&>);
    }
}

BOOST_AUTO_TEST_CASE(test_std_tuple)
{
    // Zero elements
    {
        std::tuple<> t;
        BOOST_CHECK(empty(t));
        static_assert(always_empty<decltype(t), direction::front>::value);
        static_assert(always_empty<decltype(t), direction::back>::value);
        check_equal_value(size(t), zero_type());

        static_assert(!has<range::callable::first(decltype(t))>::value);
        static_assert(!has<range::callable::at(decltype(t), zero_type)>::value);
        static_assert(
            !has<range::callable::at(decltype(t), rime::size_t<1>)>::value);

        static_assert(!has<range::callable::chop(decltype(t))>::value);
    }

    // One element.
    {
        std::tuple<double> const t(6.3);
        BOOST_CHECK(!empty(t));
        static_assert(never_empty<decltype(t), direction::front>::value);
        static_assert(never_empty<decltype(t), direction::back>::value);
        check_equal_value(size(t), one);

        static_assert(has<range::callable::first(decltype(t) &)>::value);
        static_assert(has<range::callable::at(decltype(t), zero_type)>::value);
        static_assert(
            !has<range::callable::at(rime::size_t<1>, decltype(t))>::value);

        static_assert(has<range::callable::chop(decltype(t))>::value);
        static_assert(has<range::callable::chop(decltype(view(t)))>::value);

        // Tuples are not homogeneous.
        static_assert(!has<range::callable::chop_in_place(decltype(t))>::value);
        static_assert(
            !has<range::callable::chop_in_place(decltype(view(t)))>::value);
        static_assert(
            !has<range::callable::chop_in_place(decltype(view(t)) &)>::value);

        check_equal_value(first(t), 6.3);
        check_equal_value(first(t, front), 6.3);
        check_equal_value(first(t, back), 6.3);

        check_equal_value(at(t, zero_type()), 6.3);
        check_equal_value(at(t, zero_type(), front), 6.3);
        check_equal_value(at(t, zero_type(), back), 6.3);

        auto first_and_empty_1 = range::chop(t);
        auto first_and_empty_2 = range::chop(t, back);
        check_equal_value(first_and_empty_1.first(), 6.3);
        check_equal_value(first_and_empty_2.first(), 6.3);
        check_equal_value(empty(first_and_empty_1.rest()), true_);
        check_equal_value(empty(first_and_empty_2.rest()), true_);
        static_assert(
            always_empty<
                decltype(first_and_empty_2.rest()), direction::front>::value);
        static_assert(
            always_empty<
                decltype(first_and_empty_2.rest()), direction::back>::value);
    }

    // Two elements.
    {
        std::tuple<int, char> t(4, 'a');

        BOOST_CHECK(!empty(t));
        static_assert(never_empty<decltype(t), direction::front>::value);
        static_assert(never_empty<decltype(t), direction::back>::value);
        check_equal_value(size(t), two);

        check_equal_value(first(t, front), 4);
        check_equal_value(first(t), 4);
        check_equal_value(first(t, back), 'a');

        check_equal_value(first(drop(t)), 'a');

        BOOST_CHECK(!empty(drop(t, back)));
        BOOST_CHECK(!empty(drop(t)));
        BOOST_CHECK(empty(drop(t, two)));
        BOOST_CHECK(empty(drop(t, two, back)));

        check_equal_value(at(t, zero_type()), 4);
        check_equal_value(at(t, one), 'a');
        check_equal_value(at(t, zero_type(), front), 4);
        check_equal_value(at(t, zero_type(), back), 'a');
        check_equal_value(at(t, one, back), 4);

        auto first_and_rest = range::chop(t);
        auto last_and_rest = range::chop(t, back);
        check_equal_value(first_and_rest.first(), 4);
        check_equal_value(last_and_rest.first(), 'a');
        check_equal_value(size(first_and_rest.rest()), one);
        check_equal_value(size(last_and_rest.rest()), one);

        auto first_and_empty = range::chop(last_and_rest.rest());
        check_equal_value(first_and_empty.first(), 4);
        check_equal_value(empty(first_and_empty.rest()), true_);

        static_assert(!has<range::callable::chop_in_place(decltype(t))>::value);
        static_assert(
            !has<range::callable::chop_in_place(decltype(view(t)))>::value);
        static_assert(
            !has<range::callable::chop_in_place(decltype(view(t)) &)>::value);
    }

    // Three elements
    {
        std::tuple<int, char, double> t(4, 'a', 6.3);

        BOOST_CHECK(!empty(t));
        check_equal_value(size(t), three);

        check_equal_value(first(t, front), 4);
        check_equal_value(first(t), 4);
        check_equal_value(first(drop(t)), 'a');
        check_equal_value(first(drop(t, back), back), 'a');
        check_equal_value(first(t, back), 6.3);

        BOOST_CHECK(!empty(drop(t, back)));
        BOOST_CHECK(!empty(drop(t)));
        BOOST_CHECK(!empty(drop(t, two)));
        BOOST_CHECK(empty(drop(t, three)));
        BOOST_CHECK(!empty(drop(t, two, back)));
        BOOST_CHECK(empty(drop(t, three, back)));

        check_equal_value(at(t, zero_type()), 4);
        check_equal_value(at(t, one), 'a');
        check_equal_value(at(t, two), 6.3);
        check_equal_value(at(t, zero_type(), front), 4);
        check_equal_value(at(t, one, front), 'a');
        check_equal_value(at(t, two, front), 6.3);
        check_equal_value(at(t, zero_type(), back), 6.3);
        check_equal_value(at(t, one, back), 'a');
        check_equal_value(at(t, two, back), 4);

        auto first_and_rest = range::chop(t);
        auto last_and_rest = range::chop(t, back);
        check_equal_value(first_and_rest.first(), 4);
        check_equal_value(last_and_rest.first(), 6.3);
        check_equal_value(size(first_and_rest.rest()), two);
        check_equal_value(size(last_and_rest.rest()), two);

        auto first_and_middle = range::chop(last_and_rest.rest());
        check_equal_value(first_and_middle.first(), 4);
        check_equal_value(size(first_and_middle.rest()), one);

        auto middle_and_empty = range::chop(first_and_middle.rest());
        check_equal_value(middle_and_empty.first(), 'a');
        check_equal_value(empty(middle_and_empty.rest()), true_);

        static_assert(!has<range::callable::chop_in_place(decltype(t))>::value);
        static_assert(
            !has<range::callable::chop_in_place(decltype(view(t)))>::value);
        static_assert(
            !has<range::callable::chop_in_place(decltype(view(t)) &)>::value);
    }

    utility::tracked_registry r;
    {
        // Use view_once: elements are returned by rvalue reference.
        std::tuple<tracked<int>, tracked<double>> t(
            tracked<int>(r, 7), tracked<double>(r, 4.5));
        RIME_CHECK_EQUAL(first(t).content(), 7);
        RIME_CHECK_EQUAL(first(t, back).content(), 4.5);

        // Check the status quo.
        BOOST_CHECK_EQUAL(r.counts(), tracked_counts(2, 0, 2, 0, 0, 0, 0, 2));

        auto v = view_once(std::move(t));
        static_assert(std::is_same_v<decltype(first(v)), tracked<int> &&>);
        static_assert(
            std::is_same_v<decltype(first(v, back)), tracked<double> &&>);

        // The elements should be moved out.
        tracked<int> i = at_c<0>(v);
        BOOST_CHECK_EQUAL(i.content(), 7);
        BOOST_CHECK_EQUAL(r.counts(), tracked_counts(2, 0, 3, 0, 0, 0, 0, 2));

        tracked<double> d = at_c<1>(v);
        BOOST_CHECK_EQUAL(d.content(), 4.5);
        BOOST_CHECK_EQUAL(r.counts(), tracked_counts(2, 0, 4, 0, 0, 0, 0, 2));
    }
}

BOOST_AUTO_TEST_CASE(test_std_pair)
{
    std::tuple<int, char> t(4, 'a');
    std::pair<int, char> p(4, 'a');

    // HasSize, HasBack, HasDropConstantN, HasDropRuntimeN.
    check_equal_behaviour<
        rime::true_type, rime::true_type, rime::true_type, rime::false_type>(
        p, t);

    auto && second = at_c<1>(view_once(p));
    static_assert(std::is_same_v<decltype(second), char &>);
    BOOST_CHECK_EQUAL(second, 'a');

    auto && second_move = at_c<1>(view_once(std::move(p)));
    static_assert(std::is_same_v<decltype(second_move), char &&>);
    BOOST_CHECK_EQUAL(second_move, 'a');
}

BOOST_AUTO_TEST_SUITE_END()
