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

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "range/python/range.hpp"

using range::python_range;

void test_static()
{
    static_assert(std::is_same<
                  range::tag_of<python_range<>>::type,
                  range::python_range_operation::python_range_tag>::value);
    static_assert(std::is_same<
                  range::tag_of<python_range<double> &>::type,
                  range::python_range_operation::python_range_tag>::value);
    static_assert(std::is_same<
                  range::tag_of<python_range<double> const &>::type,
                  range::python_range_operation::python_range_tag>::value);
    static_assert(std::is_same<
                  range::tag_of<python_range<double, int> const &&>::type,
                  range::python_range_operation::python_range_tag>::value);
    static_assert(std::is_same<
                  range::tag_of<python_range<double, int, float> &&>::type,
                  range::python_range_operation::python_range_tag>::value);

    static_assert(
        range::has<range::callable::empty(range::python_range<int>)>::value);
    static_assert(range::has<range::callable::empty(
                      range::python_range<int> const &)>::value);

    static_assert(
        !range::has<range::callable::size(range::python_range<int> &)>::value);

    static_assert(
        range::has<range::callable::first(range::python_range<int>)>::value);
    static_assert(
        range::has<range::callable::first(range::python_range<int> &)>::value);
    static_assert(range::has<range::callable::first(
                      range::python_range<int> const &)>::value);
    static_assert(!range::has<range::callable::first(
                      direction::back, range::python_range<int>)>::value);

    // drop: only for rvalues.
    static_assert(
        range::has<range::callable::drop(range::python_range<int>)>::value);
    static_assert(
        !range::has<range::callable::drop(range::python_range<int> &)>::value);
    static_assert(!range::has<range::callable::drop(
                      range::python_range<int> const &)>::value);
    static_assert(!range::has<range::callable::drop(
                      direction::back, range::python_range<int>)>::value);

    // chop: only for rvalues.
    static_assert(
        range::has<range::callable::chop(range::python_range<int>)>::value);
    static_assert(
        !range::has<range::callable::chop(range::python_range<int> &)>::value);
    static_assert(!range::has<range::callable::chop(
                      range::python_range<int> const &)>::value);
    static_assert(!range::has<range::callable::chop(
                      direction::back, range::python_range<int>)>::value);

    // chop_in_place: only for lvalue references.
    static_assert(!range::has<range::callable::chop_in_place(
                      range::python_range<int>)>::value);
    static_assert(range::has<range::callable::chop_in_place(
                      range::python_range<int> &)>::value);
    static_assert(!range::has<range::callable::chop_in_place(
                      range::python_range<int> const &)>::value);
    static_assert(!range::has<range::callable::chop_in_place(
                      direction::back, range::python_range<int> &)>::value);
    // But only for homogeneous ranges.
    static_assert(range::has<range::callable::chop_in_place(
                      range::python_range<> &)>::value);
    static_assert(range::has<range::callable::chop_in_place(
                      range::python_range<int> &)>::value);
    static_assert(!range::has<range::callable::chop_in_place(
                      range::python_range<int, double> &)>::value);

    // Return types.
    // first.
    static_assert(std::is_same<
                  range::result_of<range::callable::first(
                      range::python_range<>)>::type,
                  nanobind::object>::value);
    static_assert(std::is_same<
                  range::result_of<range::callable::first(
                      range::python_range<int>)>::type,
                  int>::value);
    static_assert(std::is_same<
                  range::result_of<range::callable::first(
                      range::python_range<double, char>)>::type,
                  double>::value);

    // drop.
    static_assert(std::is_same<
                  range::result_of<range::callable::drop(
                      range::python_range<>)>::type,
                  range::python_range<>>::value);
    static_assert(std::is_same<
                  range::result_of<range::callable::drop(
                      range::python_range<int>)>::type,
                  range::python_range<int>>::value);
    static_assert(std::is_same<
                  range::result_of<range::callable::drop(
                      range::python_range<double, char>)>::type,
                  range::python_range<char>>::value);
}

template <class Range> void check_empty(Range range)
{
    assert(range::empty(range));
}

