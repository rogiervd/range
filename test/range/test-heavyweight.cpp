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

#define BOOST_TEST_MODULE test_range_iterator_range
#include "utility/test/boost_unit_test.hpp"

#include "range/heavyweight.hpp"

#include <forward_list>
#include <vector>

#include "weird_count.hpp"
#include "range/iterator_range.hpp"

/**
Fake heavyweight class that has a wrong direction as a default_direction, and
converts to weird_count for traversal.
*/
struct weird_heavyweight_count {};

struct weird_heavyweight_count_tag : range::operation::heavyweight_tag {};

namespace range {

    template <> struct tag_of_qualified <weird_heavyweight_count>
    { typedef weird_heavyweight_count_tag type; };

} // namespace range

// Should not be called.
forgotten_to_define_direction implement_default_direction (
    range::operation::heavyweight_tag, weird_heavyweight_count);

inline weird_count implement_make_view (weird_heavyweight_count_tag,
    bool once, weird_heavyweight_count, weird_direction const &)
{ return weird_count(); }

struct std_forward_list_tag : range::operation::heavyweight_tag {};
struct std_vector_tag : range::operation::heavyweight_tag {};

/*
Adapt STL containers (badly) as heavyweights.
*/
namespace range {

    // Of course the containers have too few parameters here.
    template <typename Type> struct tag_of_qualified <std::forward_list <Type>>
    { typedef std_forward_list_tag type; };

    template <typename Type> struct tag_of_qualified <std::vector <Type>>
    { typedef std_vector_tag type; };

} // namespace range

// Rvalue, only if once == true.
template <class Container, class Result
    = range::iterator_range <std::move_iterator <typename Container::iterator>>>
inline Result view_stl_container (
    rime::true_type once, Container && container, utility::overload_order <1> *)
{
    return Result (
        std::move_iterator <typename Container::iterator> (container.begin()),
        std::move_iterator <typename Container::iterator> (container.end()));
}

// Reference.
template <class Container, class Result
    = range::iterator_range <typename Container::iterator>>
inline Result view_stl_container (bool once, Container & container,
    utility::overload_order <2> *)
{ return Result (container.begin(), container.end()); }

// Const reference.
template <class Container, class Result
    = range::iterator_range <typename Container::const_iterator>>
inline Result view_stl_container (bool once, Container const & container,
    utility::overload_order <3> *)
{ return Result (container.begin(), container.end()); }

template <class Once, class Container>
inline auto implement_make_view (std_forward_list_tag,
    Once once, Container && container, direction::front const &)
RETURNS (view_stl_container <typename std::decay <Container>::type> (
    once, std::forward <Container> (container), utility::pick_overload()));

template <class Once, class Container>
inline auto implement_make_view (std_vector_tag,
    Once once, Container && container, range::helper::front_or_back const &)
RETURNS (view_stl_container <typename std::decay <Container>::type> (
    once, std::forward <Container> (container), utility::pick_overload()));

BOOST_AUTO_TEST_SUITE(test_range_heavyweight)

