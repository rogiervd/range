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
