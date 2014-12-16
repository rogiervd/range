/*
Copyright 2013, 2014 Rogier van Dalen.

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

#ifndef RANGE_FOR_EACH_MACRO_HPP_INCLUDED
#define RANGE_FOR_EACH_MACRO_HPP_INCLUDED

#include "range/core.hpp"

namespace range { namespace for_each_macro_detail {
    template <class Type> struct assert_is_range {
        static_assert (::range::is_range <Type>::value,
            "Second parameter to RANGE_FOR_EACH must be a range");
        static_assert (::range::is_homogeneous <
            range::callable::view (Type)>::value,
            "Range passed into RANGE_FOR_EACH must be homogeneous");
        typedef Type type;
    };
}} // namespace range::for_each_macro_detail

/** \macro RANGE_FOR_EACH
Iterate through a range.
Replace a normal for-statement by
    <tt> RANGE_FOR_EACH (element, range_expression) { ... } </tt>

Here, \c element is the name by which each element can be referred to inside the
loop.
It is of the exact type of first (range), including reference-qualification.

\c range_expression is a range, which can be an expression.
The expression is executed exactly once.

\note This macro has been inspired by Boost.Foreach, though the code is much
    simpler thanks to C++11.

\internal
This is not very readable.
A more readable version with comments is in test/for_each_macro.cpp.
*/
#define RANGE_FOR_EACH(variable_name, range_expression) \
        /* Declare variable. */ \
        if (bool RANGE_FOR_EACH_internal_done = false) {} else \
        /* Declare reference to range_expression. */ \
        for (auto && RANGE_FOR_EACH_internal_range = range_expression; \
            !RANGE_FOR_EACH_internal_done; \
            RANGE_FOR_EACH_internal_done = true) \
        /* Declare variable. */ \
        if (bool RANGE_FOR_EACH_internal_seen = false) {} else \
            /* Actual for loop. */ \
            for (auto RANGE_FOR_EACH_internal_view = \
                    ::range::forward_view (static_cast < \
                        decltype (RANGE_FOR_EACH_internal_range)> ( \
                            RANGE_FOR_EACH_internal_range)); \
                    ! ::range::empty (RANGE_FOR_EACH_internal_view); \
                    RANGE_FOR_EACH_internal_seen = \
                        !RANGE_FOR_EACH_internal_seen) \
                if (RANGE_FOR_EACH_internal_seen) \
                    /* "break" statement inside */ \
                    break; \
                else \
                    /* Declare variable "variable_name". */ \
                    for (auto && variable_name = \
                        ::range::chop_in_place (RANGE_FOR_EACH_internal_view); \
                        !RANGE_FOR_EACH_internal_seen; \
                        RANGE_FOR_EACH_internal_seen = true)

#endif  // RANGE_FOR_EACH_MACRO_HPP_INCLUDED

