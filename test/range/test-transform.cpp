/*
Copyright 2013, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_transform
#include "utility/test/boost_unit_test.hpp"

#include "range/transform.hpp"

#include <type_traits>
#include <vector>
#include <list>
#include <tuple>

#include <boost/mpl/assert.hpp>

#include "range/std.hpp"
#include "range/for_each_macro.hpp"

#include "rime/check/check_equal.hpp"

#include "weird_count.hpp"
#include "unique_range.hpp"

/**
Range of natural numbers starting at 1.
Only provides chop_in_place.
*/
struct simple_count {
    int i;

    simple_count() : i (0) {}

    rime::false_type empty (direction::front) const { return rime::false_; }

    int chop_in_place (direction::front) { return i ++; }
};

struct simple_count_tag {};

namespace range {

    template <> struct tag_of_qualified <simple_count>
    { typedef simple_count_tag type; };

} // namespace range

BOOST_AUTO_TEST_SUITE(test_range_transform)

using range::transform;
using range::front;
using range::back;

using range::default_direction;
using range::empty;
using range::size;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

using range::second;
using range::at;

using range::is_homogeneous;

struct callable_twice {
    template <class Argument>
        Argument operator() (Argument const & argument) const
    { return argument + argument; }
};

callable_twice twice;

struct callable_duplicate {
    template <class Argument>
        std::tuple <Argument, Argument>
            operator() (Argument const & argument) const
    { return std::make_tuple (argument, argument); }
};

callable_duplicate duplicate;

struct callable_point {
    template <class Argument>
        typename std::add_pointer <Argument>::type
            operator() (Argument && argument) const
    { return &argument; }
};
// \return Pointer to the argument.
callable_point point;

BOOST_AUTO_TEST_CASE (example) {
    std::vector <int> v;
    v.push_back (5);
    v.push_back (7);
    {
        int count = 0;
        RANGE_FOR_EACH (i, v)
            count += i;
        BOOST_CHECK_EQUAL (count, 12);
    }
    {
        int count = 0;
        RANGE_FOR_EACH (i, transform (v, twice))
            count += i;
        BOOST_CHECK_EQUAL (count, 24);
    }
}

