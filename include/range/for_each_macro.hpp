// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

// Make Doxygen pick up the macro.
/** \file */

#ifndef RANGE_FOR_EACH_MACRO_HPP_INCLUDED
#define RANGE_FOR_EACH_MACRO_HPP_INCLUDED

#include "range/core.hpp"

namespace range { namespace for_each_macro_detail {
    template <class Type> struct assert_is_range
    {
        static_assert(
            ::range::is_range<Type>::value,
            "Second parameter to RANGE_FOR_EACH must be a range");
        static_assert(
            ::range::is_homogeneous<range::callable::view(Type)>::value,
            "Range passed into RANGE_FOR_EACH must be homogeneous");
        typedef Type type;
    };
}}  // namespace range::for_each_macro_detail

/** \brief
Iterate through a homogeneous range.

Replace a normal for-statement by
\code
    RANGE_FOR_EACH (element, range_expression) { ... }
\endcode

Here, \c element is the name by which each element can be referred to inside the
loop.
The type is computed automatically, as the exact type of <c>first (range)</c>,
including reference-qualification.

\c range_expression is a range, which can be an expression.
The expression is executed exactly once.

\note This macro has been inspired by Boost.Foreach.

\internal
The names of the internal variables include \c variable_name, so that
nested loops (which must use different variable names) do not declare
variables that hide each other's.

This is not very readable.
A more readable version with comments is in test/for_each_macro.cpp.
*/
#define RANGE_FOR_EACH(variable_name, range_expression)                          \
    /* Declare variable. */                                                      \
    if (bool RANGE_FOR_EACH_done_##variable_name = false) {                      \
    } else                                                                       \
        /* Declare reference to range_expression. */                             \
        for (auto && RANGE_FOR_EACH_range_##variable_name = range_expression;    \
             !RANGE_FOR_EACH_done_##variable_name;                               \
             RANGE_FOR_EACH_done_##variable_name = true)                         \
            /* Declare variable. */                                              \
            if (bool RANGE_FOR_EACH_seen_##variable_name = false) {              \
            } else                                                               \
                /* Actual for loop. */                                           \
                for (                                                            \
                    auto RANGE_FOR_EACH_view_##variable_name =                   \
                        ::range::forward_view(                                   \
                            static_cast<                                         \
                                decltype(RANGE_FOR_EACH_range_##variable_name)>( \
                                RANGE_FOR_EACH_range_##variable_name));          \
                    !::range::empty(RANGE_FOR_EACH_view_##variable_name);        \
                    RANGE_FOR_EACH_seen_##variable_name =                        \
                        !RANGE_FOR_EACH_seen_##variable_name)                    \
                    if (RANGE_FOR_EACH_seen_##variable_name)                     \
                        /* "break" statement inside */                           \
                        break;                                                   \
                    else                                                         \
                        /* Declare variable "variable_name". */                  \
                        for (auto && variable_name = ::range::chop_in_place(     \
                                 RANGE_FOR_EACH_view_##variable_name);           \
                             !RANGE_FOR_EACH_seen_##variable_name;               \
                             ((void) variable_name),                             \
                                     RANGE_FOR_EACH_seen_##variable_name =       \
                                         true)

#endif  // RANGE_FOR_EACH_MACRO_HPP_INCLUDED
