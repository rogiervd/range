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

#include <boost/mpl/pair.hpp>
#include <boost/mpl/bool.hpp>

#include "meta/vector.hpp"
#include "meta/all_of_c.hpp"
#include "meta/any_of_c.hpp"

#include "utility/overload_order.hpp"

#include "rime/core.hpp"
#include "rime/enable_if_constant.hpp"

#include "../core.hpp"

#include "tuple-fwd.hpp"

namespace range { namespace tuple_detail {

    /**
    Extract the types from a tuple_view.
    */
    template <class TupleView> struct types;

    template <std::size_t Begin, std::size_t End, class TupleReference>
        struct types <tuple_view <Begin, End, TupleReference>>
    {
        static constexpr std::size_t tuple_size
            = range::tuple_size <TupleReference>::value;

        template <class Indices> struct compute;

        template <class ... Indices> struct compute <meta::vector <Indices ...>>
        {
            typedef meta::vector <decltype (extract <
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
        template <class PredicateResults> struct result;

        template <class ... PredicateResults>
            struct result <meta::vector <PredicateResults ...>>
        : boost::mpl::eval_if <
            // If any type is known false, then false.
            meta::any_of_c <rime::equal_constant <
                rime::false_type, PredicateResults>::value ...>,
            rime::false_type,
            boost::mpl::if_ <
                // If all are known true, then true.
                meta::all_of_c <rime::equal_constant <
                    rime::true_type, PredicateResults>::value ...>,
                rime::true_type,
                // Else: runtime.
                bool>> {};

        // Implementation.

        template <class Result, class PredicateResults, class Enable = void>
            struct equal_implementation {};

        template <class PredicateResults>
            struct equal_implementation_forwarder
        : equal_implementation <
            typename result <PredicateResults>::type, PredicateResults> {};

        // Constant result.
        template <class Result, class PredicateResults>
            struct equal_implementation <Result, PredicateResults,
                typename rime::enable_if_constant <Result>::type>
        {
            template <class Left, class Right, class Predicate>
                Result operator() (Left const &, Right const &, Predicate &&)
                const
            { return Result(); }
        };

        // Empty tuples: always equal.
        template <> struct equal_implementation <bool, meta::vector<>> {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const &, Right const &, Predicate &&)
                const
            { return true; }
        };

        // Run-time result.
        template <class FirstPredicateResult, class ... PredicateResults>
            struct equal_implementation <bool,
                meta::vector <FirstPredicateResult, PredicateResults ...>>
        {
        private:
            template <class Left, class Right, class Predicate>
                bool implement (Left const & left, Right const & right,
                    Predicate && predicate, bool first_result,
                    utility::overload_order <2> *) const
            {
                return first_result
                    && equal_implementation <bool,
                        meta::vector <PredicateResults ...>>()
                            (range::drop (left), range::drop (right),
                                std::forward <Predicate> (predicate));
            }

            // If the result is know false, do not instantiate the rest of the
            // comparison.
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
            struct equal_implementation <bool,
                meta::vector <bool, bool, PredicateResults ...>>
        {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                return predicate (range::first (left), range::first (right))
                    && predicate (range::second (left), range::second (right))
                    && equal_implementation <bool,
                        meta::vector <PredicateResults ...>>()
                            (range::drop (left, rime::size_t <2>()),
                                range::drop (right, rime::size_t <2>()),
                                std::forward <Predicate> (predicate));
            }
        };

        template <class ... PredicateResults>
            struct equal_implementation <bool,
                meta::vector <bool, bool, bool, PredicateResults ...>>
        {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                return predicate (range::first (left), range::first (right))
                    && predicate (range::second (left), range::second (right))
                    && predicate (range::third (left), range::third (right))
                    && equal_implementation <bool,
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
        : equal_implementation_forwarder <
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


    /* less_lexicographical specialised for tuple. */

    namespace less_lexicographical_detail {

        /**
        A lazy compile-time range that returns the result types of applying the
        predicate, both ways around, to the types in the tuple.
        */
        template <class LeftTypes, class RightTypes, class Predicate>
            struct predicate_results;

        template <class LeftType, class RightType, class Predicate>
            struct compute_predicate_result
        {
            typedef boost::mpl::pair <
                decltype (std::declval <Predicate>() (
                    std::declval <LeftType>(), std::declval <RightType>())),
                decltype (std::declval <Predicate>() (
                    std::declval <RightType>(), std::declval <LeftType>()))
                > type;
        };

        // Left sequence is empty and right sequence is not.
        template <class RightType, class Predicate>
            struct compute_predicate_result <void, RightType, Predicate>
        { typedef boost::mpl::pair <rime::true_type, rime::false_type> type; };

        // Right sequence is empty and left sequence is not.
        template <class LeftType, class Predicate>
            struct compute_predicate_result <LeftType, void, Predicate>
        { typedef boost::mpl::pair <rime::false_type, rime::true_type> type; };

        /**
        Actually collect the result types.

        LeftTypes and RightTypes must have the same lengths.
        One of them may be padded with "void".
        */
        template <class LeftTypes, class RightTypes, class Predicate>
            struct predicate_results_implementation;

        template <class ... LeftTypes, class ... RightTypes, class Predicate>
            struct predicate_results_implementation <
                meta::vector <LeftTypes ...>, meta::vector <RightTypes ...>,
                Predicate>
        {
            typedef meta::vector <typename compute_predicate_result <
                LeftTypes, RightTypes, Predicate>::type ...,
                // Make the result for empty sequences correct.
                boost::mpl::pair <rime::false_type, rime::true_type>> type;
        };

        template <class LeftTypes, class RightTypes, class Predicate>
            struct predicate_results_left_shorter;
        template <class LeftTypes, class RightTypes, class Predicate>
            struct predicate_results_left_not_shorter;

        /**
        Return meta::vector \a Types with \c void appended as many times as
        there are elements in \a PadTypes.
        */
        template <class Types, class PadTypes> struct pad_with_void;
        template <class> struct make_void { typedef void type; };

        template <class ... Types, class ... PadTypes>
            struct pad_with_void <
                meta::vector <Types ...>, meta::vector <PadTypes ...>>
        {
            typedef meta::vector <Types ...,
                typename make_void <PadTypes>::type ...> type;
        };

        template <class LeftTypes, class RightTypes, class Predicate>
            struct predicate_results_left_shorter
        : predicate_results_implementation <
            typename pad_with_void <LeftTypes, typename
                meta::tail_of_longer <LeftTypes, RightTypes>::type>::type,
            RightTypes, Predicate> {};

        template <class LeftTypes, class RightTypes, class Predicate>
            struct predicate_results_left_not_shorter
        : predicate_results_implementation <LeftTypes,
            typename pad_with_void <RightTypes, typename
                meta::tail_of_longer <RightTypes, LeftTypes>::type>::type,
            Predicate> {};

        template <class ... LeftTypes, class ... RightTypes, class Predicate>
            struct predicate_results <meta::vector <LeftTypes ...>,
                meta::vector <RightTypes ...>, Predicate>
        : std::conditional <
            (sizeof... (LeftTypes) < sizeof... (RightTypes)),
            predicate_results_left_shorter <meta::vector <LeftTypes ...>,
                meta::vector <RightTypes ...>, Predicate>,
            predicate_results_left_not_shorter <meta::vector <LeftTypes ...>,
                meta::vector <RightTypes ...>, Predicate>
        >::type::type {};

        // result.
        template <class PredicateResults,
            class Enable1 = void, class Enable2 = void>
        struct result;

        // Both non-constant.
        template <class ... PredicateResults, class FirstLeft, class FirstRight>
            struct result <meta::vector <
                    boost::mpl::pair <FirstLeft, FirstRight>,
                    PredicateResults ...>,
                typename rime::disable_if_constant <FirstLeft>::type,
                typename rime::disable_if_constant <FirstRight>::type>
        { typedef bool type; };

        // Left true_: always true.
        template <class ... PredicateResults, class FirstLeft, class FirstRight>
            struct result <meta::vector <
                    boost::mpl::pair <FirstLeft, FirstRight>,
                    PredicateResults ...>,
                typename rime::enable_if_constant_true <FirstLeft>::type>
        { typedef rime::true_type type; };

        // Right true_: always false.
        // This should never conflict with the above, unless left < right and
        // right < left are both true, and there is no strict weak ordering.
        template <class ... PredicateResults, class FirstLeft, class FirstRight>
            struct result <meta::vector <
                    boost::mpl::pair <FirstLeft, FirstRight>,
                    PredicateResults ...>,
                typename rime::enable_if_constant_true <FirstRight>::type>
        { typedef rime::false_type type; };

        // false_, false_: both equal, so go for next.
        template <class ... PredicateResults, class FirstLeft, class FirstRight>
            struct result <meta::vector <
                    boost::mpl::pair <FirstLeft, FirstRight>,
                    PredicateResults ...>,
                typename rime::enable_if_constant_false <FirstLeft>::type,
                typename rime::enable_if_constant_false <FirstRight>::type>
        : result <meta::vector <PredicateResults ...>> {};

        // false_, bool: false or next.
        template <class ... PredicateResults, class FirstLeft, class FirstRight>
            struct result <meta::vector <
                    boost::mpl::pair <FirstLeft, FirstRight>,
                    PredicateResults ...>,
                typename rime::enable_if_constant_false <FirstLeft>::type,
                typename rime::disable_if_constant <FirstRight>::type>
        : std::conditional <
            std::is_same <typename result <
                    meta::vector <PredicateResults ...>>::type,
                rime::false_type>::value,
            rime::false_type, bool> {};

        // bool, false_: true or next.
        template <class ... PredicateResults, class FirstLeft, class FirstRight>
            struct result <meta::vector <
                    boost::mpl::pair <FirstLeft, FirstRight>,
                    PredicateResults ...>,
                typename rime::disable_if_constant <FirstLeft>::type,
                typename rime::enable_if_constant_false <FirstRight>::type>
        : std::conditional <
            std::is_same <typename result <
                    meta::vector <PredicateResults ...>>::type,
                rime::true_type>::value,
            rime::true_type, bool> {};

        // Implementation.

        template <class Result, class PredicateResults, class Enable = void>
            struct less_lexicographical_implementation {};

        template <class PredicateResults>
            struct less_lexicographical_implementation_forwarder
        : less_lexicographical_implementation <
            typename result <PredicateResults>::type, PredicateResults> {};

        // Constant result.
        template <class Result, class PredicateResults>
            struct less_lexicographical_implementation <
                Result, PredicateResults,
                typename rime::enable_if_constant <Result>::type>
        {
            template <class Left, class Right, class Predicate>
                Result operator() (Left const &, Right const &, Predicate &&)
                const
            { return Result(); }
        };

        // Run-time result.
        template <class FirstPredicateResult, class ... PredicateResults>
            struct less_lexicographical_implementation <
                bool, meta::vector <FirstPredicateResult, PredicateResults ...>>
        {
        private:
            // left < right: always return true. Don't recurse.
            template <class Left, class Right, class Predicate>
                bool implement_2 (Left const &, Right const &,
                    Predicate &&, rime::true_type, bool,
                    utility::overload_order <1> *) const
            { return true; }

            // right < left: always return false. Don't recurse.
            template <class Left, class Right, class Predicate>
                bool implement_2 (Left const &, Right const &,
                    Predicate &&, bool, rime::true_type,
                    utility::overload_order <1> *) const
            { return false; }

            // Other cases: recursion should be compiled.
            template <class Left, class Right, class Predicate>
                bool implement_2 (Left const & left, Right const & right,
                    Predicate && predicate, bool left_right, bool right_left,
                    utility::overload_order <2> *) const
            {
                if (left_right)
                    return true;
                if (right_left)
                    return false;
                return less_lexicographical_implementation <bool,
                    meta::vector <PredicateResults ...>>()
                        (range::drop (left), range::drop (right),
                            std::forward <Predicate> (predicate));
            }

            // Check whether the tuples are empty.
            template <class Left, class Right, class Predicate>
                bool implement (Left const & left, Right const & right,
                    Predicate && predicate, rime::false_type, rime::false_type)
                const
            {
                return implement_2 (left, right, predicate,
                    predicate (range::first (left), range::first (right)),
                    predicate (range::first (right), range::first (left)),
                    utility::pick_overload());
            }

            template <class Left, class Right, class Predicate>
                bool implement (Left const &, Right const &, Predicate &&,
                    rime::true_type, rime::false_type) const
            { return true; }

            template <class Left, class Right, class Predicate>
                bool implement (Left const &, Right const &, Predicate &&,
                    bool, rime::true_type) const
            { return false; }

        public:
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                return implement (left, right, predicate,
                    range::empty (left), range::empty (right));
            }
        };

        template <class ... PredicateResults>
            struct less_lexicographical_implementation <
                bool, meta::vector <boost::mpl::pair <bool, bool>,
                    PredicateResults ...>>
        {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                if (predicate (range::first (left), range::first (right)))
                    return true;
                if (predicate (range::first (right), range::first (left)))
                    return false;
                return less_lexicographical_implementation <bool,
                    meta::vector <PredicateResults ...>>()
                        (range::drop (left), range::drop (right),
                            std::forward <Predicate> (predicate));
            }
        };

        template <class ... PredicateResults>
            struct less_lexicographical_implementation <
                bool, meta::vector <boost::mpl::pair <bool, bool>,
                    boost::mpl::pair <bool, bool>, PredicateResults ...>>
        {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                if (predicate (range::first (left), range::first (right)))
                    return true;
                if (predicate (range::first (right), range::first (left)))
                    return false;
                if (predicate (range::second (left), range::second (right)))
                    return true;
                if (predicate (range::second (right), range::second (left)))
                    return false;
                return less_lexicographical_implementation <bool,
                    meta::vector <PredicateResults ...>>()
                        (range::drop (left, rime::size_t <2>()),
                            range::drop (right, rime::size_t <2>()),
                            std::forward <Predicate> (predicate));
            }
        };

