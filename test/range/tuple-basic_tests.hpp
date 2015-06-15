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

#ifndef RANGE_TEST_TUPLE_BASIC_TESTS_HPP_INCLUDED
#define RANGE_TEST_TUPLE_BASIC_TESTS_HPP_INCLUDED

#include "range/tuple.hpp"

#include "rime/check/check_equal.hpp"

using range::tuple;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::at;
using range::at_c;
using range::second;
using range::third;
using range::chop;

using range::front;
using range::back;

using range::view;
using range::view_once;
using range::always_empty;
using range::never_empty;
using range::has;
using range::result_of;

using rime::true_;
using rime::false_;

typedef rime::size_t <0> zero_type;
typedef rime::size_t <1> one_type;
typedef rime::size_t <2> two_type;
typedef rime::size_t <3> three_type;
typedef rime::size_t <4> four_type;
zero_type zero;
one_type one;
two_type two;
three_type three;
four_type four;

namespace callable = range::callable;

template <class Tuple> void check_tuple (Tuple const & t) {
    // empty consistent.
    RIME_CHECK_EQUAL (range::empty (t, front), range::empty (t));
    RIME_CHECK_EQUAL (range::empty (t, back), range::empty (t));

    // size consistent.
    RIME_CHECK_EQUAL (range::size (t, front), range::size (t));
    RIME_CHECK_EQUAL (range::size (t, back), range::size (t));

    // One can always drop with an increment of "size".
    BOOST_MPL_ASSERT ((has <callable::drop (Tuple, decltype (size (t)))>));

    // One can never drop with an run-time increment.
    BOOST_MPL_ASSERT_NOT ((has <callable::drop (Tuple, int)>));
    BOOST_MPL_ASSERT_NOT ((has <callable::at (Tuple, int)>));
}

template <class Increment, class Tuple>
    void check_has_no_drop (Increment const & increment, Tuple const & t)
{
    // No drop.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::drop (Tuple, Increment)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (Tuple, Increment, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (Tuple, Increment, direction::back)>));

    // No at.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::at (Tuple, Increment)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (Tuple, Increment, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (Tuple, Increment, direction::back)>));
}

template <class Tuple> void check_empty_implementation (Tuple const & t) {
    BOOST_MPL_ASSERT ((always_empty <Tuple, direction::front>));
    BOOST_MPL_ASSERT ((always_empty <Tuple, direction::back>));

    RIME_CHECK_EQUAL (range::empty (t), true_);
    RIME_CHECK_EQUAL (range::size (t), zero);

    check_tuple (t);

    // No first.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::first (Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::first (Tuple, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::first (Tuple, direction::back)>));

    // No drop.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::drop (Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (Tuple, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (Tuple, direction::back)>));

    // No at.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::at (Tuple, zero_type)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (Tuple, zero_type, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (Tuple, zero_type, direction::back)>));

    check_has_no_drop (t, one);
    check_has_no_drop (t, two);
}

template <class Tuple> void check_empty (Tuple const & t) {
    check_empty_implementation (t);
    check_empty_implementation (drop (t, zero));
    check_empty_implementation (drop (t, zero, back));
    check_empty_implementation (drop (t, zero, front));
}

template <class Tuple, class Element>
    void check_one_element (Tuple const & t, Element const & element)
{
    BOOST_MPL_ASSERT ((never_empty <Tuple, direction::front>));
    BOOST_MPL_ASSERT ((never_empty <Tuple, direction::back>));

    RIME_CHECK_EQUAL (range::empty (t), false_);
    RIME_CHECK_EQUAL (range::size (t), one);

    RIME_CHECK_EQUAL (first (t), element);
    RIME_CHECK_EQUAL (first (t, front), element);
    RIME_CHECK_EQUAL (first (t, back), element);

    RIME_CHECK_EQUAL (at_c <0> (t), element);
    RIME_CHECK_EQUAL (at_c <0> (t, front), element);
    RIME_CHECK_EQUAL (at_c <0> (t, back), element);

    RIME_CHECK_EQUAL (at (t, zero), element);
    RIME_CHECK_EQUAL (at (t, zero, front), element);
    RIME_CHECK_EQUAL (at (t, zero, back), element);

    check_empty (drop (t));
    check_empty (drop (t, front));
    check_empty (drop (t, back));

    check_has_no_drop (t, two);

    static_assert (has <callable::first (Tuple)>::value, "");
    static_assert (!has <callable::at_c <1> (Tuple)>::value, "");
    static_assert (!has <callable::at_c <2> (Tuple, direction::back)>::value,
        "");
    static_assert (!has <callable::second (Tuple, direction::back)>::value, "");
    static_assert (!has <callable::seventh (Tuple)>::value, "");
}

