// SPDX-FileCopyrightText: Copyright 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Helpers for all() and any().
*/

#ifndef RANGE_DETAIL_FIND_BOOL_HPP_INCLUDED
#define RANGE_DETAIL_FIND_BOOL_HPP_INCLUDED

#include "rime/always.hpp"
#include "rime/core.hpp"

#include "utility/returns.hpp"

#include "../find.hpp"

namespace range { namespace callable { namespace find_bool_detail {

    /**
    Check the truth value either at compile time or run time.
    */
    template <bool value> struct equals;

    template <> struct equals<false>
    {
        template <class Type> auto operator()(Type const & v) const
            RETURNS(rime::not_(v));
    };

    template <> struct equals<true>
    {
        template <class Type> auto operator()(Type const & v) const
            RETURNS(!rime::not_(v));
    };

    /**
    Generic implementation of all() and any().
    all() finds the first element that evaluates to false and returns false iff
    it finds one. any() finds the first element that evaluates to true and
    returns true iff it finds one.
    */
    template <bool Value> struct find_bool
    {
        equals<!Value> equals_value;
        rime::callable::always_default<rime::bool_<!Value>> when_not_empty;
        rime::callable::always_default<rime::bool_<Value>> when_empty;

        template <class Range, class Direction>
        auto operator()(Range && range, Direction const & direction) const
            RETURNS(
                range::find(
                    std::forward<Range>(range), direction, equals_value,
                    when_not_empty, when_empty));

        template <class Range> auto operator()(Range && range) const RETURNS(
            range::find(
                std::forward<Range>(range), equals_value, when_not_empty,
                when_empty));
    };

}}}  // namespace range::callable::find_bool_detail

#endif  // RANGE_DETAIL_FIND_BOOL_HPP_INCLUDED
