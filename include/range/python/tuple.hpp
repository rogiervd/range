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
Expose views as Python iterators.
*/

#ifndef RANGE_PYTHON_TUPLE_HPP_INCLUDED
#define RANGE_PYTHON_TUPLE_HPP_INCLUDED

// Include the Python C API, safely.
#include <boost/python/detail/wrap_python.hpp>

#include <type_traits>

#include <boost/python/handle.hpp>
#include <boost/python/to_python_converter.hpp>
#include <boost/python/tuple.hpp>

#include <meta/count.hpp>
#include <meta/vector.hpp>

#include "range/core.hpp"

namespace range { namespace python {

    namespace detail {

        template <class Tuple> class convert_tuple {
            static_assert (is_range <Tuple>::value, "Tuple must be a range.");

            template <class View, class ... Indices>
                static PyObject * construct_tuple (
                    View const & tuple, meta::vector <Indices ...>)
            {
                return boost::python::incref (boost::python::make_tuple (
                        range::at (Indices(), tuple) ...)
                    .ptr());
            }

        public:
            static PyObject * convert (Tuple const & tuple) {
                typedef decltype (range::size (tuple)) size_type;
                static auto constexpr size = size_type::value;
                return construct_tuple (range::view (tuple),
                    typename meta::count <size>::type());
            }
        };

    } // namespace detail

    /** \brief
    Register a C++ tuple type to be exposed to Python as a tuple.

    This must be called in your \c BOOST_PYTHON_MODULE, once for each type of
    Tuple.

    The tuple can be of any range that has fixed-length type and random access.
    The element types of the view must be convertible to
    \c boost::python::object.
    The elements of the C++ tuple are copied to Python.
    */
    template <class Tuple> inline void register_tuple() {
        static_assert (std::is_same <Tuple, typename std::decay <Tuple>::type
            >::value, "register_tuple requires an unqualied type.");

        boost::python::to_python_converter <Tuple,
            detail::convert_tuple <Tuple>>();
    }

}} // namespace python::range

#endif // RANGE_PYTHON_TUPLE_HPP_INCLUDED
