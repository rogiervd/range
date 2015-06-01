/*
Copyright 2011-2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_AUTOMATIC_ARGUMENTS_HPP_INCLUDED
#define RANGE_DETAIL_CORE_AUTOMATIC_ARGUMENTS_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/placeholders.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/transform.hpp"

#include "utility/returns.hpp"
#include "utility/nested_callable.hpp"

#include "callable_traits.hpp"
#include "../direction.hpp"

#include "meta_split.hpp"

#include "core_tag.hpp"
#include "core_base.hpp"
#include "core_default_direction.hpp"

namespace range { namespace apply {

    /**
    For most operations, there is some automatic filling in of arguments
    to be done.
    They take zero or one (or sometimes more) directions, some variable
    parameters, and one (or sometimes more) ranges.
    If no direction is given, default_direction (range) is usually used.
    The variable parameters can sometimes be filled in automatically too.
    This namespace deals with that.
    */
    namespace automatic_arguments {

        namespace categorise_arguments_detail {

            template <bool ... Values> struct bools;

            /**
            Categorise arguments into directions, other, and ranges.
            General implementation.
            */
            template <class Arguments> struct categorise_general;

            /**
            Categorise arguments into directions, other, and ranges.
            Specialised for a few, extremely common, cases.
            \tparam Arguments meta::vector of arguments.
            \tparam AreDirections \c bools of \c direction::is_direction applied
                to each of the arguments.
            \tparam AreRanges \c bools of \c is_range applied to each of the
                arguments.
            */
            template <class Arguments, class AreDirections, class AreRanges>
                struct categorise_specific
            : categorise_general <Arguments> {};


            template <class Arguments> struct categorise_general {
                typedef typename detail::split <
                        meta::front, is_direction <boost::mpl::_>, Arguments
                    >::type directions_rest;

                typedef typename directions_rest::first directions;

                typedef typename detail::split <
                        meta::back, is_range <boost::mpl::_>,
                        typename directions_rest::second
                    >::type rest_ranges;

                // Here first and second are counted from the back!
                typedef typename rest_ranges::second other;
                typedef typename rest_ranges::first ranges;

                typedef meta::vector <directions, other, ranges> type;
            };

            /* The few common cases that it is worth optimising for. */

            // Range.
            template <class Argument1>
                struct categorise_specific <meta::vector <Argument1>,
                    bools <false>, bools <true>>
            {
                typedef meta::vector <meta::vector<>, meta::vector<>,
                    meta::vector <Argument1>> type;
            };

            // Range, range.
            template <class Argument1, class Argument2>
                struct categorise_specific <meta::vector <Argument1, Argument2>,
                    bools <false, false>, bools <true, true>>
            {
                typedef meta::vector <meta::vector<>, meta::vector<>,
                    meta::vector <Argument1, Argument2>> type;
            };

            // Direction, range.
            template <class Argument1, class Argument2>
                struct categorise_specific <meta::vector <Argument1, Argument2>,
                    bools <true, false>, bools <false, true>>
            {
                typedef meta::vector <meta::vector <Argument1>, meta::vector<>,
                    meta::vector <Argument2>> type;
            };

            // Other, range.
            template <class Argument1, class Argument2>
                struct categorise_specific <meta::vector <Argument1, Argument2>,
                    bools <false, false>, bools <false, true>>
            {
                typedef meta::vector <meta::vector<>, meta::vector <Argument1>,
                    meta::vector <Argument2>> type;
            };

            // Direction, other, range.
            template <class Argument1, class Argument2, class Argument3>
                struct categorise_specific <
                    meta::vector <Argument1, Argument2, Argument3>,
                    bools <true, false, false>,
                    bools <false, false, true>>
            {
                typedef meta::vector <meta::vector <Argument1>,
                    meta::vector <Argument2>, meta::vector <Argument3>> type;
            };

            // Other, range, range.
            template <class Argument1, class Argument2, class Argument3>
                struct categorise_specific <
                    meta::vector <Argument1, Argument2, Argument3>,
                    bools <false, false, false>,
                    bools <false, true, true>>
            {
                typedef meta::vector <meta::vector <>, meta::vector <Argument1>,
                    meta::vector <Argument2, Argument3>> type;
            };

        } // namespace categorise_arguments_detail

        /**
        Categorise arguments into directions, other, and ranges.
        "type" is set to Apply <meta::vector <Directions ...>,
        meta::vector <Other...>, meta::vector <Ranges ...>, void>.
        */
        template <class Arguments> struct categorise_arguments;

        template <class ... Arguments>
            struct categorise_arguments <meta::vector <Arguments ...>>
        : categorise_arguments_detail::categorise_specific <
            meta::vector <Arguments ...>,
            categorise_arguments_detail::bools <
                direction::is_direction <Arguments>::value ...>,
            categorise_arguments_detail::bools <
                is_range <Arguments>::value ...>> {};

        /**
        Categorise Arguments into Directions, Other, and Ranges.
        Return Apply <Directions, Other, Ranges>.
        Except when Directions is empty.
        Then it takes the default direction of the first range and passes
        that as the first argument to
        Apply <meta::vector <direction>, Other, Ranges>.
        */
        template <template <class, class, class, class> class Apply,
            class Arguments, class Categorised =
                typename categorise_arguments <Arguments>::type>
        struct categorise_arguments_default_direction;

        // At least one direction.
        template <template <class, class, class, class> class Apply,
            class Arguments, class ... Directions, class Other, class Ranges>
        struct categorise_arguments_default_direction <Apply, Arguments,
            meta::vector <meta::vector <Directions ...>, Other, Ranges>>
        {
            typedef Apply <meta::vector <Directions ...>, Other, Ranges, void>
                type;
        };

        /**
        Actually call the implementation with the default direction.
        */
        template <class Implementation, class Other, class Ranges>
            struct prepend_default_direction;

        // No direction.
        template <template <class, class, class, class> class Apply,
            class Arguments, class Other, class Ranges>
        struct categorise_arguments_default_direction <Apply, Arguments,
            meta::vector <meta::vector<>, Other, Ranges>>
        {
            typedef typename meta::first <Ranges>::type range_type;

            typedef typename std::decay <typename result_of_or <
                callable::default_direction (range_type), void>::type>::type
                direction;
            typedef Apply <meta::vector <direction>, Other, Ranges, void>
                implementation;
            typedef typename boost::mpl::if_ <boost::mpl::and_ <
                        has <callable::default_direction (range_type)>,
                        operation::is_implemented <implementation>>,
                    prepend_default_direction <implementation, Other, Ranges>,
                    operation::unimplemented
                >::type type;
        };

        // No direction but no range either: a bit silly.
        // Needs disabling explicitly.
        template <template <class, class, class, class> class Apply,
            class Arguments, class Other>
        struct categorise_arguments_default_direction <Apply, Arguments,
            meta::vector <meta::vector<>, Other, meta::vector <>>>
        : operation::unimplemented {};

        /**
        \return The default direction of the first range.
        */
        template <class Range, class ... MoreRanges> inline auto
            first_default_direction (
                Range const & range, MoreRanges const & ...)
        RETURNS (::range::default_direction (range));

        // Explicit specialisations for different numbers of "Other" to keep
        // CLang 3.0 happy.
        template <class Implementation, class ... Ranges>
            struct prepend_default_direction <Implementation,
                meta::vector<>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Ranges> (ranges) ...));
        };

        template <class Implementation, class Other, class ... Ranges>
            struct prepend_default_direction <Implementation,
                meta::vector <Other>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Other && other, Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Other> (other),
                std::forward <Ranges> (ranges) ...));
        };

        template <class Implementation, class Other1, class Other2,
                class ... Ranges>
            struct prepend_default_direction <Implementation,
                meta::vector <Other1, Other2>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Other1 && other_1, Other2 && other_2,
                Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Other1> (other_1),
                std::forward <Other2> (other_2),
                std::forward <Ranges> (ranges) ...));
        };

        template <class Implementation, class Other1, class Other2,
            class Other3, class ... Ranges>
        struct prepend_default_direction <Implementation,
            meta::vector <Other1, Other2, Other3>, meta::vector <Ranges ...>>
        {
            Implementation implementation;

            auto operator() (Other1 && other_1, Other2 && other_2,
                Other3 && other_3, Ranges && ... ranges) const
            RETURNS (implementation (first_default_direction (ranges ...),
                std::forward <Other1> (other_1),
                std::forward <Other2> (other_2),
                std::forward <Other3> (other_3),
                std::forward <Ranges> (ranges) ...));
        };

    } // namespace automatic_arguments

}} // namespace range::apply

#endif  // RANGE_DETAIL_CORE_AUTOMATIC_ARGUMENTS_HPP_INCLUDED