template <class DoubleRange> void check_6_25_8_5(DoubleRange double_range)
{
    assert(range::first(double_range) == 6.25);
    double_range = range::drop(std::move(double_range));
    assert(!range::empty(double_range));
    assert(range::first(double_range) == 8.5);
    double_range = range::drop(std::move(double_range));
    assert(range::empty(double_range));
    assert(range::empty(double_range));
    assert(range::empty(double_range));
}

template <class DoubleRange> void check_6_25_8_5_chop(DoubleRange double_range)
{
    assert(range::chop_in_place(double_range) == 6.25);
    assert(!range::empty(double_range));
    assert(range::chop_in_place(double_range) == 8.5);
    assert(range::empty(double_range));
}

template <class DoubleRange>
void check_6_25_8_5_chop_in_place(DoubleRange double_range)
{
    auto next = range::chop(std::move(double_range));
    assert(next.first() == 6.25);
    assert(!range::empty(next.rest()));
    next = range::chop(std::move(next.rest()));
    assert(next.first() == 8.5);
    assert(range::empty(next.rest()));
}

void check_5_hello_untyped(python_range<> range)
{
    assert(!range::empty(range));
    assert(nanobind::cast<int>(range::first(range)) == 5);
    auto range2 = range::drop(std::move(range));
    std::string s = nanobind::cast<std::string>(range::first(range2));
    assert(s == "hello");
    // This should be possible: at the end of the type sequence, the last
    // element (here, std::string) gets repeated forever.
    range2 = range::drop(std::move(range2));
    assert(range::empty(range2));
}

void check_5_hello_typed(python_range<int, std::string> range)
{
    assert(!range::empty(range));
    assert(range::first(range) == 5);
    auto range2 = range::drop(std::move(range));
    auto second = range::chop(std::move(range2));
    assert(second.first() == "hello");
    // This should be possible: at the end of the type sequence, the last
    // element (here, std::string) gets repeated forever.
    range2 = std::move(second.rest());
    assert(range::empty(range2));
}

void check_5_hello_overtyped(python_range<int, std::string, char, double> range)
{
    assert(!range::empty(range));
    assert(range::first(range) == 5);
    auto range2 = range::drop(std::move(range));
    std::string s = range::first(range2);
    assert(s == "hello");
    auto range3 = range::drop(std::move(range2));
    assert(range::empty(range3));
}

// List of tuples, i.e. nested python_range's.
void check_hello_5_bye_27(python_range<python_range<std::string, int>> r)
{
    python_range<std::string, int> element = range::first(r);
    assert(range::first(element) == "hello");
    assert(range::first(range::drop(std::move(element))) == 5);

    r = range::drop(std::move(r));
    element = range::first(r);
    assert(range::first(element) == "bye");
    assert(range::first(range::drop(std::move(element))) == 27);

    r = range::drop(std::move(r));
    assert(range::empty(r));
}

// Check that None can be an element of the range without problem.
void check_17_None_hi(python_range<> r)
{
    assert(range::chop_in_place(r).equal(nanobind::int_(17)));
    assert(range::chop_in_place(r).is_none());
    assert(range::chop_in_place(r).equal(nanobind::str("hi")));
    assert(range::empty(r));
}

nanobind::object test_return_something() { return nanobind::int_(1); }

NB_MODULE(python_range_example, m)
{
    m.def("check_empty", check_empty<python_range<>>);

    m.def("check_empty_2", check_empty<python_range<double>>);

    m.def("check_6_25_8_5", check_6_25_8_5<python_range<double>>);
    m.def("check_6_25_8_5_chop", check_6_25_8_5_chop<python_range<double>>);
    m.def(
        "check_6_25_8_5_chop_in_place",
        check_6_25_8_5_chop_in_place<python_range<double>>);

    m.def("check_5_hello_untyped", check_5_hello_untyped);
    m.def("check_5_hello_typed", check_5_hello_typed);
    m.def("check_5_hello_overtyped", check_5_hello_overtyped);

    m.def("check_hello_5_bye_27", check_hello_5_bye_27);

    m.def("check_17_None_hi", check_17_None_hi);

    m.def("test_return_something", test_return_something);
}
