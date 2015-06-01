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

#define BOOST_TEST_MODULE test_range_scan
#include "utility/test/boost_unit_test.hpp"

#include "range/scan.hpp"

#include <vector>
#include <list>
#include <tuple>

#include <boost/optional.hpp>

#include "utility/returns.hpp"

#include "rime/check/check_equal.hpp"

#include "range/std.hpp"

#include "weird_count.hpp"
#include "unique_range.hpp"

BOOST_AUTO_TEST_SUITE(test_range_scan)

using range::scan;

using range::empty;
using range::size;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

using range::is_homogeneous;
using range::has;
namespace callable = range::callable;

struct plus {
    template <class Left, class Right>
        auto operator() (Left const & left, Right const & right) const
    RETURNS (left + right);
};

struct reference_right {
    template <class Left, class Right>
        Right & operator() (Left const & left, Right & right) const
    { return right; }
};

BOOST_AUTO_TEST_CASE (test_scan_homogeneous) {
    std::vector <int> v;
    {
        auto acc = scan (plus(), 0, v);
        static_assert (is_homogeneous <decltype (acc)>::value, "");

        RIME_CHECK_EQUAL (empty (acc), false);
        BOOST_CHECK_EQUAL (size (acc), 1);
        BOOST_CHECK_EQUAL (first (acc), 0);
        BOOST_CHECK (empty (drop (acc)));
    }

    v.push_back (1);
    {
        // Note the use of one_time_view to test whether everything is moved
        // properly when necessary.
        auto acc = scan (plus(), 0, one_time_view (v));
        RIME_CHECK_EQUAL (empty (acc), false);
        BOOST_CHECK_EQUAL (size (acc), 2);
        BOOST_CHECK_EQUAL (first (acc), 0);

        auto acc2 = drop (std::move (acc));
        BOOST_CHECK (!empty (acc2));
        BOOST_CHECK_EQUAL (first (acc2), 1);
        BOOST_CHECK (empty (drop (std::move (acc2))));
    }

    v.push_back (2);
    {
        auto acc = scan (plus(), 0, v);
        RIME_CHECK_EQUAL (empty (acc), false);
        BOOST_CHECK_EQUAL (size (acc), 3);
        BOOST_CHECK_EQUAL (first (acc), 0);

        auto chopped2 = chop (acc);
        BOOST_CHECK (!empty (chopped2.rest()));
        BOOST_CHECK_EQUAL (first (chopped2.rest()), 1);

        auto acc3 = drop (chopped2.rest());
        BOOST_CHECK (!empty (acc3));
        BOOST_CHECK_EQUAL (first (acc3), 3);
        BOOST_CHECK (empty (drop (acc3)));

        // chop_in_place.
        chop_in_place (acc);
        BOOST_CHECK_EQUAL (size (acc), 2);
        BOOST_CHECK_EQUAL (first (acc), 1);
    }
    // The same, but from the back.
    {
        auto acc = scan (range::back, plus(), 0, v);
        RIME_CHECK_EQUAL (range::default_direction (acc) == range::back,
            rime::true_);

        RIME_CHECK_EQUAL (empty (acc), false);
        BOOST_CHECK_EQUAL (size (acc), 3);
        BOOST_CHECK_EQUAL (first (acc), 0);

        auto chopped2 = chop (acc);
        BOOST_CHECK (!empty (chopped2.rest()));
        BOOST_CHECK_EQUAL (first (chopped2.rest()), 2);

        auto acc3 = drop (chopped2.rest());
        BOOST_CHECK (!empty (acc3));
        BOOST_CHECK_EQUAL (first (acc3), 3);
        BOOST_CHECK (empty (drop (acc3)));

        // chop_in_place.
        chop_in_place (acc);
        BOOST_CHECK_EQUAL (size (acc), 2);
        BOOST_CHECK_EQUAL (first (acc), 2);
    }

    // Reference return type.
    {
        int i = 0;
        auto acc = scan (reference_right(), i, v);
        static_assert (std::is_same <decltype (first (acc)), int &>::value, "");
        BOOST_CHECK_EQUAL (&first (acc), &i);

        auto chopped = chop (acc);
        BOOST_CHECK_EQUAL (&chopped.first(), &i);
        BOOST_CHECK_EQUAL (&first (chopped.rest()), &v[0]);

        acc = drop (chopped.rest());
        BOOST_CHECK_EQUAL (&first (acc), &v[1]);

        int & last = chop_in_place (acc);
        BOOST_CHECK_EQUAL (&last, &v[1]);
        BOOST_CHECK (empty (acc));
    }
    {
        // Only with chop_in_place.
        int i = 0;
        auto acc = scan (reference_right(), i, v);
        int & element0 = chop_in_place (acc);
        BOOST_CHECK_EQUAL (&element0, &i);
        int & element1 = chop_in_place (acc);
        BOOST_CHECK_EQUAL (&element1, &v[0]);
        int & element2 = chop_in_place (acc);
        BOOST_CHECK_EQUAL (&element2, &v[1]);
        BOOST_CHECK (empty (acc));
    }
    {
        // Only with chop_in_place; and with one_time_view.
        int i = 0;
        auto acc = scan (reference_right(), i, one_time_view (v));
        int & element0 = chop_in_place (acc);
        BOOST_CHECK_EQUAL (&element0, &i);
        int & element1 = chop_in_place (acc);
        BOOST_CHECK_EQUAL (&element1, &v[0]);
        int & element2 = chop_in_place (acc);
        BOOST_CHECK_EQUAL (&element2, &v[1]);
        BOOST_CHECK (empty (acc));
    }

    std::list <int> l;
    {
        auto acc = scan (plus(), 0, l);
        static_assert (!has <callable::size (decltype (acc))>::value,
            "Scan over forward range cannot implement size.");
    }
}

