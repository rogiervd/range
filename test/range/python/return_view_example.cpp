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
Provide an example usage of return_view.
The functions defined here are exported to Python, and used by
test-return_view.py.
*/

#include "range/python/return_view.hpp"

#include <vector>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "range/count.hpp"
#include "range/function_range.hpp"
#include "range/std/vector.hpp"
#include "range/tuple.hpp"

auto count(int size) RETURNS(range::count(size));

int get_next_count()
{
    static int i = 0;
    return i++;
}

range::function_range<int()> count2()
{
    return range::function_range<int()>(get_next_count);
}

class container_container
{
    std::vector<int> v;

    range::tuple<double, std::string, bool> t;

public:
    container_container() : t(13.5, "Great!", false)
    {
        v.push_back(17);
        v.push_back(19);
    }

    std::vector<int> const & get_17_19() const { return v; }

    range::tuple<double, std::string, bool> const & get_tuple() const
    {
        return t;
    }
};

NB_MODULE(return_view_example, m)
{
    namespace nb = nanobind;

    range::python::initialise_iterator(m);

    m.def("count", range::python::return_view(&count));

    m.def("count2", range::python::return_view(&count2));

    nb::class_<container_container>(m, "ContainerContainer")
        .def(nb::init<>())
        .def(
            "get_17_19",
            range::python::return_view(&container_container::get_17_19),
            nb::keep_alive<0, 1>())
        .def(
            "get_tuple",
            range::python::return_view(&container_container::get_tuple),
            nb::keep_alive<0, 1>());
}
