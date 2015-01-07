/*
Copyright 2014 Rogier van Dalen.

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
