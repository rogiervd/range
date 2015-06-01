/*
Copyright 2014 Rogier van Dalen.

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

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "range/python/range.hpp"

using range::python_range;

void test_static() {
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <python_range <>>::type, range::python_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <python_range <double> &>::type,
        range::python_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <python_range <double> const &>::type,
        range::python_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <python_range <double, int> const &&>::type,
        range::python_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <python_range <double, int, float> &&>::type,
        range::python_range_tag>));

    BOOST_MPL_ASSERT ((range::has <
        range::callable::empty (range::python_range <int>)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::empty (range::python_range <int> const &)>));

    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::size (range::python_range <int> &)>));

    BOOST_MPL_ASSERT ((range::has <
        range::callable::first (range::python_range <int>)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::first (range::python_range <int> &)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::first (range::python_range <int> const &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::first (direction::back, range::python_range <int>)>));

    // drop: only for rvalues.
    BOOST_MPL_ASSERT ((range::has <
        range::callable::drop (range::python_range <int>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::drop (range::python_range <int> &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::drop (range::python_range <int> const &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::drop (direction::back, range::python_range <int>)>));

    // chop: only for rvalues.
    BOOST_MPL_ASSERT ((range::has <
        range::callable::chop (range::python_range <int>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::chop (range::python_range <int> &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::chop (range::python_range <int> const &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::chop (direction::back, range::python_range <int>)>));

    // chop_in_place: only for lvalue references.
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::chop_in_place (range::python_range <int>)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::chop_in_place (range::python_range <int> &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::chop_in_place (range::python_range <int> const &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::chop_in_place (
            direction::back, range::python_range <int> &)>));
    // But only for homogeneous ranges.
    BOOST_MPL_ASSERT ((range::has <
        range::callable::chop_in_place (range::python_range <> &)>));
    BOOST_MPL_ASSERT ((range::has <
        range::callable::chop_in_place (range::python_range <int> &)>));
    BOOST_MPL_ASSERT_NOT ((range::has <
        range::callable::chop_in_place (range::python_range <int, double> &)>));

    // Return types.
    // first.
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::first (range::python_range<>)>::type,
        boost::python::object>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::first (range::python_range <int>)>::type,
        int>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::first (range::python_range <double, char>)>::type,
        double>));

    // drop.
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::drop (range::python_range<>)>::type,
        range::python_range<>>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::drop (range::python_range <int>)>::type,
        range::python_range <int>>));
    BOOST_MPL_ASSERT ((std::is_same <range::result_of <
        range::callable::drop (range::python_range <double, char>)>::type,
        range::python_range <char>>));
}

template <class Range> void check_empty (Range range)
{ assert (range::empty (range)); }

template <class DoubleRange>
    void check_6_25_8_5 (DoubleRange double_range)
{
    assert (range::first (double_range) == 6.25);
    double_range = range::drop (std::move (double_range));
    assert (!range::empty (double_range));
    assert (range::first (double_range) == 8.5);
    double_range = range::drop (std::move (double_range));
    assert (range::empty (double_range));
    assert (range::empty (double_range));
    assert (range::empty (double_range));
}

template <class DoubleRange>
    void check_6_25_8_5_chop (DoubleRange double_range)
{
    assert (range::chop_in_place (double_range) == 6.25);
    assert (!range::empty (double_range));
    assert (range::chop_in_place (double_range) == 8.5);
    assert (range::empty (double_range));
}

template <class DoubleRange>
    void check_6_25_8_5_chop_in_place (DoubleRange double_range)
{
    auto next = range::chop (std::move (double_range));
    assert (next.first() == 6.25);
    assert (!range::empty (next.rest()));
    next = range::chop (std::move (next.rest()));
    assert (next.first() == 8.5);
    assert (range::empty (next.rest()));
}

void check_5_hello_untyped (python_range<> range) {
    assert (!range::empty (range));
    assert (boost::python::extract <int> (range::first (range)) == 5);
    auto range2 = range::drop (std::move (range));
    std::string s = boost::python::extract <std::string> (
        range::first (range2));
    assert (s == "hello");
    // This should be possible: at the end of the type sequence, the last
    // element (here, std::string) gets repeated forever.
    range2 = range::drop (std::move (range2));
    assert (range::empty (range2));
}

void check_5_hello_typed (python_range <int, std::string> range) {
    assert (!range::empty (range));
    assert (range::first (range) == 5);
    auto range2 = range::drop (std::move (range));
    auto second = range::chop (std::move (range2));
    assert (second.first() == "hello");
    // This should be possible: at the end of the type sequence, the last
    // element (here, std::string) gets repeated forever.
    range2 = std::move (second.rest());
    assert (range::empty (range2));
}

void check_5_hello_overtyped (
    python_range <int, std::string, char, double> range)
{
    assert (!range::empty (range));
    assert (range::first (range) == 5);
    auto range2 = range::drop (std::move (range));
    std::string s = range::first (range2);
    assert (s == "hello");
    auto range3 = range::drop (std::move (range2));
    assert (range::empty (range3));
}

// List of tuples, i.e. nested python_range's.
void check_hello_5_bye_27 (
    python_range <python_range <std::string, int>> r)
{
    python_range <std::string, int> element = range::first (r);
    assert (range::first (element) == "hello");
    assert (range::first (range::drop (std::move (element))) == 5);

    r = range::drop (std::move (r));
    element = range::first (r);
    assert (range::first (element) == "bye");
    assert (range::first (range::drop (std::move (element))) == 27);

    r = range::drop (std::move (r));
    assert (range::empty (r));
}

// Check that None can be an element of the range without problem.
void check_17_None_hi (python_range<> r) {
    using boost::python::object;
    assert (range::chop_in_place (r) == object (17));
    assert (range::chop_in_place (r) == object());
    assert (range::chop_in_place (r) == object("hi"));
    assert (range::empty (r));
}

boost::python::handle<> test_return_something() {
    boost::python::object o (1);
    return boost::python::handle<> (boost::python::incref (o.ptr()));
}

BOOST_PYTHON_MODULE (python_range_example) {
    using namespace boost::python;

    range::python::convert_object_to_range <python_range <>>();
    range::python::convert_object_to_range <python_range <double>>();
    range::python::convert_object_to_range <python_range <int, std::string>>();
    range::python::convert_object_to_range <
        python_range <int, std::string, char, double>>();

    range::python::convert_object_to_range <python_range <std::string, int>>();
    range::python::convert_object_to_range <
        python_range <python_range <std::string, int>>>();

    def <void (python_range<>)> ("check_empty", check_empty);

    def <void (python_range <double>)> ("check_empty_2", check_empty);

    def <void (python_range <double>)> ("check_6_25_8_5", check_6_25_8_5);
    def <void (python_range <double>)> (
        "check_6_25_8_5_chop", check_6_25_8_5_chop);
    def <void (python_range <double>)> (
        "check_6_25_8_5_chop_in_place", check_6_25_8_5_chop_in_place);

    def ("check_5_hello_untyped", check_5_hello_untyped);
    def ("check_5_hello_typed", check_5_hello_typed);
    def ("check_5_hello_overtyped", check_5_hello_overtyped);

    def ("check_hello_5_bye_27", check_hello_5_bye_27);

    def ("check_17_None_hi", check_17_None_hi);

    def ("test_return_something", test_return_something);
}
