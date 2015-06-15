/*
Copyright 2012-2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_fold
#include "utility/test/boost_unit_test.hpp"

#include "range/fold.hpp"

#include <tuple>
#include <vector>
#include <type_traits>

#include "utility/returns.hpp"
#include "rime/check/check_equal.hpp"
#include "weird_direction.hpp"
#include "unique_range.hpp"

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
        int, std::vector <int>, plus)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::fold (
        int, std::vector <int>, direction::front, plus)>));
    BOOST_MPL_ASSERT ((range::has <range::callable::fold (
        int, std::vector <int>, direction::back, plus)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        int, std::vector <int>, weird_direction, plus)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (float)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        float, std::vector <int>)>));

    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        int, std::vector <int>, float, plus)>));
    BOOST_MPL_ASSERT_NOT ((range::has <range::callable::fold (
        int, std::vector <int>, int, plus)>));
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

    BOOST_CHECK_EQUAL (fold (6, empty_vector, f1), 6);
    BOOST_CHECK_EQUAL (fold (6, empty_vector, front, f1), 6);
    BOOST_CHECK_EQUAL (fold (6, empty_vector, back, f1), 6);

    BOOST_CHECK_EQUAL (fold (6, unique_view (empty_vector), f1), 6);
    BOOST_CHECK_EQUAL (fold (6, unique_view (empty_vector), front, f1), 6);
    BOOST_CHECK_EQUAL (fold (6, unique_view (empty_vector), back, f1), 6);

    /* Non-empty sequences. */
    std::tuple <int> one_vector (7);
    BOOST_CHECK_EQUAL (fold (3, one_vector, f1), 10);
    BOOST_CHECK_EQUAL (fold (3, one_vector, front, f1), 10);
    BOOST_CHECK_EQUAL (fold (3, one_vector, back, f1), 10);

    /* Non-empty sequences. */
    std::tuple <int, int, int> three_vector (7, 3, 17);
    BOOST_CHECK_EQUAL (fold (7, three_vector, front, f1), 34);
    BOOST_CHECK_EQUAL (fold (7, three_vector, back, f1), 34);

    /* Non-empty sequences: float returned from initial state int. */
    std::tuple <float, float, int> three_f_vector (7.25f, 3.5f, 17);
    RIME_CHECK_EQUAL (fold (7, three_f_vector, front, plus()), 34.75f);
    RIME_CHECK_EQUAL (fold (7, three_f_vector, back, plus()), 34.75f);

    /* Non-empty sequences: double. */
    std::tuple <float, double, int> three_d_vector (7.25f, 3.5, 17);
    RIME_CHECK_EQUAL (fold (7, three_d_vector, front, plus()), 34.75);
    RIME_CHECK_EQUAL (fold (7, unique_view (three_d_vector), back, plus()),
        34.75);

    // Return reference to last element
    RIME_CHECK_EQUAL (fold (0, three_d_vector, const_reference_second()), 17);
    BOOST_CHECK_EQUAL (first (three_d_vector), 7.25f);
    BOOST_CHECK_EQUAL (first (drop (three_d_vector)), 3.5);
    BOOST_CHECK_EQUAL (first (drop (drop (three_d_vector))), 17);

    /* Non-const function object */
    {
        accumulator <double> accumulate;
        fold (none(), three_d_vector, front, accumulate);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27.75);
    }
    {
        accumulator <double> accumulate;
        fold (none(), three_d_vector, back, accumulate);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27.75);
    }

    rime::int_ <2> two;
    /* Mutable vector. */
    {
        std::tuple <int, float, double> three_mutable_vector (3, 3.5f, 4.75);
        // Using RIME_CHECK_EQUAL the operation would be executed multiple
        // times.
        auto result = fold (2, unique_view (three_mutable_vector),
            front, add());
        RIME_CHECK_EQUAL (result, 13.25);
        BOOST_CHECK_EQUAL (first (three_mutable_vector), 5);
        BOOST_CHECK_EQUAL (first (drop (three_mutable_vector)), 8.5f);
        BOOST_CHECK_EQUAL (first (drop (three_mutable_vector, two)), 13.25);
    }

    {
        std::tuple <double, float, float>
            three_mutable_vector (3., 3.5f, 4.75f);
        // Using RIME_CHECK_EQUAL the operation would be executed multiple
        // times.
        auto result = fold (2, three_mutable_vector, back, add());
        RIME_CHECK_EQUAL (result, 13.25);
        BOOST_CHECK_EQUAL (first (three_mutable_vector, back), 6.75f);
        BOOST_CHECK_EQUAL (first (drop (three_mutable_vector, back), back),
            10.25f);
        BOOST_CHECK_EQUAL (first (drop (three_mutable_vector, two, back), back),
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
    BOOST_CHECK_EQUAL (fold (6, v, f1), 6);
    BOOST_CHECK_EQUAL (fold (6, unique_view (v), front, f1), 6);
    BOOST_CHECK_EQUAL (fold (6, v, back, f1), 6);

    /* Non-empty sequences. */
    v.push_back (7);
    BOOST_CHECK_EQUAL (fold (3, v, f1), 10);
    BOOST_CHECK_EQUAL (fold (3, v, front, f1), 10);
    BOOST_CHECK_EQUAL (fold (3, unique_view (v), back, f1), 10);

    /* Non-empty sequences. */
    v.push_back (3);
    v.push_back (17);
    BOOST_CHECK_EQUAL (fold (7, unique_view (v), f1), 34);
    BOOST_CHECK_EQUAL (fold (7, v, front, f1), 34);
    BOOST_CHECK_EQUAL (fold (7, v, back, f1), 34);

    /* Non-const function object */
    {
        accumulator <int> accumulate;
        fold (none(), v, front, accumulate);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27);
    }
    {
        accumulator <int> accumulate;
        fold (none(), v, back, accumulate);
        BOOST_CHECK_EQUAL (accumulate.sum(), 27);
    }

    int const zero = 0;
    // Return reference to last element
    // This is hard to do correctly.
    RIME_CHECK_EQUAL (fold (zero, v, const_reference_second()), 17);
    {
        int const & last = fold (zero, v, const_reference_second());
        int const & real_last = first (v, back);
        BOOST_CHECK (&last == &real_last);
    }
    BOOST_CHECK_EQUAL (first (v), 7);
    BOOST_CHECK_EQUAL (first (drop (v)), 3);
    BOOST_CHECK_EQUAL (first (drop (drop (v))), 17);

    /* Mutable. */
    int two = 2;
    // Using RIME_CHECK_EQUAL the operation would be executed multiple times.
    int result = fold (two, unique_view (v), front, add());
    RIME_CHECK_EQUAL (result, 29);
    BOOST_CHECK_EQUAL (first (v), 9);
    BOOST_CHECK_EQUAL (first (drop (v)), 12);
    BOOST_CHECK_EQUAL (first (drop (v, 2)), 29);

    result = fold (two, v, back, add());
    RIME_CHECK_EQUAL (result, 52);
    BOOST_CHECK_EQUAL (first (v), 52);
    BOOST_CHECK_EQUAL (first (drop (v)), 43);
    BOOST_CHECK_EQUAL (first (drop (v, 2)), 31);
}

