/*
Copyright 2011-2015 Rogier van Dalen.

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

        /**
        Categorise arguments into directions, other, and ranges.
        "type" is set to Apply <meta::vector <Directions ...>,
        meta::vector <Other...>, meta::vector <Ranges ...>, void>.
        The types in Directions are decayed.
        */
        template <class Arguments> struct categorise_arguments {
            typedef typename detail::split <
                    meta::front, is_direction <boost::mpl::_>, Arguments>::type
                directions_rest;

            typedef typename meta::as_vector <meta::transform <
                std::decay <boost::mpl::_>,
                typename directions_rest::first>>::type directions;

            typedef typename detail::split <
                    meta::back, is_range <boost::mpl::_>,
                    typename directions_rest::second
                >::type rest_ranges;

            // Here first and second are counted from the back!
            typedef typename rest_ranges::second other;
            typedef typename rest_ranges::first ranges;

            typedef meta::vector <directions, other, ranges> type;
        };

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
