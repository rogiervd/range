/*
Copyright 2011, 2012, 2013 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_VIEW_HPP_INCLUDED
#define RANGE_DETAIL_CORE_VIEW_HPP_INCLUDED

#include <type_traits>

#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/placeholders.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/all.hpp"
#include "meta/transform.hpp"

#include "rime/core.hpp"

#include "../direction.hpp"

namespace range {

namespace operation {

    template <class RangeTag, class Directions, class Enable = void>
        struct make_view;

    template <class RangeTag, class Directions, class Enable = void>
        struct view;

    /**
    Produce a lightweight range, a "view", on a heavyweight range such as a
    container.

    Directions is a meta::vector<> of at least one direction.
    The resulting type must be assignable without changing any underlying
    container.
    If possible, the resulting type should be homogeneous.
    make_view must not be defined for the resulting type.
    If the range is already a lightweight range, then make_view should not be
    defined for it.
    view (directions..., range) will then return range unchanged.

    Directions is a meta::vector<> of decayed types.
    The range is forwarded as is.
    */
    template <class RangeTag, class Directions, class Enable>
        struct make_view : unimplemented {};

    namespace detail {
        /**
        Default implementation for "view".
        If the directions are all valid for the range (the operationalisation of
        which is: if empty (direction, range) is defined for all directions),
        then the range is returned unchanged.
        */
        template <class RangeTag, class Directions, class Enable = void>
            struct passthrough_view : unimplemented {};

        template <class RangeTag, class Directions>
            struct passthrough_view <RangeTag, Directions,
                typename boost::enable_if <
                    meta::all <meta::transform <boost::mpl::and_ <
                        is_direction <boost::mpl::_1>,
                        is_implemented <operation::empty <
                            RangeTag, boost::mpl::_1>>>,
                    Directions>>>::type>
        : helper::call_with_last <1,
            Directions, helper::return_rvalue_reference>
        {};
    } // namespace detail

    template <class RangeTag, class Directions, class Enable> struct view
    : boost::mpl::if_ <
        is_implemented <make_view <RangeTag, Directions>>,
        make_view <RangeTag, Directions>,
        detail::passthrough_view <RangeTag, Directions>>::type {};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct view;
} // namespace apply

namespace callable {
    struct view : generic <apply::view> {};
} // namespace callable

static const auto view = callable::view();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct view : operation::unimplemented {};

        template <class Directions, class Range>
            struct view <Directions, meta::vector<>, meta::vector <Range>>
        : operation::view <typename range::tag_of <Range>::type, Directions> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct view
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::view, meta::vector <Arguments ...>>::type {};

} // namespace apply

/* is_view */

/**
Metafunction that returns true iff Range is a view.
That is, view (directions..., range) returns range itself.

The last argument is Range.
The arguments before it form Directions.
If only one argument is given, it is Range, and its default direction is used.
*/
template <class ... Arguments> struct is_view
: rime::as_rime_constant <typename std::is_same <
        typename std::decay <
            // Last argument is the range.
            typename meta::first <meta::back, meta::vector <Arguments...>>::type
            >::type,
        typename decayed_result_of <callable::view (Arguments...)>::type
    >::type>::type {};


namespace apply {

    namespace automatic_arguments {

        /* apply_with_view. */

        template <class Directions, class Range> struct is_view_in;
        template <class ... Directions, class Range>
            struct is_view_in <meta::vector <Directions ...>, Range>
        : is_view <Directions ..., Range> {};

        /**
        Contain a type "apply", that when called, calls Apply with the
        arguments given.
        If the ranges (the third argument) are all views in the directions (the
        first argument), then "apply" will derive from Apply.
        If not all ranges are views, they are first converted to views, and then
        the arguments are forwarded to Apply.
        */
        template <template <class, class, class, class> class Apply>
            struct call_with_view
        {
            template <class Directions, class Other, class Ranges,
                class Enable = void>
            struct apply : operation::unimplemented {};

            // If all ranges are views.
            template <class Directions, class Others, class ... Ranges>
                struct apply <Directions, Others, meta::vector <Ranges ...>,
                    typename boost::enable_if <
                        meta::all <meta::vector <
                            is_view_in <Directions, Ranges> ...>>>::type>
            : Apply <Directions, Others, meta::vector <Ranges ...>, void> {};

