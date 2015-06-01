/*
Copyright 2014 Rogier van Dalen.

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

#ifndef RANGE_CALL_UNPACK_HPP_INCLUDED
#define RANGE_CALL_UNPACK_HPP_INCLUDED

#include "meta/count.hpp"

#include "range/core.hpp"
#include "range/curry.hpp"

namespace range {

namespace callable {

    class call_unpack {
        struct implementation {
            template <class Function, class Arguments, class ... Indices>
                auto operator() (Function && function, Arguments && arguments,
                    meta::vector <Indices ...>) const
            RETURNS (std::forward <Function> (function) (
                ::range::at (Indices(), arguments) ...));
        };

    public:
        template <class Function, class Arguments>
            auto operator() (Function && function, Arguments && arguments) const
        RETURNS (implementation() (
            std::forward <Function> (function),
            ::range::view_once (std::forward <Arguments> (arguments)),
            typename meta::count <result_of <size (Arguments)>::type::value
                >::type()));
    };

} // namespace callable

/** \brief
Call a function with parameters taken from a range.

The range's length and types must be known at compile time, like a \ref tuple.

\param function The function (or function object) to be called.
\param arguments The range with arguments that the function is called with.
*/
static auto const call_unpack = callable::call_unpack();

namespace curry {

    /**
    Curry a function to be called with a range of arguments.
    */
    static auto const call_unpack = callable::curried <callable::call_unpack>();

} // namespace curry

} // namespace range

#endif // RANGE_CALL_UNPACK_HPP_INCLUDED
