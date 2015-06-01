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

/* \file
Provide an example usage of python_range.
The functions defined here are exported to Python, and used by
test-python_range.py.
*/

#include "range/python/return_view.hpp"

#include <vector>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>

#include "range/count.hpp"
#include "range/function_range.hpp"
#include "range/std/container.hpp"
#include "range/tuple.hpp"

auto count (int size) RETURNS (range::count (size));

int get_next_count() {
    static int i = 0;
    return i ++;
}

range::function_range <int()> count2() {
    return range::function_range <int()> (get_next_count);
}

class container_container {
    std::vector <int> v;

    range::tuple <double, std::string, bool> t;

public:
    container_container()
    : t (13.5, "Great!", false)
    {
        v.push_back (17);
        v.push_back (19);
    }

    std::vector <int> const & get_17_19() const { return v; }

    range::tuple <double, std::string, bool> const & get_tuple() const
    { return t; }
};

int test();

BOOST_PYTHON_MODULE (return_view_example) {
    using namespace boost::python;
    using namespace range::python;

    range::python::initialise_iterator();

    def ("count", &count, range::python::return_view<>());

    def ("count2", &count2, range::python::return_view<>());

    class_ <container_container> ("ContainerContainer")
        .def ("get_17_19", &container_container::get_17_19,
            range::python::return_view_of_internal_reference <1>())
        .def ("get_tuple", &container_container::get_tuple,
            range::python::return_view_of_internal_reference <1>());
}
