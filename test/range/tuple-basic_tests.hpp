/*
Copyright 2013, 2014 Rogier van Dalen.

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

#ifndef RANGE_TEST_TUPLE_BASIC_TESTS_HPP_INCLUDED
#define RANGE_TEST_TUPLE_BASIC_TESTS_HPP_INCLUDED

#include "range/tuple.hpp"

#include "check_equal.hpp"

using range::tuple;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::at;
using range::at_c;
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
    RIME_CHECK_EQUAL (range::empty (front, t), range::empty (t));
    RIME_CHECK_EQUAL (range::empty (back, t), range::empty (t));

    // size consistent.
    RIME_CHECK_EQUAL (range::size (front, t), range::size (t));
    RIME_CHECK_EQUAL (range::size (back, t), range::size (t));

    // One can always drop with an increment of "size".
    BOOST_MPL_ASSERT ((has <callable::drop (callable::size (Tuple), Tuple)>));

    // One can never drop with an run-time increment.
    BOOST_MPL_ASSERT_NOT ((has <callable::drop (int, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((has <callable::at (int, Tuple)>));
}

template <class Increment, class Tuple>
    void check_has_no_drop (Increment const & increment, Tuple const & t)
{
    // No drop.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::drop (Increment, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (direction::front, Increment, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (direction::back, Increment, Tuple)>));

    // No at.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::at (Increment, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (direction::front, Increment, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (direction::back, Increment, Tuple)>));
}

template <class Tuple> void check_empty_implementation (Tuple const & t) {
    BOOST_MPL_ASSERT ((always_empty <direction::front, Tuple>));
    BOOST_MPL_ASSERT ((always_empty <direction::back, Tuple>));

    RIME_CHECK_EQUAL (range::empty (t), true_);
    RIME_CHECK_EQUAL (range::size (t), zero);

    check_tuple (t);

    // No first.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::first (Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::first (direction::front, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::first (direction::back, Tuple)>));

    // No drop.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::drop (Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (direction::front, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::drop (direction::back, Tuple)>));

    // No at.
    BOOST_MPL_ASSERT_NOT ((range::has <callable::at (zero_type, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (direction::front, zero_type, Tuple)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        callable::at (direction::back, zero_type, Tuple)>));

    check_has_no_drop (one, t);
    check_has_no_drop (two, t);
}

template <class Tuple> void check_empty (Tuple const & t) {
    check_empty_implementation (t);
    check_empty_implementation (drop (zero, t));
    check_empty_implementation (drop (back, zero, t));
    check_empty_implementation (drop (front, zero, t));
}

template <class Tuple, class Element>
    void check_one_element (Tuple const & t, Element const & element)
{
    BOOST_MPL_ASSERT ((never_empty <direction::front, Tuple>));
    BOOST_MPL_ASSERT ((never_empty <direction::back, Tuple>));

    RIME_CHECK_EQUAL (range::empty (t), false_);
    RIME_CHECK_EQUAL (range::size (t), one);

    RIME_CHECK_EQUAL (first (t), element);
    RIME_CHECK_EQUAL (first (front, t), element);
    RIME_CHECK_EQUAL (first (back, t), element);

    RIME_CHECK_EQUAL (at_c <0> (t), element);
    RIME_CHECK_EQUAL (at_c <0> (front, t), element);
    RIME_CHECK_EQUAL (at_c <0> (back, t), element);

    RIME_CHECK_EQUAL (at (zero, t), element);
    RIME_CHECK_EQUAL (at (front, zero, t), element);
    RIME_CHECK_EQUAL (at (back, zero, t), element);

    check_empty (drop (t));
    check_empty (drop (front, t));
    check_empty (drop (back, t));

    check_has_no_drop (two, t);
}

template <class Tuple, class Element1, class Element2>
    void check_two_elements (
        Tuple const & t, Element1 const & element1, Element2 const & element2)
{
    BOOST_MPL_ASSERT ((never_empty <direction::front, Tuple>));
    BOOST_MPL_ASSERT ((never_empty <direction::back, Tuple>));

    RIME_CHECK_EQUAL (range::empty (t), false_);
    RIME_CHECK_EQUAL (range::size (t), two);

    // first.
    RIME_CHECK_EQUAL (first (t), element1);
    RIME_CHECK_EQUAL (first (front, t), element1);
    RIME_CHECK_EQUAL (first (back, t), element2);

    // at (0).
    RIME_CHECK_EQUAL (at_c <0> (t), element1);
    RIME_CHECK_EQUAL (at_c <0> (front, t), element1);
    RIME_CHECK_EQUAL (at_c <0> (back, t), element2);

    RIME_CHECK_EQUAL (at (zero, t), element1);
    RIME_CHECK_EQUAL (at (front, zero, t), element1);
    RIME_CHECK_EQUAL (at (back, zero, t), element2);

    // at (1).
    RIME_CHECK_EQUAL (at_c <1> (t), element2);
    RIME_CHECK_EQUAL (at_c <1> (front, t), element2);
    RIME_CHECK_EQUAL (at_c <1> (back, t), element1);

    RIME_CHECK_EQUAL (at (one, t), element2);
    RIME_CHECK_EQUAL (at (front, one, t), element2);
    RIME_CHECK_EQUAL (at (back, one, t), element1);

    // Check subranges.
    check_one_element (drop (t), element2);
    check_one_element (drop (front, t), element2);
    check_one_element (drop (back, t), element1);

    check_empty (drop (two, t));
    check_empty (drop (front, two, t));
    check_empty (drop (back, two, t));

    check_has_no_drop (three, t);
}

template <class Tuple, class Element1, class Element2, class Element3>
    void check_three_elements (
        Tuple const & t, Element1 const & element1, Element2 const & element2,
        Element3 const & element3)
{
    BOOST_MPL_ASSERT ((never_empty <direction::front, Tuple>));
    BOOST_MPL_ASSERT ((never_empty <direction::back, Tuple>));

    RIME_CHECK_EQUAL (range::empty (t), false_);
    RIME_CHECK_EQUAL (range::size (t), three);

    RIME_CHECK_EQUAL (first (t), element1);
    RIME_CHECK_EQUAL (first (front, t), element1);

    RIME_CHECK_EQUAL (first (back, t), element3);

    // at (0).
    RIME_CHECK_EQUAL (at_c <0> (t), element1);
    RIME_CHECK_EQUAL (at_c <0> (front, t), element1);
    RIME_CHECK_EQUAL (at_c <0> (back, t), element3);

    RIME_CHECK_EQUAL (at (zero, t), element1);
    RIME_CHECK_EQUAL (at (front, zero, t), element1);
    RIME_CHECK_EQUAL (at (back, zero, t), element3);

    // at (1).
    RIME_CHECK_EQUAL (at_c <1> (t), element2);
    RIME_CHECK_EQUAL (at_c <1> (front, t), element2);
    RIME_CHECK_EQUAL (at_c <1> (back, t), element2);

    RIME_CHECK_EQUAL (at (one, t), element2);
    RIME_CHECK_EQUAL (at (front, one, t), element2);
    RIME_CHECK_EQUAL (at (back, one, t), element2);

    // at (2).
    RIME_CHECK_EQUAL (at_c <2> (t), element3);
    RIME_CHECK_EQUAL (at_c <2> (front, t), element3);
    RIME_CHECK_EQUAL (at_c <2> (back, t), element1);

    RIME_CHECK_EQUAL (at (two, t), element3);
    RIME_CHECK_EQUAL (at (front, two, t), element3);
    RIME_CHECK_EQUAL (at (back, two, t), element1);

    // Check subranges.
    check_two_elements (drop (t), element2, element3);
    check_two_elements (drop (front, t), element2, element3);
    check_two_elements (drop (back, t), element1, element2);

    check_one_element (drop (two, t), element3);
    check_one_element (drop (front, two, t), element3);
    check_one_element (drop (back, two, t), element1);

    check_empty (drop (three, t));
    check_empty (drop (front, three, t));
    check_empty (drop (back, three, t));

    check_has_no_drop (four, t);
}

#endif // RANGE_TEST_TUPLE_BASIC_TESTS_HPP_INCLUDED
