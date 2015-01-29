/*
Copyright 2014 Rogier van Dalen.

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

public:
    container_container() {
        v.push_back (17);
        v.push_back (19);
    }

    std::vector <int> const & get_17_19() const { return v; }
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
            range::python::return_view_of_internal_reference <1>());
}
