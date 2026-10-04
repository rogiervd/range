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

#include <cstdint>
#include <type_traits>

#include <nanobind/nanobind.h>

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
    the containers support in Nanobind (the headers in nanobind/stl).
    */
    class python_iterator {
        any_range <nanobind::object, capability::unique_capabilities> range;

        // Convert any object to Python.
        struct to_python_object {
            template <class Type>
                nanobind::object operator() (Type && o) const
            { return nanobind::cast (std::forward <Type> (o)); }
        };

    public:
        template <class Range2, class Enable = typename
            utility::disable_if_same_or_derived <python_iterator, Range2>::type>
        python_iterator (Range2 && range)
        : range (range::transform (std::forward <Range2> (range),
            to_python_object())) {}

        python_iterator (python_iterator &&) = default;

        /** \brief
        Return the next element of the view, as a nanobind::object, and
        move on to the next element.

        This is the behaviour of a Python iterator.
        Raises \c StopIteration if there are no more elements.
        */
        nanobind::object next() {
            if (empty (range))
                throw nanobind::stop_iteration (
                    "No more elements in C++ range.");
            return chop_in_place (range);
        }

        python_iterator & iter() { return *this; }
    };

    /** \brief
    Initialise support for Python iterators.

    This must be called once in your \c NB_MODULE, before any function that
    returns a view is called.
    */
    inline void initialise_iterator (nanobind::module_ & module) {
        nanobind::class_ <python_iterator> (module, "CppRangeIterator")
            // If it quacks like a duck...
            .def ("__next__", &python_iterator::next)
            .def ("__iter__", &python_iterator::iter,
                nanobind::rv_policy::reference);
    }

    /** \brief
    Nanobind type caster that converts a view to a Python iterator.

    Expose a view type to Python by specialising Nanobind's \c type_caster for
    it, in your own code, before the type is used in a bound function:

    \code
    namespace nanobind { namespace detail {
        template <> struct type_caster <my_view>
        : range::python::view_caster <my_view> {};
    }}
    \endcode

    The element type of the view must be convertible by Nanobind.
    The view will be traversed in direction \ref front, which must be the
    default direction.
    Conversion from Python to C++ is not supported.
    \c initialise_iterator must have been called, probably in your NB_MODULE
    function.
    */
    template <class View> struct view_caster {
        static_assert (is_view <View>::value,
            "view_caster can only be used on views.");

        static_assert (std::is_same <View, typename std::decay <View>::type
            >::value, "view_caster requires an unqualified type.");

        static_assert (std::is_same <typename
                decayed_result_of <callable::default_direction (View)>::type,
                direction::front
            >::value, "The default direction must be direction::front.");

        using Value = View;
        static constexpr auto Name =
            nanobind::detail::const_name ("CppRangeIterator");

        template <class T> using Cast = View;
        template <class T> static constexpr bool can_cast() { return true; }

        bool from_python (nanobind::handle, std::uint8_t,
            nanobind::detail::cleanup_list *) noexcept
        { return false; }

        static nanobind::handle from_cpp (View const & view,
            nanobind::rv_policy, nanobind::detail::cleanup_list *)
        {
            // Make a typed iterator, owned by Python.
            return nanobind::cast (python_iterator (range::view (view)),
                nanobind::rv_policy::move).release();
        }
    };

}} // namespace range::python

#endif // RANGE_PYTHON_ITERATOR_HPP_INCLUDED
