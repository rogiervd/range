/*
Copyright 2013-2015 Rogier van Dalen.

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

// Make Doxygen pick up the macro.
/** \file */

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

