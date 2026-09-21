/*
Copyright 2014, 2015, 2026 Rogier van Dalen.

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
Expose return values that are ranges as Python iterators.
*/

#ifndef RANGE_PYTHON_RETURN_VIEW_HPP_INCLUDED
#define RANGE_PYTHON_RETURN_VIEW_HPP_INCLUDED

#include <utility>

#include <nanobind/nanobind.h>

#include "iterator.hpp"

#include "range/core.hpp"

namespace range { namespace python {

    /** \brief
    Wrap a function that returns a view (or a reference to a range) so that
    it returns a Python iterator.

    Pass the result to \c def of Nanobind.
    The function can be a free function or a member function.
    The Python iterator that the view is converted to will be traversed in
    direction \ref front, which must be the default direction.
    The view must be homogeneous.
    The return value is converted with \c range::view.

    The view can be movable but not copyable.

    You must call \c range::python::initialise_iterator() once in your
    \c NB_MODULE for this to work.

    If the function returns a reference to a range that is owned by an argument,
    for example the object that a member function is called on, then also pass
    \c nanobind::keep_alive <0, 1>() (for argument 1, the object itself) to
    \c def.
    That keeps the argument alive at least until the returned iterator goes out
    of scope.
    */
    template <class Result, class... Arguments>
    inline auto return_view(Result (*function)(Arguments...))
    {
        return [function](Arguments... arguments) -> python_iterator {
            return python_iterator(
                range::view(function(std::forward<Arguments>(arguments)...)));
        };
    }

    /// \overload
    template <class Result, class Class, class... Arguments>
    inline auto return_view(Result (Class::*function)(Arguments...))
    {
        return [function](
                   Class & object, Arguments... arguments) -> python_iterator {
            return python_iterator(
                range::view(
                    (object.*function)(std::forward<Arguments>(arguments)...)));
        };
    }

    /// \overload
    template <class Result, class Class, class... Arguments>
    inline auto return_view(Result (Class::*function)(Arguments...) const)
    {
        return [function](
                   Class const & object,
                   Arguments... arguments) -> python_iterator {
            return python_iterator(
                range::view(
                    (object.*function)(std::forward<Arguments>(arguments)...)));
        };
    }

}}  // namespace range::python

#endif  // RANGE_PYTHON_RETURN_VIEW_HPP_INCLUDED
