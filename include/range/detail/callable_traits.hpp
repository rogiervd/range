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

/** \file
Use utility/nested_callable.hpp to set up generic traits for callables.
*/

#ifndef RANGE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED
#define RANGE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/not.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/utility/enable_if.hpp>

#include "utility/nested_callable.hpp"

namespace callable_traits {

    /**
    Base class for marking an operation as not implemented.
    Operations should be marked as unimplemented when their return type cannot
    be computed, and in other cases where it can be decided at compile-time
    that an operation is not be implemented.
    */
    struct unimplemented {};

    /**
    Evaluate to true iff the Operation does not derive from unimplemented.
    */
    template <class Operation> struct is_implemented
    : boost::mpl::not_ <std::is_base_of <unimplemented, Operation>> {};

    /**
    Functor that forwards to a newly constructed object of class
    Apply <FixedArguments ..., Arguments ...>
    where Arguments denotes the run-time arguments that get forwarded to that
    object's operator().

    The templated type "apply" derives from Apply <...>.
    */
    template <template <class ...> class Apply, class ... FixedArguments>
        struct generic
    {
        template <class ... Arguments> struct apply
        : Apply <FixedArguments ..., Arguments ...> {};

        template <class ... Arguments> auto
            operator() (Arguments && ... arguments) const
        -> typename std::result_of <apply <Arguments ...> (Arguments ...)>::type
        {
            return apply <Arguments ...>() (
                std::forward <Arguments> (arguments) ...);
        }
    };

    /**
    Evaluate to true if the function expression is implemented.
    The functor type must have a templated class apply that takes the parameters
    Arguments and derives from unimplemented if the operation is unimplemented,
    and not if otherwise.
    */
    template <class FunctionExpression> struct callable_is_implemented;
    template <class Callable, class ... Arguments>
        struct callable_is_implemented <Callable (Arguments...)>
    : is_implemented <
        typename std::decay <Callable>::type::template apply <Arguments...>> {};

    /**
    Compile-time constant that returns whether an operation has been implemented
    for a particular set of parameters.
    */
    template <class Expression> struct has
    : nested_callable::all <callable_is_implemented <boost::mpl::_1>,
        Expression> {};

    /**
    Find the result of a nested call expression.
    */
    template <class Expression> struct result_of
    : nested_callable::result_of <Expression> {};

    /**
    Find the result of a nested call expression, or the second argument.
    If no second argument is given, do not yield any argument (useful to disable
    a function using SFINAE).
    */
    template <class ... Arguments> struct result_of_or {};

    template <class Expression> struct result_of_or <Expression>
    : boost::lazy_enable_if <has <Expression>, result_of <Expression>> {};

    template <class Expression, class Otherwise>
        struct result_of_or <Expression, Otherwise>
    : boost::mpl::eval_if <has <Expression>,
        result_of <Expression>,
        boost::mpl::identity <Otherwise>>
    {};

} // namespace callable_traits

#endif // RANGE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED
