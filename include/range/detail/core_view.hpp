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

    If \a Move is true, then view_once was called, and each element in the view
    can be assumed to be seen only once.
    Therefore, if a container is passed in that owns the elements, they can be
    viewed as rvalue references.

    It makes sense to specialise this first for <c>Move=true</c> and providing a
    view that does not move the elements.
    Then, an additional specialisation for <c>Move=true</c> can be added.
    This will usually do the same as when <c>Move=false</c>, except for adding
    an overload that receives an rvalue reference.
    It can therefore derive from the specialisation with <c>Move=false</c> and
    say <c>using make_view \<false, ...>::operator();</c>.
    The additional overload can then be added.
    */
    template <bool Move, class RangeTag, class Directions, class Enable = void>
        struct make_view
    : std::conditional <Move,
        make_view <false, RangeTag, Directions>, unimplemented>::type {};

    /**
    Call make_view, or, if that is not defined, return the range as is.
    Do not specialise this; merely specialise make_view for heavyweight ranges.
    */
    template <class RangeTag, class Directions, class Enable = void>
        struct view;

    /**
    Produce a lightweight range on a heavyweight range, assuming each element
    will be consumed only once.
    The default definition of this forwards to normal "view", and this often
    provides the correct behaviour.

    However, instantiate this if the view of the heavyweight range should be
    different if an rvalue reference is passed in.
    If the range is the sole owner of the elements, the desired behaviour of the
    view can be for first() to return the elements by rvalue reference instead
    of normal reference.
    Usually, a specialisation can derive from view <RangeTag, Directions>,
    import its \c operator() with <c>using operator();</c>, and provide an
    additional overload for rvalue references.

    Not that even when an rvalue is passed in, the view should not contain a
    copy of the heavyweight range.
    The view should still assume that the reference remains usable as long as
    the view is used.

    Only specialise this if make_view is also defined, otherwise "is_view" is
    not defined sensibly.
    */
    template <class RangeTag, class Directions, class Enable = void>
        struct view_once;

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
        // First argument ("Move") to make_view is "false".
        is_implemented <make_view <false, RangeTag, Directions>>,
        make_view <false, RangeTag, Directions>,
        detail::passthrough_view <RangeTag, Directions>>::type {};

    template <class RangeTag, class Directions, class Enable> struct view_once
    : boost::mpl::if_ <
        // First argument ("Move") to make_view is "true".
        is_implemented <make_view <true, RangeTag, Directions>>,
        make_view <true, RangeTag, Directions>,
        detail::passthrough_view <RangeTag, Directions>>::type {};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct view;
    template <class ... Arguments> struct view_once;
} // namespace apply

namespace callable {
    struct view : generic <apply::view> {};
    struct view_once : generic <apply::view_once> {};
} // namespace callable

/**
Turn a range into a view.
A view should be lightweight.

If the range is already a view, then the range itself is returned.

\param directions
    Directions that the view should allow traversal in.
    If this is not given, then the default direction is used.
\param range
    Range that the view should be over.
*/
static const auto view = callable::view();

/**
Turn a range into a view that, if the range is a temporary, can return rvalue
references as elements.
That is, the elements can be moved.
The caller must therefore only access each element only once (or have knowledge
particular to the view).
*/
static const auto view_once = callable::view_once();

namespace apply {

    namespace automatic_arguments {

        /* view. */
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct view : operation::unimplemented {};

        template <class Directions, class Range>
            struct view <Directions, meta::vector<>, meta::vector <Range>>
        : operation::view <typename range::tag_of <Range>::type, Directions> {};

        /* view_once. */
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct view_once : operation::unimplemented {};

        template <class Directions, class Range>
            struct view_once <Directions, meta::vector<>, meta::vector <Range>>
        : operation::view_once <
            typename range::tag_of <Range>::type, Directions> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct view
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::view, meta::vector <Arguments ...>>::type {};

    template <class ... Arguments> struct view_once
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::view_once, meta::vector <Arguments ...>>::type {};

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
                        std::forward <Ranges> (ranges)) ...));
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
                        std::forward <Ranges> (ranges)) ...)); \
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
