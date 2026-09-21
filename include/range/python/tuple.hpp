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
Expose views as Python iterators.
*/

#ifndef RANGE_PYTHON_TUPLE_HPP_INCLUDED
#define RANGE_PYTHON_TUPLE_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <nanobind/nanobind.h>

#include <meta/count_c.hpp>
#include <meta/vector.hpp>

#include "range/core.hpp"

namespace range { namespace python {

    namespace detail {

        template <class View, std::size_t... Indices>
        inline nanobind::object construct_tuple(
            View const & tuple, meta::size_t_vector<Indices...>)
        {
            return nanobind::make_tuple(range::at_c<Indices>(tuple)...);
        }

    }  // namespace detail

    /** \brief
    Nanobind type caster that converts a C++ tuple to a Python tuple.

    Expose a C++ tuple type to Python by specialising Nanobind's
    \c type_caster for it, in your own code:

    \code
    namespace nanobind { namespace detail {
        template <typename Arg> struct type_caster <YourType <int, Arg>>
        : range::python::tuple_caster <YourType <int, Arg>> {};
    }}
    \endcode

    The tuple can be of any range that has fixed-length type and random access.
    The element types of the view must be convertible by Nanobind.
    The elements of the C++ tuple are copied to Python.
    Conversion from Python to C++ is not supported.
    */
    template <class Tuple> struct tuple_caster
    {
        static_assert(is_range<Tuple>::value, "Tuple must be a range.");
        static_assert(
            std::is_same<Tuple, typename std::decay<Tuple>::type>::value,
            "tuple_caster requires an unqualified type.");

        using Value = Tuple;
        static constexpr auto Name = nanobind::detail::const_name("tuple");

        template <class T> using Cast = Tuple;
        template <class T> static constexpr bool can_cast() { return true; }

        bool from_python(
            nanobind::handle, std::uint8_t,
            nanobind::detail::cleanup_list *) noexcept
        {
            return false;
        }

        static nanobind::handle from_cpp(
            Tuple const & tuple, nanobind::rv_policy,
            nanobind::detail::cleanup_list *)
        {
            typedef decltype(range::size(tuple)) size_type;
            static auto constexpr size = size_type::value;
            return detail::construct_tuple(
                       range::view(tuple), typename meta::count_c<size>::type())
                .release();
        }
    };

}}  // namespace range::python

#endif  // RANGE_PYTHON_TUPLE_HPP_INCLUDED
