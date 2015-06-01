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

#define BOOST_TEST_MODULE test_range_function_range
#include "utility/test/boost_unit_test.hpp"

#include "range/function_range.hpp"

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/assert.hpp>


BOOST_AUTO_TEST_SUITE(test_suite_function_range)

int count() {
    static int i = 0;
    return ++ i;
}

int & count_reference() {
    static int i = 0;
    ++ i;
    return i;
}

BOOST_AUTO_TEST_CASE (test_range_function_range) {
    typedef range::function_range <int ()> f_range;
    auto r_temp = range::make_function_range (count);
    // f_range r_temp (&count);
    f_range r (std::move (r_temp));

    BOOST_MPL_ASSERT ((range::never_empty <direction::front, f_range>));

    range::chopped <int, f_range> next = range::chop (std::move (r));
    BOOST_CHECK_EQUAL (next.first(), 1);
    range::chopped <int, f_range> next2 = range::chop (next.forward_rest());
    BOOST_CHECK_EQUAL (next2.first(), 2);

    auto r2 = next2.move_rest();
    // drop() should be automatically implemented for rvalues.
    r2 = range::drop (std::move (r2));
    BOOST_CHECK_EQUAL (range::chop_in_place (r2), 4);
    BOOST_CHECK_EQUAL (range::chop_in_place (r2), 5);
    BOOST_CHECK_EQUAL (range::first (std::move (r2)), 6);
}

BOOST_AUTO_TEST_CASE (test_range_function_range_reference) {
    typedef range::function_range <int & (*)()> f_range;
    f_range r_temp (count_reference);
    f_range r (std::move (r_temp));

    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <f_range>::type, range::function_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <f_range &&>::type, range::function_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <f_range &>::type, range::function_range_tag>));

    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <f_range const>::type, range::function_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <f_range const &&>::type, range::function_range_tag>));
    BOOST_MPL_ASSERT ((std::is_same <
        range::tag_of <f_range const &>::type, range::function_range_tag>));

    BOOST_MPL_ASSERT ((range::has <range::callable::chop (f_range)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::chop (f_range &&)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::chop (f_range &)>));

    BOOST_MPL_ASSERT ((range::has <range::callable::chop (f_range const)>));
    BOOST_MPL_ASSERT_NOT ((
        range::has <range::callable::chop (f_range const &)>));

    range::chopped <int &, f_range> next = range::chop (std::move (r));
    BOOST_CHECK_EQUAL (next.first(), 1);
    auto next2 = range::chop (next.forward_rest());
    BOOST_CHECK_EQUAL (next2.first(), 2);

    // Secretly change the static variable in count_reference.
    // next.first() must be a reference, so this should be fine.
    next.first() = 5;
    auto next3 = range::chop (std::move (next2.rest()));
    BOOST_CHECK_EQUAL (next3.first(), 6);

    auto r2 = next3.forward_rest();
    int i = range::chop_in_place (r2);
    BOOST_CHECK_EQUAL (i, 7);
    i = range::chop_in_place (r2);
    BOOST_CHECK_EQUAL (i, 8);
}

BOOST_AUTO_TEST_SUITE_END()