BOOST_AUTO_TEST_CASE (test_range_heavyweight) {
    BOOST_MPL_ASSERT ((range::has <range::callable::empty (
        std::forward_list <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::empty (
        std::forward_list <int> &, direction::front)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        std::forward_list <int> const)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
        std::forward_list <int> const &, direction::front &)>));

    using range::empty;
    using range::size;
    using range::first;
    using range::at;
    using range::drop;
    using range::chop;
    using range::front;
    using range::back;

    rime::int_ <1> one;
    rime::int_ <2> two;

    // std::forward_list.
    {
        std::forward_list <int> l;

        BOOST_CHECK (empty (l));

        l.push_front (3);
        BOOST_CHECK (!empty (l));
        BOOST_CHECK_EQUAL (first (l), 3);

        auto first_and_rest = chop (l);
        BOOST_CHECK_EQUAL (first_and_rest.first(), 3);
        BOOST_CHECK (empty (first_and_rest.rest()));

        l.push_front (5);
        BOOST_CHECK (!empty (l));
        BOOST_CHECK_EQUAL (first (l), 5);
        BOOST_CHECK_EQUAL (first (drop (l)), 3);
        BOOST_CHECK (empty (drop (drop (l))));

        first_and_rest = chop (l);
        BOOST_CHECK_EQUAL (first_and_rest.first(), 5);
        first_and_rest = chop (first_and_rest.rest());
        BOOST_CHECK_EQUAL (first_and_rest.first(), 3);
        BOOST_CHECK (empty (first_and_rest.rest()));

        auto v = range::view (l);
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::chop_in_place (decltype (l) &)>));
        BOOST_MPL_ASSERT ((
            range::has <range::callable::chop_in_place (decltype (v) &)>));

        // view_once on lvalue: equivalent to view.
        auto v2 = range::view_once (l);
        BOOST_MPL_ASSERT ((std::is_same <decltype (v2), decltype (v)>));

        // view_once on rvalue reference: move elements out.
        auto v3 = range::view_once (std::move (l));
        BOOST_MPL_ASSERT_NOT ((std::is_same <decltype (v3), decltype (v)>));
        auto && element = first (v3);
        BOOST_CHECK_EQUAL (element, 5);
        BOOST_MPL_ASSERT ((std::is_same <decltype (element), int &&>));
    }

    // std::vector
    {
        std::vector <double> v;
        BOOST_CHECK (empty (v));
        BOOST_CHECK_EQUAL (size (v), 0u);

        v.push_back (3.3);
        BOOST_CHECK (!empty (v));
        BOOST_CHECK_EQUAL (size (v), 1u);
        BOOST_CHECK_EQUAL (first (v), 3.3);

        v.push_back (5.5);
        BOOST_CHECK (!empty (v));
        BOOST_CHECK_EQUAL (size (v), 2u);
        BOOST_CHECK_EQUAL (first (v, front), 3.3);
        BOOST_CHECK_EQUAL (first (v, back), 5.5);
        BOOST_CHECK_EQUAL (first (v, front), 3.3);
        BOOST_CHECK_EQUAL (first (v, back), 5.5);

        BOOST_CHECK_EQUAL (first (drop (v)), 5.5);
        BOOST_CHECK_EQUAL (first (drop (v, back)), 3.3);
        BOOST_CHECK_EQUAL (first (drop (v, one, back), back), 3.3);

        BOOST_CHECK_EQUAL (at (v, 0), 3.3);
        BOOST_CHECK_EQUAL (at (v, 1), 5.5);
        BOOST_CHECK_EQUAL (at (v, 0, back), 5.5);
        BOOST_CHECK_EQUAL (at (v, 1, back), 3.3);

        BOOST_CHECK (empty (drop (drop (v))));
        BOOST_CHECK (empty (drop (drop (v), one, back), back));
        BOOST_CHECK (empty (drop (drop (v, front))));
        BOOST_CHECK (empty (drop (v, 2)));
        BOOST_CHECK (empty (drop (v, 2u, back)));
        BOOST_CHECK (empty (drop (v, two)));
        BOOST_CHECK (empty (drop (v, two, back)));

        auto moved_view = range::view_once (std::move (v));

        auto && first_element = at (moved_view, 0);
        BOOST_CHECK_EQUAL (first_element, 3.3);
        BOOST_MPL_ASSERT ((std::is_same <decltype (first_element), double &&>));

        auto && second_element = at (moved_view, 1);
        BOOST_CHECK_EQUAL (second_element, 5.5);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (second_element), double &&>));
    }

    // weird_heavyweight_count
    {
        // weird_direction.
        BOOST_MPL_ASSERT ((range::has <range::callable::view (
            weird_heavyweight_count, weird_direction)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::empty (
            weird_heavyweight_count, weird_direction)>));
        BOOST_MPL_ASSERT_NOT ((range::has <range::callable::size (
            weird_heavyweight_count, weird_direction)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::first (
            weird_heavyweight_count const &, weird_direction)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            weird_heavyweight_count &, weird_direction)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (
            weird_heavyweight_count &, int, weird_direction)>));
        BOOST_MPL_ASSERT ((range::has <range::callable::chop (
            weird_heavyweight_count &, weird_direction)>));

        // without direction.
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::view (weird_heavyweight_count)>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::empty (weird_heavyweight_count)>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::size (weird_heavyweight_count)>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::first (weird_heavyweight_count const &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::drop (weird_heavyweight_count &)>));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::drop (int, weird_heavyweight_count &)>));

        weird_heavyweight_count w;
        weird_direction d (7);

        BOOST_CHECK (!empty (w, d));
        BOOST_CHECK_EQUAL (first (w, d), 0);
        BOOST_CHECK_EQUAL (first (drop (w, d), d), 1);
        BOOST_CHECK_EQUAL (first (drop (w, one, d), d), 1);
        BOOST_CHECK_EQUAL (first (drop (w, two, d), d), 2);

        auto first_and_rest = chop (w, d);
        BOOST_CHECK_EQUAL (first_and_rest.first(), 0);
        BOOST_CHECK_EQUAL (first (first_and_rest.rest(), d), 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
