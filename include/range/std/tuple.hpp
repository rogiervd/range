/*
Copyright 2013, 2015 Rogier van Dalen.

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

/** \file
Turn heterogeneous containers in std:: into ranges.
This means: std::tuple and std::pair.
*/

#ifndef RANGE_STD_TUPLE_HPP_INCLUDED
#define RANGE_STD_TUPLE_HPP_INCLUDED

// It is not obvious how to forward-declare std::get from in libstdc++.
// So just include the headers.
// This is an implementation detail and subject to change in future versions.
#include <utility>
#include <tuple>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/pair.hpp>

#include "meta/vector.hpp"
#include "meta/transform.hpp"
#include "meta/enumerate.hpp"

#include "range/core.hpp"
#include "range/heavyweight.hpp"
#include "range/member_view.hpp"

#include "range/detail/enable_if_front_back.hpp"

namespace range {

    // Tags.

    template <class First, class Second>
        struct tag_of_qualified <std::pair <First, Second>>
    { typedef heavyweight_tag <std::pair <First, Second>> type; };

    template <class ... Types>
        struct tag_of_qualified <std::tuple <Types ...>>
    { typedef heavyweight_tag <std::tuple <Types ...>> type; };

    namespace operation {

        namespace detail {

            /**
            Implementation of make_view for std::pair.
            Uses member_view with the "first" and "second" member variables.
            */
            template <bool Move, class First, class Second>
                struct make_view_std_pair;

            // Lvalue reference.
            template <class First, class Second>
                struct make_view_std_pair <false, First, Second>
            {
                typedef std::pair <First, Second> pair;
                typedef meta::vector <
                        member_extractor <First pair::*, &pair::first>,
                        member_extractor <Second pair::*, &pair::second>>
                    extractors_type;

                member_view <pair &, extractors_type>
                    operator() (pair & p) const
                { return member_view <pair &, extractors_type> (p); }

                member_view <pair const &, extractors_type>
                    operator() (pair const & p) const
                { return member_view <pair const &, extractors_type> (p); }
            };

            // Rvalue reference.
            template <class First, class Second>
                struct make_view_std_pair <true, First, Second>
            : make_view_std_pair <false, First, Second>
            {
                typedef make_view_std_pair <false, First, Second> base_type;
                typedef typename base_type::pair pair;
                typedef typename base_type::extractors_type extractors_type;

                using make_view_std_pair <false, First, Second>::operator();

                member_view <pair &&, extractors_type>
                    operator() (pair && p) const
                {
                    return member_view <pair &&, extractors_type> (
                        std::move (p));
                }
            };

            /**
            Extractor of elements from std::tuple.
            */
            template <class Index> struct std_tuple_extractor {
                template <class ... Types>
                    auto operator() (std::tuple <Types ...> & tuple) const
                RETURNS (std::get <Index::value> (tuple));

                template <class ... Types>
                    auto operator() (std::tuple <Types ...> const & tuple) const
                RETURNS (std::get <Index::value> (tuple));

                /**
                C++11 does not have an overload specific for rvalue references;
                only C++14 does.
                For consistency, manually cast the result to an rvalue
                reference.
                */
                template <class ... Types> typename
                    std::tuple_element <Index::value, std::tuple <Types ...>
                >::type && operator() (std::tuple <Types ...> && tuple) const
                {
                    typedef typename std::tuple_element <Index::value,
                        std::tuple <Types ...>>::type && result_type;
                    return static_cast <result_type> (
                        std::get <Index::value> (tuple));
                }
            };

            /**
            Implementation of make_view for std::tuple.
            This is essentially a generalisation of make_view_std_pair.
            */
            template <bool Move, class ... Types> struct make_view_std_tuple {
                // Create meta::vector <
                //     std_tuple_extractors <rime::size_t<0>>, ...>.
                typedef typename meta::as_vector <meta::transform <
                    std_tuple_extractor <boost::mpl::first <boost::mpl::_>>,
                    typename meta::enumerate <meta::vector <Types...>>::type
                    >>::type extractors_type;

                typedef std::tuple <Types ...> tuple_type;

                member_view <tuple_type &, extractors_type>
                    operator() (tuple_type & t) const
                {
                    return member_view <tuple_type &, extractors_type> (t);
                }

                member_view <tuple_type const &, extractors_type>
                    operator() (tuple_type const & t) const
                {
                    return member_view <tuple_type const &, extractors_type> (
                        t);
                }
            };

            template <class ... Types>
                struct make_view_std_tuple <true, Types ...>
            : make_view_std_tuple <false, Types ...>
            {
                typedef make_view_std_tuple <false, Types ...> base_type;
                typedef typename base_type::extractors_type extractors_type;

                using base_type::operator();
                typedef typename base_type::tuple_type tuple_type;

                member_view <tuple_type &&, extractors_type>
                    operator() (tuple_type && t) const
                {
                    return member_view <tuple_type &&, extractors_type> (
                        std::move (t));
                }
            };

        } // namespace detail

        // Enable make_view only if Directions is front, back, or a combination.
        // Specialisation for std::pair.
        template <bool Move, class First, class Second, class Directions,
                class Range>
            struct make_view <Move, heavyweight_tag <std::pair <First, Second>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions,
            detail::make_view_std_pair <Move, First, Second>> {};

        // Specialisation for std::tuple.
        template <bool Move, class ... Types, class Directions, class Range>
            struct make_view <Move, heavyweight_tag <std::tuple <Types ...>>,
                Directions, Range,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions,
            detail::make_view_std_tuple <Move, Types ...>> {};

    } // namespace operation

} // namespace range

#endif  // RANGE_STD_TUPLE_HPP_INCLUDED

