/*
Copyright 2015 Rogier van Dalen.

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
