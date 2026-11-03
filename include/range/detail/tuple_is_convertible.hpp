// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define traits that determine whether a sequence of types can be constructed from
a sequence of arguments implicitly, or only explicitly.
These are used to make the element-wise constructor of \c tuple conditionally
explicit.
*/

#ifndef RANGE_DETAIL_TUPLE_IS_CONVERTIBLE_HPP_INCLUDED
#define RANGE_DETAIL_TUPLE_IS_CONVERTIBLE_HPP_INCLUDED

#include <type_traits>

#include "utility/type_sequence_traits.hpp"

namespace range { namespace tuple_detail {

    /**
    Evaluate to \c true iff \a Type can be constructed from \a Argument, and
    \a Argument is implicitly convertible to \a Type.
    */
    template <class Type, class Argument> struct is_implicitly_constructible
    : std::integral_constant<
          bool,
          std::is_constructible<Type, Argument>::value
              && std::is_convertible<Argument, Type>::value> {};

    /**
    Evaluate to \c true iff the meta ranges \a Types and \a Arguments are of the
    same length, and each type in \a Types can be constructed from the
    corresponding argument in \a Arguments, and each argument is implicitly
    convertible to the corresponding type.

    This is the condition under which a constructor that takes each element
    separately can be implicit.
    */
    template <class Types, class Arguments> struct are_implicitly_constructible
    : utility::all_binary<is_implicitly_constructible, Types, Arguments> {};

    /**
    Evaluate to \c true iff the meta ranges \a Types and \a Arguments are of the
    same length, and each type in \a Types can be constructed from the
    corresponding argument in \a Arguments, but at least one of the arguments is
    not implicitly convertible to the corresponding type.

    This is the condition under which a constructor that takes each element
    separately must be explicit.
    */
    template <class Types, class Arguments>
    struct are_only_explicitly_constructible
    : std::integral_constant<
          bool,
          utility::are_constructible<Types, Arguments>::value
              && !are_implicitly_constructible<Types, Arguments>::value> {};

}}  // namespace range::tuple_detail

#endif  // RANGE_DETAIL_TUPLE_IS_CONVERTIBLE_HPP_INCLUDED
