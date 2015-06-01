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

/** \file
Expose return values that are ranges as Python iterators.
*/

#ifndef RANGE_PYTHON_RETURN_VIEW_HPP_INCLUDED
#define RANGE_PYTHON_RETURN_VIEW_HPP_INCLUDED

// Include the Python C API, safely.
#include <boost/python/detail/wrap_python.hpp>

#include <boost/python/default_call_policies.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/with_custodian_and_ward.hpp>

#include "iterator.hpp"

#include "range/core.hpp"

namespace range { namespace python {

    /** \brief
    A call policy for Boost.Python to return a view as a Python iterator.

    Use this call policy for a function that returns a view.
    The Python iterator that the view is converted to will be traversed in
    direction \ref front, which must be the default direction.
    The view must be homogeneous.

    The view can be movable but not copyable.
    This may depend on the implementation of Boost.Python and be a bit lucky,
    but our luck is unlikely to change since any new version of Boost.Python
    will surely consider rvalue references.

    You must call \c range::python::initialise_iterator() once in your
    \c BOOST_PYTHON_MODULE for this call policy to work.

    \sa return_view_of_internal_reference
    */
    template <class BasePolicy = boost::python::default_call_policies>
        struct return_view
    : BasePolicy
    {
        struct result_converter {
            template <class View> struct apply {
                static_assert (is_view <View>::value,
                    "To use the return_view call policy for Boost.Python, "
                    "the return type must be a view.");

                static_assert (std::is_same <View,
                            typename std::decay <View>::type>::value,
                    "To use the return_view call policy for Boost.Python, "
                    "the return type must be unqualied.");

                typedef detail::iterator_converter <View> type;
            };
        };
    };

    /** \brief
    A call policy for Boost.Python to return a reference to a range as a Python
    iterator.

    This assumes that the range that the return value references is contained
    in argument \a owner_argument_index of the function.
    After the function is called, range::view will called on the range.
    The Python iterator that the view is converted to will be traversed in
    direction \ref front, which must be the default direction.
    The view must be homogeneous.

    This is similar to the standard
    <c>boost::python::return_internal_reference</c> in that it keeps an argument
    alive at least until the return value goes out of scope.
    Additionally, however, the return value is converted into a view.

    You must call \c range::python::initialise_iterator() once in your
    \c BOOST_PYTHON_MODULE for this call policy to work.

    \sa return_view
    */
    template <std::size_t owner_argument_index,
            class BasePolicy = boost::python::default_call_policies>
        struct return_view_of_internal_reference
    : boost::python::with_custodian_and_ward_postcall <
        0, owner_argument_index, BasePolicy>
    {
        static_assert (owner_argument_index > 0,
            "The index of the argument that owns the result must be greater "
            "than 0 (0 means the result itself).");

        struct result_converter {
            template <class Range> struct apply {
                static_assert (is_range <Range>::value,
                    "To use the return_internal_reference_view call policy "
                    "for Boost.Python, the return type must be "
                    "(a reference to) a range.");

                static_assert (std::is_lvalue_reference <Range>::value,
                    "To use the return_internal_reference_view call policy "
                    "for Boost.Python, the return type must be "
                    "an lvalue reference (to a range).");

                typedef detail::iterator_converter <
                    typename std::decay <Range>::type> type;
            };
        };
    };

}} // namespace range::python

#endif // RANGE_PYTHON_RETURN_VIEW_HPP_INCLUDED
