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

namespace range {

    // Tags.

    namespace std_tuple_operation {

        struct std_pair_tag : heavyweight::heavyweight_tag {};
        struct std_tuple_tag : heavyweight::heavyweight_tag {};

    } // namespace std_tuple_operation

    template <class First, class Second>
        struct tag_of_qualified <std::pair <First, Second>>
    { typedef std_tuple_operation::std_pair_tag type; };

    template <class ... Types>
        struct tag_of_qualified <std::tuple <Types ...>>
    { typedef std_tuple_operation::std_tuple_tag type; };

    namespace std_tuple_operation {

        template <class QPair> struct std_pair_member_view {
            typedef typename std::decay <QPair>::type pair;
            typedef meta::vector <
                    member_extractor <
                        typename pair::first_type pair::*, &pair::first>,
                    member_extractor <
                        typename pair::second_type pair::*, &pair::second>>
                extractors_type;

            typedef member_view <QPair, extractors_type> type;
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
            template <class ... Types, class Result =
                typename std::tuple_element <Index::value,
                    std::tuple <Types ...>>::type &&>
            Result operator() (std::tuple <Types ...> && tuple) const
            { return static_cast <Result> (std::get <Index::value> (tuple)); }
        };

        template <class QTuple, class ... Types> struct std_tuple_member_view {
            // Create meta::vector <
            //     std_tuple_extractor <rime::size_t<0>>, ...>.
            typedef typename meta::as_vector <meta::transform <
                std_tuple_extractor <boost::mpl::first <boost::mpl::_>>,
                typename meta::enumerate <meta::vector <Types...>>::type
                >>::type extractors_type;

            typedef member_view <QTuple, extractors_type> type;
        };

        // implement_make_view for std::pair.
        // Const reference.
        template <class First, class Second,
            class Result = typename std_tuple_operation::std_pair_member_view <
                std::pair <First, Second> const &>::type>
        inline Result implement_make_view (std_tuple_operation::std_pair_tag,
            bool once, std::pair <First, Second> const & pair,
            helper::front_or_back,
            helper::front_or_back = helper::front_or_back())
        { return Result (pair); }

        // Reference.
        template <class First, class Second,
            class Result = typename std_tuple_operation::std_pair_member_view <
                std::pair <First, Second> &>::type>
        inline Result implement_make_view (std_tuple_operation::std_pair_tag,
            bool once, std::pair <First, Second> & pair,
            helper::front_or_back,
            helper::front_or_back = helper::front_or_back())
        { return Result (pair); }

        // Rvalue reference.
        template <class First, class Second,
            class Result = typename std_tuple_operation::std_pair_member_view <
                std::pair <First, Second> &&>::type>
        inline Result implement_make_view (std_tuple_operation::std_pair_tag,
            rime::true_type once, std::pair <First, Second> && pair,
            helper::front_or_back,
            helper::front_or_back = helper::front_or_back())
        { return Result (std::move (pair)); }


        // implement_make_view for std::tuple.
        // Const reference.
        template <class ... Types,
            class Result = typename std_tuple_operation::std_tuple_member_view <
                std::tuple <Types ...> const &, Types ...>::type>
        inline Result implement_make_view (std_tuple_operation::std_tuple_tag,
            bool once, std::tuple <Types ...> const & tuple,
            helper::front_or_back,
            helper::front_or_back = helper::front_or_back())
        { return Result (tuple); }

        // Reference.
        template <class ... Types,
            class Result = typename std_tuple_operation::std_tuple_member_view <
                std::tuple <Types ...> &, Types ...>::type>
        inline Result implement_make_view (std_tuple_operation::std_tuple_tag,
            bool once, std::tuple <Types ...> & tuple,
            helper::front_or_back,
            helper::front_or_back = helper::front_or_back())
        { return Result (tuple); }

        // Rvalue reference.
        template <class ... Types,
            class Result = typename std_tuple_operation::std_tuple_member_view <
                std::tuple <Types ...> &&, Types ...>::type>
        inline Result implement_make_view (std_tuple_operation::std_tuple_tag,
            rime::true_type once, std::tuple <Types ...> && tuple,
            helper::front_or_back,
            helper::front_or_back = helper::front_or_back())
        { return Result (std::move (tuple)); }

    } // namespace std_tuple_operation

} // namespace range

#endif // RANGE_STD_TUPLE_HPP_INCLUDED