            /*
            This implementation for the case where not all ranges are views
            should work, but does not on any compiler I have tried because they
            get confused about multiple variable arguments.
            Below, therefore, there are specialisations for different numbers
            of arguments for Directions and Others.
            */
            /*
            template <class ... Directions, class ... Others, class ... Ranges>
                struct apply <meta::vector <Directions ...>,
                    meta::vector <Others ...>, meta::vector <Ranges ...>,
                    typename boost::disable_if <
                        meta::all <meta::vector <
                            is_view_in <meta::vector <Directions ...>,
                            Ranges> ...>>>::type>
            {
                Apply <meta::vector <Directions ...>, meta::vector <Others ...>,
                    meta::vector <typename result_of <
                        callable::view (Directions ..., Ranges)>::type...>,
                    void> underlying;

                auto operator() (Directions const & ... directions,
                    Others && ... others,
                    Ranges && ... ranges) const
                RETURNS (underlying (directions ...,
                    std::forward <Others> (others) ...,
                    range::view (directions ...,
                        std::forward <Ranges> (ranges)) ...))
            };*/

#define RANGE_CORE_BASE_class_Direction(z, n, data) \
                class Direction ## n ,
#define RANGE_CORE_BASE_class_Other(z, n, data) \
                class Other ## n ,

#define RANGE_CORE_BASE_Direction_const_direction(z, n, data) \
                Direction ## n const & direction ## n ,
#define RANGE_CORE_BASE_Other_other(z, n, data) \
                Other ## n && other ## n ,

#define RANGE_CORE_BASE_Direction(z, n, data) Direction ## n ,
#define RANGE_CORE_BASE_direction(z, n, data) direction ## n ,
#define RANGE_CORE_BASE_std_forward_other(z, n, data) \
                std::forward <Other ## n > ( other ## n ),

#define RANGE_CORE_BASE_apply(z, direction_num, others_num) \
            template < \
                BOOST_PP_REPEAT (direction_num, \
                    RANGE_CORE_BASE_class_Direction,) \
                BOOST_PP_REPEAT (others_num, RANGE_CORE_BASE_class_Other,) \
                class ... Ranges> \
            struct apply < \
                meta::vector <  \
                    BOOST_PP_ENUM_PARAMS (direction_num, Direction)>, \
                meta::vector <BOOST_PP_ENUM_PARAMS (others_num, Other)>, \
                meta::vector <Ranges ...>, \
                typename boost::disable_if < \
                    meta::all <meta::vector < \
                        is_view_in <meta::vector < \
                        BOOST_PP_ENUM_PARAMS (direction_num, Direction)>, \
                        Ranges> ...>>>::type> \
            { \
                Apply < \
                    meta::vector < \
                        BOOST_PP_ENUM_PARAMS (direction_num, Direction)>, \
                    meta::vector <BOOST_PP_ENUM_PARAMS (others_num, Other)>, \
                    meta::vector <typename result_of < callable::view ( \
                        BOOST_PP_REPEAT (direction_num, \
                            RANGE_CORE_BASE_Direction,) \
                        Ranges)> \
                    ::type...>, void> underlying; \
            \
                auto operator() ( \
                    BOOST_PP_REPEAT (direction_num, \
                        RANGE_CORE_BASE_Direction_const_direction,) \
                    BOOST_PP_REPEAT (others_num, RANGE_CORE_BASE_Other_other,) \
                    Ranges && ... ranges) const \
                RETURNS (underlying ( \
                    BOOST_PP_REPEAT (direction_num, \
                        RANGE_CORE_BASE_direction,) \
                    BOOST_PP_REPEAT (others_num, \
                        RANGE_CORE_BASE_std_forward_other,) \
                    range::view ( \
                        BOOST_PP_REPEAT (direction_num, \
                            RANGE_CORE_BASE_direction,) \
                        std::forward <Ranges> (ranges)) ...)) \
            };

            // RANGE_CORE_BASE_apply(z, 0..5, n)
#define RANGE_CORE_BASE_apply_direction_num(z, n, data) \
            BOOST_PP_REPEAT (5, RANGE_CORE_BASE_apply, n)

            // RANGE_CORE_BASE_apply_direction_num (z, 0..5, data)
            // i.e. RANGE_CORE_BASE_apply(z, 0..5, 0..5)
            BOOST_PP_REPEAT (5, RANGE_CORE_BASE_apply_direction_num,)

#undef RANGE_CORE_BASE_class_Direction
#undef RANGE_CORE_BASE_class_Other
#undef RANGE_CORE_BASE_Direction_const_direction
#undef RANGE_CORE_BASE_Other_other
#undef RANGE_CORE_BASE_Direction
#undef RANGE_CORE_BASE_direction
#undef RANGE_CORE_BASE_std_forward_other
#undef RANGE_CORE_BASE_apply
#undef RANGE_CORE_BASE_apply_direction_num

        };

    } // namespace automatic_arguments

} // namespace apply


} // namespace range

#endif  // RANGE_DETAIL_CORE_VIEW_HPP_INCLUDED

