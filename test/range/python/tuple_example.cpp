/*
Copyright 2015 Rogier van Dalen.

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

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "range/tuple.hpp"
#include "range/std/tuple.hpp"
#include "range/transform.hpp"
#include "range/view_shared.hpp"

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
RETURNS (range::transform (twice(), range::view_shared (
    get_int_float_string (i, f))));

BOOST_PYTHON_MODULE (tuple_example) {
    using namespace boost::python;

    double_string = std::make_tuple (6.5, "Excellent.");

    range::python::register_tuple <std::tuple <double, std::string>>();
    range::python::register_tuple <range::tuple <int, float, std::string>>();
    range::python::register_tuple <decltype (get_twice (5, 6))>();

    def ("getDoubleString", &get_double_string);
    def ("getIntBoolString", &get_int_float_string);
    def ("getTwice", &get_twice);
}
