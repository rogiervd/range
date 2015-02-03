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
Provide an example usage of python/iterator.hpp.
The functions defined here are exported to Python, and used by
test-python_range.py.
*/

#include "range/python/iterator.hpp"

#include <list>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include <boost/optional.hpp>

#include "range/std/container.hpp"
#include "range/std/view_optional.hpp"
#include "range/tuple.hpp"

std::list <double> doubles;

auto get_doubles() RETURNS (range::view (doubles));

void set_first_to_doubles (boost::python::object & object) {
    object [0] = range::view (doubles);
}

range::tuple <int, std::string, float> tuple;

auto get_tuple() RETURNS (range::view (tuple));

boost::optional <bool> optional;

auto get_optional() RETURNS (range::view_optional (optional));

BOOST_PYTHON_MODULE (iterator_example) {
    doubles.push_back (3.5);
    doubles.push_back (7.25);

    tuple = range::make_tuple (6, "hello", 17.5f);

    optional = true;

    range::python::initialise_iterator();
    range::python::register_view <decltype (get_doubles())>();
    range::python::register_view <decltype (get_tuple())>();
    range::python::register_view <decltype (get_optional())>();

    using namespace boost::python;

    def ("getDoubles", &get_doubles);
    def ("setFirstToDoubles", &set_first_to_doubles);
    def ("getTuple", &get_tuple);
    def ("getOptional", &get_optional);
}
