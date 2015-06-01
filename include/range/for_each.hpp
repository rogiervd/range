/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_FOR_EACH_HPP_INCLUDED
#define RANGE_FOR_EACH_HPP_INCLUDED

#include <boost/mpl/if.hpp>

#include "core.hpp"
#include "fold.hpp"

namespace range {

/*
Interface.
The structure of this is the same as some of detail/core_*.hpp.
*/

namespace operation {

    // Reminder.
    // The general implementation is given at the bottom of this file.
    template <class RangeTag, class Direction, class Function, class Range,
        class Enable /* = void*/>
    struct for_each;

} // namespace operation

namespace apply {

    namespace automatic_arguments {

        // for_each.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct for_each : operation::unimplemented {};

        template <class Direction, class Function, class Range>
            struct for_each <meta::vector <Direction>,
                meta::vector <Function>, meta::vector <Range>>
        : operation::for_each <typename range::tag_of <Range>::type,
            typename std::decay <Direction>::type, Function, Range &&> {};

    } // namespace automatic_arguments

    /** for_each */
    template <class ... Arguments> struct for_each
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view_once <
            automatic_arguments::for_each>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

namespace callable {
    struct for_each : generic <apply::for_each> {};
} // namespace callable

/**
Call a unary function for each element of a range, traversing it along
\a direction.

Any result from the functions is ignored.

\param direction
    (optional) The direction in which the range is traversed.
    If it is not given, then the default direction of the range is used.
\param function
    The function to be called on each element.
\param range
    The range to get the elements from.
*/
static const auto for_each = callable::for_each();

/* Implementation */

namespace operation {

    namespace for_each_detail {

        /**
        Pretend function result type to implement "for_each" using "fold".
        (In a fold, the result of the function gets passed to the next function,
        so "void" would not work.)
        */
        struct none {};

        /**
        A function for "fold" accepts a state, but for "for_each" it doesn't.
        Therefore, forward to the function, leaving out "none".
        */
        template <class Function> struct function_wrapper {
            Function function;

            function_wrapper (Function && function)
            : function (std::forward <Function> (function)) {}

            template <class Element>
                none operator() (none, Element && element) const
            {
                function (std::forward <Element> (element));
                return none();
            }
        };

        struct use_fold {
            template <class Direction, class Function, class Range>
                void operator() (Direction const & direction,
                    Function && function, Range && range) const
            {
                static_assert (range::is_view <Direction, Range>::value,
                    "Internal error: the range must be a view here.");
                function_wrapper <Function> function_wrapper (
                    std::forward <Function> (function));
                range::fold (direction, function_wrapper,
                    for_each_detail::none(), std::forward <Range> (range));
            }
        };

    } // namespace for_each_detail

    // Implemented if "fold" is implemented.
    template <class RangeTag, class Direction, class Function, class Range,
            class Enable>
        struct for_each
    : boost::mpl::if_ <is_implemented <fold <
        RangeTag, Direction, for_each_detail::function_wrapper <Function>,
            for_each_detail::none, Range>>,
        for_each_detail::use_fold, unimplemented>::type {};

} // namespace operation

} // namespace range

#endif // RANGE_FOR_EACH_HPP_INCLUDED
