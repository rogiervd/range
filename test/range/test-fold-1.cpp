/*
Copyright 2012, 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_fold
#include "utility/test/boost_unit_test.hpp"

#include "range/fold.hpp"

#include <tuple>
#include <vector>
#include <type_traits>

#include "utility/returns.hpp"
#include "check_equal.hpp"
#include "weird_direction.hpp"

template <class Range>
    inline auto unique_view (Range && range)
RETURNS (range::view (std::forward <Range> (range)));

#include "range/std.hpp"

BOOST_AUTO_TEST_SUITE(test_range_fold)

template <class Type> struct plus_fixed {
    Type operator() (Type a, Type b) const { return a + b; }
};

struct plus {
    template <class Left, class Right>
    auto operator() (Left const & a, Right const & b) const RETURNS (a + b);
};

struct add {
    // Workaround for GCC 4.6.
    template <class Left, class Right> struct result {
        typedef decltype (std::declval <Right &>() += std::declval <Left>())
            type;
    };

    template <class Left, class Right>
    typename result <Left, Right>::type
        operator() (Left const & a, Right & b) const
    { return (b += a); }
};

struct const_reference_second {
    template <class Left, class Right>
        Right const & operator() (Left const &, Right const & b) const
    { return b; }
};

class none {};

template <class Type> class accumulator {
private:
    Type m_sum;
public:
    accumulator() : m_sum() {}

    typedef none result_type;

    result_type operator() (none, Type const & t) {
        m_sum += t;
        return none();
    }

    Type const & sum() const { return this->m_sum; }
};

// Return the first argument, changing types:
// double -> short, short -> int &, int & -> float, float -> float.
struct type_changer {
    short operator() (double d, int &) { return short (d + 1); }
    int & operator() (short s, int & r) { return r; }
    float operator() (int & i, int &) { return float (i + 1); }
    float operator() (float f, int &) { return f + 1; }
};

BOOST_AUTO_TEST_CASE (has) {
    BOOST_MPL_ASSERT ((range::has <range::callable::fold (
        plus, int, std::vector <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::fold (
        direction::front, plus, int, std::vector <int>)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::fold (
        direction::back, plus, int, std::vector <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        weird_direction, plus, int, std::vector <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (float)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        float, std::vector <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        float, plus, int, std::vector <int>)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        int, plus, int, std::vector <int>)>));
}

BOOST_AUTO_TEST_CASE (test_fold_heterogeneous) {
    using range::fold;
    using range::drop;
    using range::first;
    using range::front;
    using range::back;
    plus_fixed <int> f1;

    /* Empty sequence. */
    std::tuple <> empty_vector;

    BOOST_CHECK_EQUAL (fold (f1, 6, empty_vector), 6);
    BOOST_CHECK_EQUAL (fold (front, f1, 6, empty_vector), 6);
    BOOST_CHECK_EQUAL (fold (back, f1, 6, empty_vector), 6);

    BOOST_CHECK_EQUAL (fold (f1, 6, unique_view (empty_vector)), 6);
    BOOST_CHECK_EQUAL (fold (front, f1, 6, unique_view (empty_vector)), 6);
    BOOST_CHECK_EQUAL (fold (back, f1, 6, unique_view (empty_vector)), 6);

    /* Non-empty sequences. */
    std::tuple <int> one_vector (7);
    BOOST_CHECK_EQUAL (fold (f1, 3, one_vector), 10);
    BOOST_CHECK_EQUAL (fold (front, f1, 3, one_vector), 10);
    BOOST_CHECK_EQUAL (fold (back, f1, 3, one_vector), 10);

    /* Non-empty sequences. */
    std::tuple <int, int, int> three_vector (7, 3, 17);
    BOOST_CHECK_EQUAL (fold (front, f1, 7, three_vector), 34);
    BOOST_CHECK_EQUAL (fold (back, f1, 7, three_vector), 34);

    /* Non-empty sequences: float returned from initial state int. */
    std::tuple <float, float, int> three_f_vector (7.25f, 3.5f, 17);
    RIME_CHECK_EQUAL (fold (front, plus(), 7, three_f_vector), 34.75f);
    RIME_CHECK_EQUAL (fold (back, plus(), 7, three_f_vector), 34.75f);

    /* Non-empty sequences: double. */
    std::tuple <float, double, int> three_d_vector (7.25f, 3.5, 17);
    RIME_CHECK_EQUAL (fold (front, plus(), 7, three_d_vector), 34.75);
    RIME_CHECK_EQUAL (fold (back, plus(), 7, unique_view (three_d_vector)),
        34.75);

    // Return reference to last element
    RIME_CHECK_EQUAL (fold (const_reference_second(), 0, three_d_vector), 17);
    BOOST_CHECK_EQUAL (first (three_d_vector), 7.25f);
    BOOST_CHECK_EQUAL (first (drop (three_d_vector)), 3.5);
    BOOST_CHECK_EQUAL (first (drop (drop (three_d_vector))), 17);

    /* Non-const function object */
    {
        accumulator <double> accumulate;
        fold (front, accumulate, none(), three_d_vector);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27.75);
    }
    {
        accumulator <double> accumulate;
        fold (back, accumulate, none(), three_d_vector);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27.75);
    }

    rime::int_ <2> two;
    /* Mutable vector. */
    {
        std::tuple <int, float, double> three_mutable_vector (3, 3.5f, 4.75);
        // Using RIME_CHECK_EQUAL the operation would be executed multiple
        // times.
        auto result = fold (front, add(), 2,
            unique_view (three_mutable_vector));
        RIME_CHECK_EQUAL (result, 13.25);
        BOOST_CHECK_EQUAL (first (three_mutable_vector), 5);
        BOOST_CHECK_EQUAL (first (drop (three_mutable_vector)), 8.5f);
        BOOST_CHECK_EQUAL (first (drop (two, three_mutable_vector)), 13.25);
    }

    {
        std::tuple <double, float, float>
            three_mutable_vector (3., 3.5f, 4.75f);
        // Using RIME_CHECK_EQUAL the operation would be executed multiple
        // times.
        auto result = fold (back, add(), 2, three_mutable_vector);
        RIME_CHECK_EQUAL (result, 13.25);
        BOOST_CHECK_EQUAL (first (back, three_mutable_vector), 6.75f);
        BOOST_CHECK_EQUAL (first (back, drop (back, three_mutable_vector)),
            10.25f);
        BOOST_CHECK_EQUAL (first (back, drop (back, two, three_mutable_vector)),
            13.25);
    }
}

