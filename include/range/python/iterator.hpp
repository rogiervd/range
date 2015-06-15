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

#ifndef RANGE_PYTHON_ITERATOR_HPP_INCLUDED
#define RANGE_PYTHON_ITERATOR_HPP_INCLUDED

// Include the Python C API, safely.
#include <boost/python/detail/wrap_python.hpp>

#include <type_traits>

#include <boost/python/object.hpp>
#include <boost/python/class.hpp>
#include <boost/python/return_arg.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/to_python_converter.hpp>
#include <boost/python/errors.hpp>

#include "utility/disable_if_same.hpp"

#include "range/core.hpp"
#include "range/any_range.hpp"
#include "range/transform.hpp"

namespace range { namespace python {

    /** \brief
    Present a view of a range as something that can act like a Python
    iterator.

    The underlying range must implement \c empty() and \c chop_in_place().

    If you want to use a full-fledged container, instead of using this, see
    http://www.boost.org/doc/libs/release/libs/python/doc/v2/indexing.html
    */
    class python_iterator {
        any_range <boost::python::object, capability::unique_capabilities>
            range;

        // Convert any object to Python.
        struct to_python_object {
            template <class Type>
                boost::python::object operator() (Type && o) const
            { return boost::python::object (std::forward <Type> (o)); }
        };

        void stop_iteration() {
            PyErr_SetString (PyExc_StopIteration,
                "No more elements in C++ range.");
            boost::python::throw_error_already_set();
        }

    public:
        template <class Range2, class Enable = typename
            utility::disable_if_same_or_derived <python_iterator, Range2>::type>
        python_iterator (Range2 && range)
        : range (range::transform (std::forward <Range2> (range),
            to_python_object())) {}

        /** \brief
        Return the next element of the view, as a boost::python::object, and
        move on to the next element.

        This is the behaviour of a Python iterator.
        */
        boost::python::object next() {
            if (empty (range))
                stop_iteration();
            return boost::python::object (chop_in_place (range));
        }

        python_iterator & iter() { return *this; }
    };

    namespace detail {

        /* Returning views. */
        template <class Range> struct iterator_converter {
            static_assert (is_range <Range>::value, "Range must be a range.");

            bool convertible() const { return true; }

            template <class QRange>
                PyObject * operator() (QRange && range) const
            {
                static_assert (std::is_same <
                    typename std::decay <QRange>::type,
                    typename std::decay <Range>::type>::value,
                    "Boost.Python should call this with a possibly "
                    "differently-qualified version of Range.");

                // Make a typed iterator. On the heap!
                python_iterator * iterator =
                    new python_iterator (
                        range::view (std::forward <QRange> (range)));

                // Make converter that takes ownership of the new object.
                boost::python::manage_new_object
                    ::apply <python_iterator *>::type iterator_converter;

                return iterator_converter (iterator);
            }

            PyTypeObject const * get_pytype() const { return 0; }
        };

        template <class Range> struct convert_iterator {
            static PyObject * convert (Range const & range) {
                iterator_converter <Range> converter;
                return converter (range);
            }
        };

    } // namespace detail

    /** \brief
    Initialise support for Python iterators.

    This must be called once in your \c BOOST_PYTHON_MODULE.
    */
    inline void initialise_iterator() {
        using namespace boost::python;

        class_ <python_iterator, boost::noncopyable> (
            "CppRangeIterator", no_init)
            // If it quacks like a duck...
            .def (
#if PY_VERSION_HEX >= 0x03000000
                "__next__",
#else
                "next",
#endif
                &python_iterator::next)
            .def ("__iter__", &python_iterator::iter, return_self<>());
    }

    /** \brief
    Register a view type to be exposes to Python as an iterator.

    This must be called in your \c BOOST_PYTHON_MODULE, once for each type of
    View.

    The element type of the view must be convertible to
    \c boost::python::object.
    The view will be traversed in direction \ref front, which must be the
    default direction.
    */
    template <class View> inline void register_view() {
        static_assert (is_view <View>::value,
            "register_view can only be used on views.");

        static_assert (std::is_same <View, typename std::decay <View>::type
            >::value, "register_view requires an unqualied type.");

        static_assert (std::is_same <typename
                decayed_result_of <callable::default_direction (View)>::type,
                direction::front
            >::value, "The default direction must be direction::front.");

        boost::python::to_python_converter <View,
            detail::convert_iterator <View>>();
    }

}} // namespace range::python

#endif // RANGE_PYTHON_ITERATOR_HPP_INCLUDED