template <class Tuple, class Element1, class Element2>
    void check_two_elements (
        Tuple const & t, Element1 const & element1, Element2 const & element2)
{
    BOOST_MPL_ASSERT ((never_empty <Tuple, direction::front>));
    BOOST_MPL_ASSERT ((never_empty <Tuple, direction::back>));

    RIME_CHECK_EQUAL (range::empty (t), false_);
    RIME_CHECK_EQUAL (range::size (t), two);

    // first.
    RIME_CHECK_EQUAL (first (t), element1);
    RIME_CHECK_EQUAL (first (t, front), element1);
    RIME_CHECK_EQUAL (first (t, back), element2);

    // at (0).
    RIME_CHECK_EQUAL (at_c <0> (t), element1);
    RIME_CHECK_EQUAL (at_c <0> (t, front), element1);
    RIME_CHECK_EQUAL (at_c <0> (t, back), element2);

    RIME_CHECK_EQUAL (at (t, zero), element1);
    RIME_CHECK_EQUAL (at (t, zero, front), element1);
    RIME_CHECK_EQUAL (at (t, zero, back), element2);

    // at (1).
    RIME_CHECK_EQUAL (at_c <1> (t), element2);
    RIME_CHECK_EQUAL (at_c <1> (t, front), element2);
    RIME_CHECK_EQUAL (at_c <1> (t, back), element1);

    RIME_CHECK_EQUAL (at (t, one), element2);
    RIME_CHECK_EQUAL (at (t, one, front), element2);
    RIME_CHECK_EQUAL (at (t, one, back), element1);

    RIME_CHECK_EQUAL (second (t), element2);
    RIME_CHECK_EQUAL (second (t, front), element2);
    RIME_CHECK_EQUAL (second (t, back), element1);

    // Check subranges.
    check_one_element (drop (t), element2);
    check_one_element (drop (t, front), element2);
    check_one_element (drop (t, back), element1);

    check_empty (drop (t, two));
    check_empty (drop (t, two, front));
    check_empty (drop (t, two, back));

    check_has_no_drop (t, three);

    static_assert (has <callable::second (Tuple)>::value, "");
    static_assert (!has <callable::at_c <2> (Tuple)>::value, "");
    static_assert (!has <callable::at_c <3> (Tuple, direction::back)>::value,
        "");
    static_assert (!has <callable::third (Tuple, direction::back)>::value, "");
    static_assert (!has <callable::seventh (Tuple)>::value, "");
}

template <class Tuple, class Element1, class Element2, class Element3>
    void check_three_elements (
        Tuple const & t, Element1 const & element1, Element2 const & element2,
        Element3 const & element3)
{
    BOOST_MPL_ASSERT ((never_empty <Tuple, direction::front>));
    BOOST_MPL_ASSERT ((never_empty <Tuple, direction::back>));

    RIME_CHECK_EQUAL (range::empty (t), false_);
    RIME_CHECK_EQUAL (range::size (t), three);

    RIME_CHECK_EQUAL (first (t), element1);
    RIME_CHECK_EQUAL (first (t, front), element1);

    RIME_CHECK_EQUAL (first (t, back), element3);

    // at (0).
    RIME_CHECK_EQUAL (at_c <0> (t), element1);
    RIME_CHECK_EQUAL (at_c <0> (t, front), element1);
    RIME_CHECK_EQUAL (at_c <0> (t, back), element3);

    RIME_CHECK_EQUAL (at (t, zero), element1);
    RIME_CHECK_EQUAL (at (t, zero, front), element1);
    RIME_CHECK_EQUAL (at (t, zero, back), element3);

    // at (1).
    RIME_CHECK_EQUAL (at_c <1> (t), element2);
    RIME_CHECK_EQUAL (at_c <1> (t, front), element2);
    RIME_CHECK_EQUAL (at_c <1> (t, back), element2);

    RIME_CHECK_EQUAL (at (t, one), element2);
    RIME_CHECK_EQUAL (at (t, one, front), element2);
    RIME_CHECK_EQUAL (at (t, one, back), element2);

    RIME_CHECK_EQUAL (second (t), element2);
    RIME_CHECK_EQUAL (second (t, front), element2);
    RIME_CHECK_EQUAL (second (t, back), element2);

    // at (2).
    RIME_CHECK_EQUAL (at_c <2> (t), element3);
    RIME_CHECK_EQUAL (at_c <2> (t, front), element3);
    RIME_CHECK_EQUAL (at_c <2> (t, back), element1);

    RIME_CHECK_EQUAL (at (t, two), element3);
    RIME_CHECK_EQUAL (at (t, two, front), element3);
    RIME_CHECK_EQUAL (at (t, two, back), element1);

    RIME_CHECK_EQUAL (third (t), element3);
    RIME_CHECK_EQUAL (third (t, front), element3);
    RIME_CHECK_EQUAL (third (t, back), element1);

    // Check subranges.
    check_two_elements (drop (t), element2, element3);
    check_two_elements (drop (t, front), element2, element3);
    check_two_elements (drop (t, back), element1, element2);

    check_one_element (drop (t, two), element3);
    check_one_element (drop (t, two, front), element3);
    check_one_element (drop (t, two, back), element1);

    check_empty (drop (t, three));
    check_empty (drop (t, three, front));
    check_empty (drop (t, three, back));

    check_has_no_drop (four, t);

    static_assert (has <callable::third (Tuple)>::value, "");
    static_assert (!has <callable::at_c <3> (Tuple)>::value, "");
    static_assert (!has <callable::at_c <4> (Tuple, direction::back)>::value,
        "");
    static_assert (!has <callable::fourth (Tuple, direction::back)>::value, "");
    static_assert (!has <callable::seventh (Tuple)>::value, "");
}

#endif // RANGE_TEST_TUPLE_BASIC_TESTS_HPP_INCLUDED
