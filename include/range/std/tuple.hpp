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
        struct tag_of_bare <std::pair <First, Second>>
    { typedef heavyweight_tag <std::pair <First, Second>> type; };

    template <class ... Types>
        struct tag_of_bare <std::tuple <Types ...>>
    { typedef heavyweight_tag <std::tuple <Types ...>> type; };

    namespace operation {

        namespace detail {

            /**
            Implementation of make_view for std::pair.
            Uses member_view with the "first" and "second" member variables.
            */
            template <class First, class Second> struct make_view_std_pair {
                typedef std::pair <First, Second> pair;
                typedef meta::vector <
                        member_extractor <First pair::*, &pair::first>,
                        member_extractor <Second pair::*, &pair::second>>
                    extractors_type;

                member_view <pair, extractors_type> operator() (pair & p) const
                { return member_view <pair, extractors_type> (p); }

                member_view <pair const, extractors_type>
                    operator() (pair const & p) const
                { return member_view <pair const, extractors_type> (p); }
            };

            // For std::tuple, a custom extractor is useful.
            template <class Index> struct std_tuple_extractor {
                template <class Tuple> auto operator() (Tuple && tuple) const
                RETURNS (std::get <Index::value> (std::forward <Tuple> (tuple)))
            };

            /**
            Implementation of make_view for std::tuple.
            This is essentially a generalisation of make_view_std_pair.
            */
            template <class ... Types> struct make_view_std_tuple {
                // Create meta::vector <
                //     std_tuple_extractors <rime::size_t<0>>, ...>.
                typedef typename meta::as_vector <meta::transform <
                    std_tuple_extractor <boost::mpl::first <boost::mpl::_>>,
                    typename meta::enumerate <meta::vector <Types...>>::type
                    >>::type extractors_type;

                member_view <std::tuple <Types ...>, extractors_type>
                    operator() (std::tuple <Types ...> & t) const
                {
                    return member_view <std::tuple <Types ...>,
                        extractors_type> (t);
                }

                member_view <std::tuple <Types ...> const, extractors_type>
                    operator() (std::tuple <Types ...> const & t) const
                {
                    return member_view <
                        std::tuple <Types ...> const, extractors_type> (t);
                }
            };

        } // namespace detail

        // Enable make_view only if Directions is front, back, or a combination.
        // Specialisation for std::pair.
        template <class First, class Second, class Directions>
            struct make_view <heavyweight_tag <std::pair <First, Second>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions,
            detail::make_view_std_pair <First, Second>> {};

        // Specialisation for std::tuple.
        template <class ... Types, class Directions>
            struct make_view <heavyweight_tag <std::tuple <Types ...>>,
                Directions,
                typename detail::enable_if_front_back <Directions>::type>
        : helper::call_with_last <1, Directions,
            detail::make_view_std_tuple <Types ...>> {};

    } // namespace operation

} // namespace range

#endif  // RANGE_STD_TUPLE_HPP_INCLUDED