BOOST_AUTO_TEST_CASE (test_fold_homogeneous) {
    using range::fold;
    using range::drop;
    using range::first;
    using range::front;
    using range::back;

    plus_fixed <int> f1;

    /* Empty sequence. */
    std::vector <int> v;
    BOOST_CHECK_EQUAL (fold (f1, 6, v), 6);
    BOOST_CHECK_EQUAL (fold (front, f1, 6, unique_view (v)), 6);
    BOOST_CHECK_EQUAL (fold (back, f1, 6, v), 6);

    /* Non-empty sequences. */
    v.push_back (7);
    BOOST_CHECK_EQUAL (fold (f1, 3, v), 10);
    BOOST_CHECK_EQUAL (fold (front, f1, 3, v), 10);
    BOOST_CHECK_EQUAL (fold (back, f1, 3, unique_view (v)), 10);

    /* Non-empty sequences. */
    v.push_back (3);
    v.push_back (17);
    BOOST_CHECK_EQUAL (fold (f1, 7, unique_view (v)), 34);
    BOOST_CHECK_EQUAL (fold (front, f1, 7, v), 34);
    BOOST_CHECK_EQUAL (fold (back, f1, 7, v), 34);

    /* Non-const function object */
    {
        accumulator <int> accumulate;
        fold (front, accumulate, none(), v);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27);
    }
    {
        accumulator <int> accumulate;
        fold (back, accumulate, none(), v);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27);
    }

    int const zero = 0;
    // Return reference to last element
    // This is hard to do correctly.
    RIME_CHECK_EQUAL (fold (const_reference_second(), zero, v), 17);
    {
        int const & last = fold (const_reference_second(), zero, v);
        int const & real_last = first (back, v);
        BOOST_CHECK (&last == &real_last);
    }
    BOOST_CHECK_EQUAL (first (v), 7);
    BOOST_CHECK_EQUAL (first (drop (v)), 3);
    BOOST_CHECK_EQUAL (first (drop (drop (v))), 17);

    /* Mutable. */
    int two = 2;
    // Using RIME_CHECK_EQUAL the operation would be executed multiple times.
    int result = fold (front, add(), two, unique_view (v));
    RIME_CHECK_EQUAL (result, 29);
    BOOST_CHECK_EQUAL (first (v), 9);
    BOOST_CHECK_EQUAL (first (drop (v)), 12);
    BOOST_CHECK_EQUAL (first (drop (2, v)), 29);

    result = fold (back, add(), two, v);
    RIME_CHECK_EQUAL (result, 52);
    BOOST_CHECK_EQUAL (first (v), 52);
    BOOST_CHECK_EQUAL (first (drop (v)), 43);
    BOOST_CHECK_EQUAL (first (drop (2, v)), 31);
}