        template <class ... PredicateResults>
            struct less_lexicographical_implementation <
                bool, meta::vector <boost::mpl::pair <bool, bool>,
                    boost::mpl::pair <bool, bool>,
                    boost::mpl::pair <bool, bool>, PredicateResults ...>>
        {
            template <class Left, class Right, class Predicate>
                bool operator() (Left const & left, Right const & right,
                    Predicate && predicate) const
            {
                if (predicate (range::first (left), range::first (right)))
                    return true;
                if (predicate (range::first (right), range::first (left)))
                    return false;
                if (predicate (range::second (left), range::second (right)))
                    return true;
                if (predicate (range::second (right), range::second (left)))
                    return false;
                if (predicate (range::third (left), range::third (right)))
                    return true;
                if (predicate (range::third (right), range::third (left)))
                    return false;
                return less_lexicographical_implementation <bool,
                    meta::vector <PredicateResults ...>>()
                        (range::drop (left, rime::size_t <3>()),
                            range::drop (right, rime::size_t <3>()),
                            std::forward <Predicate> (predicate));
            }
        };

        // less_lexicographical.
        template <class LeftTypes, class RightTypes, class Predicate,
                class Enable = void>
            struct less_lexicographical;

        template <class ... LeftTypes, class ... RightTypes, class Predicate>
            struct less_lexicographical <
                meta::vector <LeftTypes ...>, meta::vector <RightTypes ...>,
                Predicate>
        : less_lexicographical_implementation_forwarder <
            typename less_lexicographical_detail::predicate_results <
                meta::vector <LeftTypes ...>, meta::vector <RightTypes ...>,
                Predicate>::type>
        {};

    } // namespace less_lexicographical_detail

    template <std::size_t Size, class LeftView, class RightView,
        class Predicate>
    inline auto implement_less_lexicographical (
        tuple_view_tag <Size>, tuple_view_tag <Size>,
        LeftView const & left, RightView const & right,
        direction::front, Predicate && predicate)
    RETURNS (less_lexicographical_detail::less_lexicographical <typename
        types <LeftView>::type, typename types <RightView>::type, Predicate>()
            (left, right, std::forward <Predicate> (predicate)));

}} // namespace range::tuple_detail

#endif  // RANGE_DETAIL_TUPLE_COMPARE_HPP_INCLUDED