BOOST_AUTO_TEST_CASE (test_scan_heterogeneous) {
    {
        std::tuple<> t;
        auto acc = scan (plus(), 0, t);
        RIME_CHECK_EQUAL (empty (acc), rime::false_);
        RIME_CHECK_EQUAL (size (acc), rime::size_t <1>());

        BOOST_CHECK_EQUAL (first (acc), 0);
        BOOST_CHECK (empty (drop (acc)));
    }

    {
        std::tuple <int> t (1);
        auto acc = scan (plus(), 0, t);
        BOOST_CHECK (!empty (acc));
        RIME_CHECK_EQUAL (size (acc), rime::size_t <2>());
        BOOST_CHECK_EQUAL (first (acc), 0);

        auto acc2 = drop (acc);
        BOOST_CHECK (!empty (acc2));
        BOOST_CHECK_EQUAL (first (acc2), 1);
        BOOST_CHECK (empty (drop (acc2)));
    }

    {
        std::tuple <int, double> t (1, 2.0);
        auto acc = scan (plus(), 0, t);
        BOOST_CHECK (!empty (acc));
        RIME_CHECK_EQUAL (size (acc), rime::size_t <3>());
        BOOST_CHECK_EQUAL (first (acc), 0);

        auto acc2 = drop (acc);
        BOOST_CHECK (!empty (acc2));
        BOOST_CHECK_EQUAL (first (acc2), 1);

        auto acc3 = drop (acc2);
        BOOST_CHECK (!empty (acc3));
        RIME_CHECK_EQUAL (first (acc3), 3.0);
        BOOST_CHECK (empty (drop (acc3)));
    }
}

BOOST_AUTO_TEST_CASE (scan_weird_count) {
    weird_direction direction (7);

    auto s = scan (direction, plus(), 0, weird_count (4));

    BOOST_CHECK_EQUAL (first (direction, s), 0);
    s = drop (direction, s);
    BOOST_CHECK_EQUAL (first (direction, s), 4);
    s = drop (direction, s);
    BOOST_CHECK_EQUAL (first (direction, s), 9);
    s = drop (direction, s);
    BOOST_CHECK_EQUAL (first (direction, s), 15);
}


