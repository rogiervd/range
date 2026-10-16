// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Use utility/nested_callable.hpp to set up generic traits for callables.
*/

#ifndef RANGE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED
#define RANGE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/and.hpp>
#include <boost/mpl/identity.hpp>

#include "utility/enable_if_compiles.hpp"

#include "meta/all_of_c.hpp"

namespace callable_traits {

namespace detail {

    /* result_of. */

    // By default, do not contain "type".
    template <class Expression, class Enable = void> struct result_of {};

    // Contain "type" iff the call is possible.
    // (This specialisation is taken out of consideration by SFINAE if not.)
    template <class Function, class... Arguments> struct result_of<
        Function(Arguments...),
        typename utility::enable_if_compiles<decltype(std::declval<Function>()(
            std::declval<Arguments>()...))>::type>
    {
        typedef decltype(std::declval<Function>()(
            std::declval<Arguments>()...)) type;
    };

    // In case a function pointer is passed in.
    template <class Function, class... Arguments>
    struct result_of<Function (*)(Arguments...)>
    : result_of<Function(Arguments...)> {};

    /* decayed_result_of. */

    template <class Expression, class Enable = void> struct decayed_result_of {
    };

    template <class Function, class... Arguments> struct decayed_result_of<
        Function(Arguments...),
        typename utility::enable_if_compiles<decltype(std::declval<Function>()(
            std::declval<Arguments>()...))>::type>
    : std::decay<decltype(std::declval<Function>()(
          std::declval<Arguments>()...))> {};

    template <class Function, class... Arguments>
    struct decayed_result_of<Function (*)(Arguments...)>
    : decayed_result_of<Function(Arguments...)> {};

    /* has. */

    // By default, no.
    template <class Expression, class Enable = void> struct has
    : std::false_type {};

    // Iff the call is possible, yes.
    // (This specialisation is taken out of consideration by SFINAE if not.)
    template <class Function, class... Arguments> struct has<
        Function(Arguments...),
        typename utility::enable_if_compiles<decltype(std::declval<Function>()(
            std::declval<Arguments>()...))>::type> : std::true_type {};

    // In case a function pointer is passed in.
    template <class Function, class... Arguments>
    struct has<Function (*)(Arguments...)> : has<Function(Arguments...)> {};

}  // namespace detail

/** \brief
Compute the result of a call expression, as a \c typedef \c type.

If the expression is invalid, then do not contain \c type at all.
*/
template <class Expression> struct result_of : detail::result_of<Expression> {};

/** \brief
Compute the decayed result of a call expression, as a \c typedef \c type.

If the expression is invalid, then do not contain \c type at all.
*/
template <class Expression> struct decayed_result_of
: detail::decayed_result_of<Expression> {};

/** \brief
Boolean constant that indicates whether a function call expression is valid.

Iff compiling the call succeeds, <c>value == true</c>.
*/
template <class Expression> struct has : detail::has<Expression> {};

}  // namespace callable_traits

#endif  // RANGE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED
