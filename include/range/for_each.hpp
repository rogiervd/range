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

#include "rime/nothing.hpp"

#include "core.hpp"
#include "fold.hpp"

namespace range {

/* Implementation */

namespace for_each_detail {

    /**
    Pretend result type to implement "for_each" using "fold".
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
        // This uses "auto" merely to enable SFINAE; the return type is always
        // void.
        template <class Range, class Direction, class Function>
            auto operator() (Range && range, Direction const & direction,
                Function && function) const
        RETURNS (rime::nothing (range::fold (
            none(), std::forward <Range> (range), direction,
            function_wrapper <Function> (std::forward <Function> (function)))));
    };

} // namespace for_each_detail

/* Interface. */

namespace helper {

    /** \brief
    Hook for implementing for() for a type of range.

    This does normally not have to be implemented, unless the default
    implementation (which uses fold()) does not suffice.

    To provide an implementation of for_each() specific to a range, implement
    either member function for_each() on the range, or free function
    implement_for_each().
    If both of these are defined, then the free function will be preferred.

    \param tag The range tag.
    \param range The range to get the elements from.
    \param direction The direction in which the range is traversed.
    \param function The function to be called on each element.
    */
    void implement_for_each (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_for_each;

        class for_each {
            struct dispatch {
                // Use implement_for_each, if it is implemented.
                template <class Range, class Direction, class Function>
                    auto operator() (Range && range,
                        Direction const & direction, Function && function,
                        overload_order <1> *) const
                RETURNS (implement_for_each (tag_of <Range>::type(),
                    std::forward <Range> (range),
                    direction, std::forward <Function> (function)));

                // Use member function .for_each, if it is implemented.
                template <class Range, class Direction, class Function>
                    auto operator() (Range && range,
                        Direction const & direction, Function && function,
                        overload_order <2> *) const
                RETURNS (helper::member_access::for_each (
                    std::forward <Range> (range), direction,
                    std::forward <Function> (function)));

                // Use default implementation.
                template <class Range, class Direction, class Function>
                    auto operator() (Range && range,
                        Direction const & direction, Function && function,
                        overload_order <3> *) const
                RETURNS (for_each_detail::use_fold() (
                        std::forward <Range> (range), direction,
                    std::forward <Function> (function)));
            };

        public:
            template <class Range, class Direction, class Function>
            auto operator() (Range && range, Direction const & direction,
                Function && function) const
            RETURNS (dispatch() (
                range::view_once (std::forward <Range> (range), direction),
                direction,
                std::forward <Function> (function), pick_overload()));

            // Without direction: use default_direction.
            template <class Range, class Function>
                auto operator() (Range && range, Function && function) const
            RETURNS (dispatch() (
                range::view_once (std::forward <Range> (range)),
                range::default_direction (range),
                std::forward <Function> (function), pick_overload()));
        };

    } // namespace implementation

    using implementation::for_each;

} // namespace callable

/**
Call a unary function for each element of a range, traversing it along
\a direction.

Any result from the functions is ignored.

\param range
    The range to get the elements from.
\param direction
    (optional) The direction in which the range is traversed.
    If it is not given, then the default direction of the range is used.
\param function
    The function to be called on each element.
*/
static const auto for_each = callable::for_each();

} // namespace range

#endif // RANGE_FOR_EACH_HPP_INCLUDED