BOOST_AUTO_TEST_CASE (scan_unique_range) {
    std::vector <int> v;
    {
        auto acc = scan (plus(), 0, one_time_view (v));
        static_assert (is_homogeneous <decltype (acc)>::value, "");

        RIME_CHECK_EQUAL (empty (acc), false);
        BOOST_CHECK_EQUAL (size (acc), 1);


        static_assert (has <callable::first (decltype (acc) &&)>::value, "");
        static_assert (
            has <callable::first (decltype (acc) const &)>::value, "");

        // drop and chop only available for rvalue references.
        static_assert (has <callable::drop (decltype (acc) &&)>::value, "");
        static_assert (has <callable::chop (decltype (acc) &&)>::value, "");
        static_assert (
            !has <callable::drop (decltype (acc) const &)>::value, "");
        static_assert (
            !has <callable::chop (decltype (acc) const &)>::value, "");

        auto chopped = chop (std::move (acc));
        BOOST_CHECK_EQUAL (chopped.first(), 0);
        BOOST_CHECK (empty (chopped.rest()));
    }

    v.push_back (1);
    v.push_back (7);
    {
        auto acc = scan (plus(), 0, unique_view (v));
        static_assert (is_homogeneous <decltype (acc)>::value, "");

        RIME_CHECK_EQUAL (empty (acc), false);
        BOOST_CHECK_EQUAL (size (acc), 3);

        auto first = chop_in_place (acc);
        RIME_CHECK_EQUAL (first, 0);
        BOOST_CHECK_EQUAL (size (acc), 2);

        auto chopped = chop (std::move (acc));
        RIME_CHECK_EQUAL (chopped.first(), 1);
        BOOST_CHECK_EQUAL (size (chopped.rest()), 1);

        auto acc3 = drop (chopped.move_rest());
        BOOST_CHECK (empty (acc3));
    }
}

/* Use case: simple sequence of parsers. */

typedef range::iterator_range <std::string::const_iterator> input_type;

/**
Intermediate state of a sequence of parsers.
Contains the result of the single parsers, if it succeeded, and the rest of the
input.
*/
template <class Type> struct parse_outcome {
    boost::optional <Type> output;
    input_type rest;

    parse_outcome (input_type const & rest)
    : output(), rest (rest) {}

    parse_outcome (Type const & output, input_type const & rest)
    : output (output), rest (rest) {}

    bool success() const { return !!output; }
};

template <> struct parse_outcome <void> {
    bool success_;
    input_type rest;

    parse_outcome (input_type const & rest)
    : success_ (false), rest (rest) {}

    parse_outcome (bool success, input_type const & rest)
    : success_ (success), rest (rest) {}

    bool success() const { return success_; }
};

struct any_char_parser {};
struct char_parser {
    char c;
    char_parser (char c) : c (c) {}
};

struct parse {

    template <class LastOutput, class Result = parse_outcome <char>>
    Result operator() (parse_outcome <LastOutput> const & state,
        any_char_parser) const
    {
        if (range::empty (state.rest))
            return Result (state.rest);
        return Result (first (state.rest), drop (state.rest));
    }

    template <class LastOutput, class Result = parse_outcome <void>>
    Result operator() (parse_outcome <LastOutput> const & state,
        char_parser const parser) const
    {
        if (range::empty (state.rest) || range::first (state.rest) != parser.c)
            return Result (state.rest);
        return Result (true, drop (state.rest));
    }

};

BOOST_AUTO_TEST_CASE (test_scan_parser) {
    // Any XML tag of two char's, such as <BR> or <ul>
    auto parsers = std::make_tuple (
        char_parser ('<'), any_char_parser(), any_char_parser(),
        char_parser ('>'));

    {
        std::string const sequence = "<BR>!";
        parse_outcome <void> initial_state (true,
            range::make_iterator_range (sequence));

        auto result = scan (parse(), initial_state, parsers);

        auto result2 = drop (result);
        auto result3 = drop (result2);

        BOOST_CHECK_EQUAL (first (result3).output.get(), 'B');

        auto result4 = drop (result3);
        BOOST_CHECK_EQUAL (first (result4).output.get(), 'R');

        auto result5 = drop (result4);

        // The remainder is '!'.
        BOOST_CHECK_EQUAL (first (first (result5).rest), '!');

        auto result6 = drop (result5);
        BOOST_CHECK (empty (result6));
    }
}

BOOST_AUTO_TEST_SUITE_END()