BOOST_AUTO_TEST_CASE (test_range_transform) {
    {
        std::tuple <> t;
        auto v = transform (t, duplicate);
        auto direction = default_direction (v);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (direction), direction::front>));
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (v)>));

        RIME_CHECK_EQUAL (empty (v), rime::true_);
        RIME_CHECK_EQUAL (size (v), rime::size_t <0u>());

        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::first (decltype (v))>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::drop (decltype (v))>));
        BOOST_MPL_ASSERT_NOT ((
            range::has <range::callable::chop (decltype (v))>));
    }
    {
        std::tuple <int> t (7);
        auto v = transform (t, duplicate);
        BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (v)>));
        // v should have elements:
        // std::tuple <int, int> (7, 7)

        RIME_CHECK_EQUAL (empty (v), rime::false_);
        RIME_CHECK_EQUAL (size (v), rime::size_t <1u>());

        BOOST_MPL_ASSERT ((range::has <range::callable::first (decltype (v))>));
        BOOST_MPL_ASSERT ((range::has <range::callable::drop (decltype (v))>));

        auto f = range::first (v);
        BOOST_MPL_ASSERT ((std::is_same <decltype (f), std::tuple <int, int>>));
        BOOST_CHECK_EQUAL (first (f), 7);
        BOOST_CHECK_EQUAL (second (f), 7);

        RIME_CHECK_EQUAL (empty (drop (v)), rime::true_);

        auto chopped = chop (v);
        static_assert (std::is_same <decltype (chopped.first()),
            std::tuple <int, int> const &>::value, "");
        BOOST_CHECK_EQUAL (first (chopped.first()), 7);
        BOOST_CHECK_EQUAL (second (chopped.first()), 7);
        RIME_CHECK_EQUAL (empty (chopped.rest()), rime::true_);
    }
    {
        std::tuple <int, char, double> t (7, 'a', 9.25);
        {
            auto v = transform (t, duplicate);
            BOOST_MPL_ASSERT_NOT ((is_homogeneous <decltype (v)>));
            // v should have elements:
            // std::tuple <int, int> (7, 7)
            // std::tuple <char, char> ('a', 'a')
            // std::tuple <double, double> (9.25, 9.25)

            RIME_CHECK_EQUAL (empty (v), rime::false_);
            RIME_CHECK_EQUAL (size (v), rime::size_t <3u>());

            BOOST_MPL_ASSERT ((
                range::has <range::callable::first (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (decltype (v))>));

            auto e1 = range::first (v);
            BOOST_MPL_ASSERT ((
                std::is_same <decltype (e1), std::tuple <int, int>>));
            BOOST_CHECK_EQUAL (first (e1), 7);
            BOOST_CHECK_EQUAL (second (e1), 7);

            auto e2 = range::first (drop (v));
            BOOST_MPL_ASSERT ((
                std::is_same <decltype (e2), std::tuple <char, char>>));
            BOOST_CHECK_EQUAL (first (e2), 'a');
            BOOST_CHECK_EQUAL (second (e2), 'a');

            auto e3 = range::first (drop (v, rime::size_t <2>()));
            BOOST_MPL_ASSERT ((
                std::is_same <decltype (e3), std::tuple <double, double>>));
            BOOST_CHECK_EQUAL (first (e3), 9.25);
            BOOST_CHECK_EQUAL (second (e3), 9.25);

            RIME_CHECK_EQUAL (
                empty (drop (v, rime::size_t <3>())), rime::true_);

            // Test chop.
            auto chopped1 = chop (v);
            static_assert (std::is_same <decltype (chopped1.first()),
                std::tuple <int, int> const &>::value, "");
            BOOST_CHECK_EQUAL (first (chopped1.first()), 7);
            BOOST_CHECK_EQUAL (second (chopped1.first()), 7);
            RIME_CHECK_EQUAL (empty (chopped1.rest()), rime::false_);

            auto chopped2 = chop (chopped1.rest());
            static_assert (std::is_same <decltype (chopped2.first()),
                std::tuple <char, char> const &>::value, "");
            BOOST_CHECK_EQUAL (first (chopped2.first()), 'a');
            BOOST_CHECK_EQUAL (second (chopped2.first()), 'a');
            RIME_CHECK_EQUAL (empty (chopped2.rest()), rime::false_);

            auto chopped3 = chop (chopped2.rest());
            static_assert (std::is_same <decltype (chopped3.first()),
                std::tuple <double, double> const &>::value, "");
            BOOST_CHECK_EQUAL (first (chopped3.first()), 9.25);
            BOOST_CHECK_EQUAL (second (chopped3.first()), 9.25);
            RIME_CHECK_EQUAL (empty (chopped3.rest()), rime::true_);
        }
        {
            /*
            Convert the tuple into a range of pointers to its element.
            Note that this is pretty neat.
            Conceptually, just because "point" is defined to return pointers,
            suddently v has a type equivalent to
                tuple <int *, char *, double *>
            but then on-the-fly.
            */
            auto v = transform (t, point);

            // Should be pointing at the actual element.
            BOOST_CHECK_EQUAL (first (v), &first (t));

            // Change original element through transform_view.
            // (This may be bad form, but it is supported.)
            *at (v, rime::size_t <2>()) = 4.5;
            BOOST_CHECK_EQUAL (first (t, back), 4.5);
        }
    }
}

