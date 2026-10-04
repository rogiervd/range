/*
Copyright 2015, 2026 Rogier van Dalen.

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

/* \file
Provide an example usage of python/tuple.hpp.
The functions defined here are exported to Python, and used by
test-tuple.py.
*/

#include "range/python/tuple.hpp"

#include <tuple>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "range/tuple.hpp"
#include "range/std/tuple.hpp"
#include "range/transform.hpp"
#include "range/view_shared.hpp"

namespace nb = nanobind;

std::tuple <double, std::string> double_string;

std::tuple <double, std::string> get_double_string()
{ return double_string; }

range::tuple <int, float, std::string> get_int_float_string (int i, float f) {
    return range::make_tuple (i, f, "Wow!");
}

struct twice {
    template <class Type> Type operator() (Type const & o) const
    { return o + o; }
};

auto get_twice (int i, float f)
RETURNS (range::transform (range::view_shared (get_int_float_string (i, f)),
    twice()));

// Tell Nanobind how to convert the return types to Python.
namespace nanobind { namespace detail {

    // The type caster for std::tuple already exists but this specialises it
    // for no good reason.
    template <> struct type_caster <std::tuple <double, std::string>>
    : range::python::tuple_caster <std::tuple <double, std::string>> {};

    template <> struct type_caster <range::tuple <int, float, std::string>>
    : range::python::tuple_caster <range::tuple <int, float, std::string>> {};

    template <> struct type_caster <decltype (get_twice (5, 6))>
    : range::python::tuple_caster <decltype (get_twice (5, 6))> {};

}} // namespace nanobind::detail

NB_MODULE (tuple_example, m) {
    double_string = std::make_tuple (6.5, "Excellent.");

    m.def ("getDoubleString", &get_double_string);
    m.def ("getIntBoolString", &get_int_float_string);
    m.def ("getTwice", &get_twice);
}