BOOST_AUTO_TEST_CASE (test_fold_heterogeneous_function) {
    using range::fold;
    // Result type that settles on double after the first application.
    {
        std::vector <double> v;
        auto result = fold (1, v, plus());
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (result), rime::variant <int, double>>));
        BOOST_CHECK (result.contains <int>());
        BOOST_CHECK_EQUAL (rime::get <int> (result), 1);

        v.push_back (1.5);
        auto result2 = fold (1, unique_view (v), plus());
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
            auto result = fold (1., v, type_changer());
            BOOST_MPL_ASSERT ((std::is_same <decltype (result),
                rime::variant <double, short, int &, float>>));
            BOOST_CHECK (result.contains <double>());
            BOOST_CHECK_EQUAL (rime::get <double> (result), 1);
        }
        {
            auto result = fold (short (1), v, type_changer());
            BOOST_CHECK (result.contains <short>());
            BOOST_CHECK_EQUAL (rime::get <short> (result), 1);
        }

        v.push_back (4);
        {
            auto result = fold (1., v, type_changer());
            BOOST_CHECK (result.contains <short>());
            BOOST_CHECK_EQUAL (rime::get <short> (result), 2);
        }
        {
            auto result = fold (short (1), v, type_changer());
            BOOST_CHECK (result.contains <int &>());
            BOOST_CHECK_EQUAL (rime::get <int &> (result), 4);
        }

        v.push_back (4);
        {
            auto result = fold (1., v, type_changer());
            BOOST_CHECK (result.contains <int &>());
            BOOST_CHECK_EQUAL (rime::get <int &> (result), 4);
        }
        {
            auto result = fold (short (1), v, type_changer());
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 5);
        }

        v.push_back (4);
        {
            auto result = fold (1., unique_view (v), type_changer());
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 5);
        }
        {
            auto result = fold (short (1), v, type_changer());
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 6);
        }

        v.push_back (4);
        {
            auto result = fold (1., v, type_changer());
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 6);
        }
        {
            auto result = fold (short (1), unique_view (v), type_changer());
            BOOST_CHECK (result.contains <float>());
            BOOST_CHECK_EQUAL (rime::get <float> (result), 7);
        }
    }

    {
        std::tuple <int, int> t (4, 7);
        {
            auto result = fold (1., unique_view (t), type_changer());
            BOOST_MPL_ASSERT ((std::is_same <decltype (result), int>));
            BOOST_CHECK_EQUAL (result, 7);
        }
    }
    {
        std::tuple <int, int, int> t (4, 7, 9);
        {
            auto result = fold (1., t, type_changer());
            BOOST_MPL_ASSERT ((std::is_same <decltype (result), float>));
            BOOST_CHECK_EQUAL (result, 8);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