BOOST_AUTO_TEST_CASE (test_range_transform_homogeneous) {
    {
        std::vector <double> c;
        c.push_back (6);
        c.push_back (10.5);
        c.push_back (-8);
        {
            auto v = transform (c, twice);
            BOOST_MPL_ASSERT ((is_homogeneous <decltype (v)>));
            // v should have elements 12, 21, -16.

            BOOST_MPL_ASSERT ((
                range::has <range::callable::empty (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::size (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::first (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (decltype (v), int)>));

            BOOST_CHECK (!empty (v));
            BOOST_CHECK_EQUAL (size (v), 3u);

            BOOST_CHECK_EQUAL (first (v), 12.);
            BOOST_CHECK_EQUAL (at (v, 1), 21.);
            BOOST_CHECK_EQUAL (at (v, 2), -16.);
            BOOST_CHECK_EQUAL (first (v, back), -16.);
            BOOST_CHECK_EQUAL (at (v, 1, back), 21.);
            BOOST_CHECK_EQUAL (at (v, 2, back), 12.);

            auto chopped1 = chop (v);
            BOOST_CHECK_EQUAL (chopped1.first(), 12);
            auto chopped2 = chop (chopped1.rest());
            BOOST_CHECK_EQUAL (chopped2.first(), 21);
            auto chopped3 = chop (chopped2.rest());
            BOOST_CHECK_EQUAL (chopped3.first(), -16);
            BOOST_CHECK (empty (chopped3.rest()));

            // Check that the transform view is assignable.
            v = drop (v);
            BOOST_CHECK_EQUAL (first (v), 21.);
            v = drop (v);
            BOOST_CHECK_EQUAL (first (v), -16.);
            v = drop (v);
            BOOST_CHECK (empty (v));
        }
        {
            // Transform the container into a range of pointers to elements.
            auto v = transform (c, point);

            BOOST_CHECK_EQUAL (first (v), &first (c));
            BOOST_CHECK_EQUAL (at (v, 1), &at (c, 1));

            *first (v) = 27.5;
            BOOST_CHECK_EQUAL (first (c), 27.5);
        }
    }

    // std::list.
    {
        std::list <double> c;
        c.push_back (6);
        c.push_back (10.5);
        c.push_back (-8);
        {
            auto v = transform (c, twice);
            BOOST_MPL_ASSERT ((is_homogeneous <decltype (v)>));
            // v should have elements 12, 21, -16.

            BOOST_MPL_ASSERT ((
                range::has <range::callable::empty (decltype (v))>));
            BOOST_MPL_ASSERT_NOT ((
                range::has <range::callable::size (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::first (decltype (v))>));
            BOOST_MPL_ASSERT ((
                range::has <range::callable::drop (decltype (v))>));
            BOOST_MPL_ASSERT_NOT ((
                range::has <range::callable::drop (decltype (v), int)>));

            BOOST_CHECK (!empty (v));

            BOOST_CHECK_EQUAL (first (v), 12.);
            BOOST_CHECK_EQUAL (first (drop (v)), 21.);
            BOOST_CHECK_EQUAL (first (drop (drop (v))), -16.);
            BOOST_CHECK_EQUAL (first (v, back), -16.);
            BOOST_CHECK_EQUAL (first (drop (v, back), back), 21.);
            BOOST_CHECK_EQUAL (first (drop (drop (v, back), back), back), 12.);
        }
    }

    // Stacked transforms.
    {
        std::vector <double> c;
        c.push_back (6);
        c.push_back (10.5);
        c.push_back (-8);

        auto v = transform (transform (c, twice), duplicate);
        // v should contain (12, 12), (21, 21), (-16, -16).

        BOOST_CHECK_EQUAL (size (v), 3u);
        BOOST_CHECK (first (v) == std::make_tuple (12., 12.));
        BOOST_CHECK (first (drop (v)) == std::make_tuple (21., 21.));
        BOOST_CHECK (first (v, back) == std::make_tuple (-16., -16.));

        auto chopped1 = chop (v);
        BOOST_CHECK (chopped1.first() == std::make_tuple (12., 12.));
        auto chopped2 = chop (chopped1.rest());
        BOOST_CHECK (chopped2.first() == std::make_tuple (21., 21.));
        auto chopped3 = chop (chopped2.rest());
        BOOST_CHECK (chopped3.first() == std::make_tuple (-16., -16.));
        BOOST_CHECK (empty (chopped3.rest()));
    }
}

BOOST_AUTO_TEST_CASE (test_range_transform_weird_count) {
    {
        weird_count w;
        weird_direction direction (7);

        auto v = transform (w, twice, weird_direction (7));

        BOOST_MPL_ASSERT ((std::is_same <range::result_of <
                range::callable::default_direction (decltype (v))>::type,
            forgotten_to_define_direction>));

        BOOST_CHECK (!empty (v, direction));
        BOOST_MPL_ASSERT_NOT ((range::has <
            range::callable::size (weird_direction, decltype (v))>));

        BOOST_CHECK_EQUAL (first (v, direction), 0);
        BOOST_CHECK_EQUAL (first (drop (v, direction), direction), 2);
        BOOST_CHECK_EQUAL (first (drop (v, 5, direction), direction), 10);
    }
    // Should work with views as well.
    {
        weird_count w;
        weird_direction direction (7);

        auto view = range::view (w, direction);
        auto transformed = transform (view, duplicate, direction);

        BOOST_CHECK (!empty (transformed, direction));

        BOOST_CHECK (first (transformed, direction) == std::make_tuple (0, 0));
        BOOST_CHECK (first (drop (transformed, 2, direction), direction)
            == std::make_tuple (2, 2));
    }
}

template <class Type> struct show_type;

BOOST_AUTO_TEST_CASE (unique_underlying) {
    std::vector <int> v;
    v.push_back (6);
    v.push_back (20);
    v.push_back (-5);

    {
        auto t = transform (unique_view (v), twice);

        BOOST_CHECK_EQUAL (first (t), 12);
        t = drop (std::move (t));
        BOOST_CHECK_EQUAL (first (t), 40);
        t = drop (std::move (t));
        BOOST_CHECK_EQUAL (first (t), -10);
        t = drop (std::move (t));
        BOOST_CHECK (empty (t));
    }
    {
        auto t = transform (one_time_view (v), twice);

        // Chop only available for rvalue references.
        static_assert (range::has <range::callable::chop (decltype (t))
            >::value, "");
        static_assert (!range::has <range::callable::chop (decltype (t) const &)
            >::value, "");

        auto chopped1 = chop (std::move (t));
        BOOST_CHECK_EQUAL (chopped1.first(), 12);
        auto chopped2 = chop (chopped1.move_rest());
        BOOST_CHECK_EQUAL (chopped2.first(), 40);
        // first() is available as long as it is the last call.
        BOOST_CHECK_EQUAL (first (chopped2.move_rest()), -10);
    }
}

BOOST_AUTO_TEST_CASE (only_chop_in_place) {
    {
        simple_count c;
        int zero = chop_in_place (c);
        BOOST_CHECK_EQUAL (zero, 0);
        int one = chop_in_place (c);
        BOOST_CHECK_EQUAL (one, 1);
    }
    {
        auto even = transform (simple_count(), twice);
        int zero = chop_in_place (even);
        BOOST_CHECK_EQUAL (zero, 0);
        int two = chop_in_place (even);
        BOOST_CHECK_EQUAL (two, 2);
        int four = chop_in_place (even);
        BOOST_CHECK_EQUAL (four, 4);
        int six = chop_in_place (even);
        BOOST_CHECK_EQUAL (six, 6);
    }
}

/**
Round numbers up to a step size referenced.
Not considered or tested on negative numbers.
*/
class round_up {
    int const & step;
public:
    round_up (int const & step) : step (step) {}

    int operator() (int n) const {
        return ((n + (step - 1)) / step) * step;
    }
};

BOOST_AUTO_TEST_CASE (function_with_reference) {
    int step = 5;
    round_up round (step);

    // Test "round_up".
    BOOST_CHECK_EQUAL (round (0), 0);
    BOOST_CHECK_EQUAL (round (1), 5);
    BOOST_CHECK_EQUAL (round (4), 5);
    BOOST_CHECK_EQUAL (round (5), 5);
    BOOST_CHECK_EQUAL (round (23), 25);

    // Change step size.
    step = 3;
    BOOST_CHECK_EQUAL (round (7), 9);

    std::vector <int> v;
    v.push_back (1);
    v.push_back (5);
    v.push_back (10);
    v.push_back (27);

    {
        auto rounded = transform (v, round);

        BOOST_CHECK_EQUAL (first (rounded), 3);
        rounded = drop (rounded);
        BOOST_CHECK_EQUAL (first (rounded), 6);
        rounded = drop (rounded);

        step = 7;
        int fourteen = chop_in_place (rounded);
        BOOST_CHECK_EQUAL (fourteen, 14);

        auto chopped = chop (rounded);
        BOOST_CHECK_EQUAL (chopped.first(), 28);
        BOOST_CHECK (empty (chopped.rest()));
    }
    {
        // With one_time_view.
        auto rounded = transform (one_time_view (v), round);

        step = 4;

        auto four = chop_in_place (rounded);
        BOOST_CHECK_EQUAL (four, 4);

        auto chopped = chop (std::move (rounded));
        BOOST_CHECK_EQUAL (chopped.first(), 8);
    }
}

BOOST_AUTO_TEST_SUITE_END()