BOOST_AUTO_TEST_CASE (test_fold_heterogeneous_function) {
    using range::fold;
    // Result type that settles on double after the first application.
    {
        std::vector <double> v;
        auto result = fold (plus(), 1, v);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (result), rime::variant <int, double>>));
        BOOST_CHECK (result.contains <int>());
        BOOST_CHECK_EQUAL (rime::get <int> (result), 1);

        v.push_back (1.5);
        auto result2 = fold (plus(), 1, unique_view (v));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (result2), rime::variant <int, double>>));
        BOOST_CHECK (result2.contains <double>());
        BOOST_CHECK_EQUAL (rime::get <double> (result2), 2.5);
    }

    // Result type that takes a bit longer to settle:
    // double -> short, short -> int &, int & -> float, float -> float.
    {
        std::vector <int> v;
        {
            auto result = fold (type_changer(), 1., v);
            BOOST_MPL_ASSERT ((std::is_same <decltype (result),
                rime::variant <double, short, int &, float>>));
            BOOST_CHECK (result.contains <double>());
            BOOST_CHECK_EQUAL (rime::get <double> (result), 1);
        }
        {
            auto result = fold (type_changer(), short (1), v);
            BOOST_CHECK (result.contains <short>());
            BOOST_CHECK_EQUAL (rime::get <short> (result), 1);
        }

        v.push_back (4);
        {
            auto result = fold (type_changer(), 1., v);
            BOOST_CHECK (result.contains <short>());
            BOOST_CHECK_EQUAL (rime::get <short> (result), 2);
        }
        {
            auto result = fold (type_changer(), short (1), v);
            BOOST_CHECK (result.contains <int &>());
            BOOST_CHECK_EQUAL (rime::get <int &> (result), 4);
        }

        v.push_back (4);
        {
            auto result = fold (type_changer(), 1., v);
            BOOST_CHECK (result.contains <int &>());
            BOOST_CHECK_EQUAL (rime::get <int &> (result), 4);
        }
        {
            auto result = fold (type_changer(), short (1), v);
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 5);
        }

        v.push_back (4);
        {
            auto result = fold (type_changer(), 1., unique_view (v));
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 5);
        }
        {
            auto result = fold (type_changer(), short (1), v);
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 6);
        }

        v.push_back (4);
        {
            auto result = fold (type_changer(), 1., v);
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 6);
        }
        {
            auto result = fold (
                type_changer(), short (1), unique_view (v));
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 7);
        }
    }

    {
        std::tuple <int, int> t (4, 7);
        {
            auto result = fold (type_changer(), 1., unique_view (t));
            BOOST_MPL_ASSERT ((std::is_same <decltype (result), int>));
            BOOST_CHECK_EQUAL (result, 7);
        }
    }
    {
        std::tuple <int, int, int> t (4, 7, 9);
        {
            auto result = fold (type_changer(), 1., t);
            BOOST_MPL_ASSERT ((std::is_same <decltype (result), float>));
            BOOST_CHECK_EQUAL (result, 8);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
