/*
Copyright 2015 Rogier van Dalen.

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
Define optimised versions of "equal" and "less_lexicographical" for tuples.
*/

#ifndef RANGE_DETAIL_TUPLE_COMPARE_HPP_INCLUDED
#define RANGE_DETAIL_TUPLE_COMPARE_HPP_INCLUDED

#include <type_traits>

#include "utility/overload_order.hpp"

#include "rime/core.hpp"
#include "rime/enable_if_constant.hpp"

#include "../core.hpp"

#include "tuple-fwd.hpp"

namespace range {

namespace tuple_detail {

    template <class TupleView> struct types;

    template <std::size_t Begin, std::size_t End, class TupleReference>
        struct types <class tuple_view <Begin, End, TupleReference>>
    {
        static constexpr std::size_t tuple_size
            = range::tuple_size <TupleReference>::value;

        template <class Indices> struct compute;

        template <class ... Indices> struct compute <meta::vector <Indices ...>>
        {
            typedef meta::vector <
                decltype (extract <
                ((tuple_size - Begin - 1) - Indices::value)>() (
                    std::declval <TupleReference>())) ...> type;
        };

        typedef typename compute <typename meta::count <End - Begin>::type
            >::type type;
    };

    /* equal specialised for tuple. */

    // Different sizes: never equal.
    template <std::size_t LeftSize, std::size_t RightSize,
        class LeftView, class RightView, class Predicate>
    inline rime::false_type equal (
        tuple_view_tag <LeftSize>, tuple_view_tag <RightSize>,
        LeftView const &, RightView const &, direction::front, Predicate &&)
    { return rime::false_; }

    namespace equal_detail {

        // result.
        template <class PredicateResults, class Enable = void> struct result;

        template <> struct result <meta::vector<>>
        { typedef rime::true_type type; };

        template <class First, class ... Rest>
            struct result <meta::vector <First, Rest ...>,
                typename std::enable_if <!rime::is_constant <First>::value
                    >::type>
        { typedef bool type; };

        template <class First, class ... Rest>
            struct result <meta::vector <First, Rest ...>,
                typename rime::enable_if_constant_true <First>::type>
        : result <meta::vector <Rest ...>> {};

        template <class First, class ... Rest>
            struct result <meta::vector <First, Rest ...>,
                typename rime::enable_if_constant_false <First>::type>
        { typedef rime::false_type type; };

        // implementation.

        template <class PredicateResults,
            class Result = typename result <PredicateResults>::type,
            class Enable = void>
        struct equal_implementation {};

        // Constant result.
        template <class PredicateResults, class Result>
            struct equal_implementation <PredicateResults, Result,
                typename std::enable_if <rime::is_constant <Result>::value
                    >::type>
        {
            template <class Left, class Right, class Predicate>
                Result operator() (Left const &, Right const &, Predicate &&)
                const
            { return Result(); }
        };

        // Run-time result.
        template <class FirstPredicateResult, class ... PredicateResults>
            struct equal_implementation <
                meta::vector <FirstPredicateResult, PredicateResults ...>, bool>
        {
        private:
            template <class Left, class Right, class Predicate>
                bool implement (Left const & left, Right const & right,
                    Predicate && predicate, bool first_result,
                    utility::overload_order <2> *) const
            {
                return first_result
                    && equal_implementation <
                        meta::vector <PredicateResults ...>>()
                            (range::drop (left), range::drop (right),
                                std::forward <Predicate> (predicate));
            }

            template <class Left, class Right, class Predicate>
                bool implement (Left const &, Right const &,
                    Predicate &&, rime::false_type,
                    utility::overload_order <1> *) const
            { return false; }

        public:
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                return implement (left, right, predicate,
                    predicate (range::first (left), range::first (right)),
                    utility::pick_overload());
            }
        };

        // Common scenario: comparisons return bool.
        template <class ... PredicateResults>
            struct equal_implementation <
                meta::vector <bool, bool, PredicateResults ...>, bool>
        {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                return predicate (range::first (left), range::first (right))
                    && predicate (range::second (left), range::second (right))
                    && equal_implementation <
                        meta::vector <PredicateResults ...>>()
                            (range::drop (left, rime::size_t <2>()),
                                range::drop (right, rime::size_t <2>()),
                                std::forward <Predicate> (predicate));
            }
        };

        template <class ... PredicateResults>
            struct equal_implementation <
                meta::vector <bool, bool, bool, PredicateResults ...>, bool>
        {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                return predicate (range::first (left), range::first (right))
                    && predicate (range::second (left), range::second (right))
                    && predicate (range::third (left), range::third (right))
                    && equal_implementation <
                        meta::vector <PredicateResults ...>>()
                            (range::drop (left, rime::size_t <3>()),
                                range::drop (right, rime::size_t <3>()),
                                std::forward <Predicate> (predicate));
            }
        };

        // equal.
        template <class LeftTypes, class RightTypes, class Predicate,
                class Enable = void>
            struct equal;

        template <class ... LeftTypes, class ... RightTypes, class Predicate>
            struct equal <
                meta::vector <LeftTypes ...>, meta::vector <RightTypes ...>,
                Predicate, typename std::enable_if <
                    sizeof... (LeftTypes) == sizeof... (RightTypes)>::type>
        : equal_implementation <
            meta::vector <decltype (std::declval <Predicate>() (
                std::declval <LeftTypes>(), std::declval <RightTypes>())) ...>>
        {};

    } // namespace equal_detail

    template <std::size_t Size, class LeftView, class RightView,
        class Predicate>
    inline auto implement_equal (tuple_view_tag <Size>, tuple_view_tag <Size>,
        LeftView const & left, RightView const & right,
        direction::front, Predicate && predicate)
    RETURNS (equal_detail::equal <typename
        types <LeftView>::type, typename types <RightView>::type, Predicate>()
            (left, right, std::forward <Predicate> (predicate)));

} // namespace tuple_detail

} // namespace range

#endif  // RANGE_DETAIL_TUPLE_COMPARE_HPP_INCLUDED
