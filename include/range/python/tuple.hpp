/*
Copyright 2014, 2015 Rogier van Dalen.

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
