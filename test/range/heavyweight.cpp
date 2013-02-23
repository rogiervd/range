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

#define BOOST_TEST_MODULE test_range_iterator_range
#include "../boost_unit_test.hpp"

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

namespace range {

    template <> struct tag_of_bare <weird_heavyweight_count>
    { typedef heavyweight_tag <weird_heavyweight_count> type; };

    namespace operation {
        template <>
            struct default_direction <heavyweight_tag <weird_heavyweight_count>>
        {
            // Should not be called.
            forgotten_to_define_direction operator() (weird_heavyweight_count);
        };

        template <>
            struct make_view <heavyweight_tag <weird_heavyweight_count>,
                meta::vector <weird_direction>>
        {
            weird_count operator() (
                weird_direction const &, weird_heavyweight_count)
            { return weird_count(); }
        };

    } // namespace operation

} // namespace range

/*
Adapt STL containers (badly) as heavyweights.
*/
namespace range {

    // Of course the containers have too few parameters here.
    template <typename Type> struct tag_of_bare <std::forward_list <Type>>
    { typedef heavyweight_tag <std::forward_list <Type>> type; };

    template <typename Type> struct tag_of_bare <std::vector <Type>>
    { typedef heavyweight_tag <std::vector <Type>> type; };

    namespace operation {

        namespace detail {
            template <class Container> struct view_stl_container {
                typedef typename Container::iterator iterator;
                typedef typename Container::const_iterator
                    const_iterator;

                template <class Direction>
                iterator_range <iterator> operator() (
                    Direction const &, Container & list) const
                {
                    return iterator_range <iterator> (list.begin(), list.end());
                }

                template <class Direction>
                iterator_range <const_iterator> operator() (
                    Direction const &, Container const & list) const
                {
                    return iterator_range <const_iterator> (
                        list.begin(), list.end());
                }
            };

        } // namespace detail

        template <typename Type>
            struct make_view <heavyweight_tag <std::forward_list <Type>>,
                meta::vector <direction::front>>
        : detail::view_stl_container <std::forward_list <Type>> {};

        template <typename Type>
            struct make_view <heavyweight_tag <std::vector <Type>>,
                meta::vector <direction::front>>
        : detail::view_stl_container <std::vector <Type>> {};

        template <typename Type>
            struct make_view <heavyweight_tag <std::vector <Type>>,
                meta::vector <direction::back>>
        : detail::view_stl_container <std::vector <Type>> {};

    } // namespace operation
} // namespace range

BOOST_AUTO_TEST_SUITE(test_range_heavyweight)

BOOST_AUTO_TEST_CASE (test_range_heavyweight) {
    BOOST_MPL_ASSERT ((range::has::empty <std::forward_list <int>>));
    BOOST_MPL_ASSERT ((range::has::empty <
        direction::front, std::forward_list <int> &>));
    BOOST_MPL_ASSERT_NOT ((range::has::size <std::forward_list <int> const>));
    BOOST_MPL_ASSERT_NOT ((range::has::size <
        direction::front &, std::forward_list <int> const &>));

    using range::empty;
    using range::size;
    using range::first;
    using range::drop;
    using range::front;
    using range::back;

    rime::int_ <1> one;
    rime::int_ <2> two;

    // std::forward_list
    {
        std::forward_list <int> l;

        BOOST_CHECK (empty (l));

        l.push_front (3);
        BOOST_CHECK (!empty (l));
        BOOST_CHECK_EQUAL (first (l), 3);

        l.push_front (5);
        BOOST_CHECK (!empty (l));
        BOOST_CHECK_EQUAL (first (l), 5);
        BOOST_CHECK_EQUAL (first (drop (l)), 3);
        BOOST_CHECK (empty (drop (drop (l))));
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
        BOOST_CHECK_EQUAL (first (front, v), 3.3);
        BOOST_CHECK_EQUAL (first (back, v), 5.5);
        BOOST_CHECK_EQUAL (first (front, v), 3.3);
        BOOST_CHECK_EQUAL (first (back, v), 5.5);

        BOOST_CHECK_EQUAL (first (drop (v)), 5.5);
        BOOST_CHECK_EQUAL (first (drop (back, v)), 3.3);
        BOOST_CHECK_EQUAL (first (back, drop (back, one, v)), 3.3);

        BOOST_CHECK (empty (drop (drop (v))));
        BOOST_CHECK (empty (drop (back, drop (v))));
        BOOST_CHECK (empty (drop (front, drop (back, one, v))));
        BOOST_CHECK (empty (drop (2, v)));
        BOOST_CHECK (empty (drop (back, 2u, v)));
        BOOST_CHECK (empty (drop (two, v)));
        BOOST_CHECK (empty (drop (back, two, v)));
    }

    BOOST_MPL_ASSERT_NOT ((range::operation::is_implemented<range::apply::view <
        forgotten_to_define_direction, weird_heavyweight_count> >));

    // weird_heavyweight_count
    {
        // weird_direction.
        BOOST_MPL_ASSERT ((range::has::view <
            weird_direction, weird_heavyweight_count>));
        BOOST_MPL_ASSERT ((range::has::empty <
            weird_direction, weird_heavyweight_count>));
        BOOST_MPL_ASSERT_NOT ((range::has::size <
            weird_direction, weird_heavyweight_count>));
        BOOST_MPL_ASSERT ((range::has::first <
            weird_direction, weird_heavyweight_count const &>));
        BOOST_MPL_ASSERT ((range::has::drop <
            weird_direction, weird_heavyweight_count &>));
        BOOST_MPL_ASSERT ((range::has::drop <
            weird_direction, int, weird_heavyweight_count &>));

        // without direction
        BOOST_MPL_ASSERT_NOT ((range::has::view <weird_heavyweight_count>));
        BOOST_MPL_ASSERT_NOT ((range::has::empty <weird_heavyweight_count>));
        BOOST_MPL_ASSERT_NOT ((range::has::size <weird_heavyweight_count>));
        BOOST_MPL_ASSERT_NOT ((
            range::has::first <weird_heavyweight_count const &>));
        BOOST_MPL_ASSERT_NOT ((range::has::drop <weird_heavyweight_count &>));
        BOOST_MPL_ASSERT_NOT ((
            range::has::drop <int, weird_heavyweight_count &>));

        weird_heavyweight_count w;
        weird_direction d (7);

        BOOST_CHECK (!empty (d, w));
        BOOST_CHECK_EQUAL (first (d, w), 0);
        BOOST_CHECK_EQUAL (first (d, drop (d, w)), 1);
        BOOST_CHECK_EQUAL (first (d, drop (d, one, w)), 1);
        BOOST_CHECK_EQUAL (first (d, drop (d, two, w)), 2);
    }
}

BOOST_AUTO_TEST_SUITE_END()

