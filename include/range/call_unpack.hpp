// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_CALL_UNPACK_HPP_INCLUDED
#define RANGE_CALL_UNPACK_HPP_INCLUDED

#include <cstddef>

#include "meta/count_c.hpp"
#include "meta/vector_c.hpp"

#include "utility/returns.hpp"
#include "utility/storage.hpp"

#include "range/core.hpp"

namespace range {

namespace callable {

    namespace call_unpack_detail {

        struct implementation
        {
            template <class Function, class Arguments, std::size_t... Indices>
            auto operator()(
                Function && function, Arguments && arguments,
                meta::size_t_vector<Indices...>) const
                RETURNS(
                    std::forward<Function>(function)(
                        ::range::at_c<Indices>(arguments)...));
        };

    }  // namespace call_unpack_detail

    /** \brief
    Callable that passes a tuple (or similar) of arguments to a function.

    \tparam Function
        Function type; the function is stored qualified, as-is.
    */
    template <class Function> class call_unpack_function
    {
        Function function_;

    public:
        call_unpack_function(
            typename utility::storage::pass<Function>::type function)
        : function_(function)
        {}

        call_unpack_function(
            typename utility::storage::pass_rvalue<Function>::type function)
        : function_(function)
        {}

        template <class Arguments> auto operator()(Arguments && arguments)
            RETURNS(
                call_unpack_detail::implementation()(
                    function_,
                    ::range::view_once(std::forward<Arguments>(arguments)),
                    typename meta::count_c<
                        result_of<size(Arguments)>::type::value>::type()));

        template <class Arguments> auto operator()(Arguments && arguments) const
            RETURNS(
                call_unpack_detail::implementation()(
                    function_,
                    ::range::view_once(std::forward<Arguments>(arguments)),
                    typename meta::count_c<
                        result_of<size(Arguments)>::type::value>::type()));
    };

    class call_unpack
    {
    public:
        template <class Function, class Arguments>
        auto operator()(Function && function, Arguments && arguments) const
            RETURNS(
                call_unpack_detail::implementation()(
                    std::forward<Function>(function),
                    ::range::view_once(std::forward<Arguments>(arguments)),
                    typename meta::count_c<
                        result_of<size(Arguments)>::type::value>::type()));

        template <class Function>
        call_unpack_function<Function> operator()(Function && function) const
        {
            return call_unpack_function<Function>(
                std::forward<Function>(function));
        }
    };

}  // namespace callable

/** \brief
Call a function with parameters taken from a range.

The range's length and types must be known at compile time, like a \ref tuple.

If no range is given, a lazy functor is returned which takes a range and then
calls the function with it.

If the function should be called straight away, but with no arguments, then
an empty tuple can be passed in.

\param function
    The function (or function object) to be called.
    If no argument tuple is given, an rvalue function is stored as a value,
    and an lvalue reference function is stored as this reference.
\param arguments (optional)
    The range with arguments that the function is called with.
*/
static auto const call_unpack = callable::call_unpack();

}  // namespace range

#endif  // RANGE_CALL_UNPACK_HPP_INCLUDED
