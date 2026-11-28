// SPDX-FileCopyrightText: Copyright 2012-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test "fold".
This includes tests on moving elements and states, on the types of states,
and on large ranges, to test for stack overflows.
*/

#define BOOST_TEST_MODULE test_range_fold
#include "boost/test/unit_test.hpp"

#include "range/fold.hpp"

#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "rime/check/check_equal.hpp"
#include "rime/core.hpp"
#include "rime/variant.hpp"
#include "unique_range.hpp"
#include "utility/returns.hpp"
#include "weird_direction.hpp"

#include "range/core.hpp"
#include "range/direction.hpp"
#include "range/function_range.hpp"
#include "range/std/list.hpp"
#include "range/std/tuple.hpp"
#include "range/std/vector.hpp"
#include "range/take.hpp"

BOOST_AUTO_TEST_SUITE(test_range_fold)

template <class Type> struct plus_fixed
{
    Type operator()(Type a, Type b) const { return a + b; }
};

struct plus
{
    template <class Left, class Right>
    auto operator()(Left const & a, Right const & b) const RETURNS(a + b);
};

struct add
{
    // Workaround for GCC 4.6.
    template <class Left, class Right> struct result
    {
        typedef decltype(std::declval<Right &>() += std::declval<Left>()) type;
    };

    template <class Left, class Right>
    typename result<Left, Right>::type operator()(
        Left const & a, Right & b) const
    {
        return (b += a);
    }
};

struct const_reference_second
{
    template <class Left, class Right>
    Right const & operator()(Left const &, Right const & b) const
    {
        return b;
    }
};

class none {};

template <class Type> class accumulator
{
private:
    Type m_sum;

public:
    accumulator() : m_sum() {}

    typedef none result_type;

    result_type operator()(none, Type const & t)
    {
        m_sum += t;
        return none();
    }

    Type const & sum() const { return this->m_sum; }
};

// Return the first argument, changing types:
// double -> short, short -> int &, int & -> float, float -> float.
struct type_changer
{
    short operator()(double d, int &) { return short(d + 1); }
    int & operator()(short, int & r) { return r; }
    float operator()(int & i, int &) { return float(i + 1); }
    float operator()(float f, int &) { return f + 1; }
};

BOOST_AUTO_TEST_CASE(has)
{
    static_assert(
        range::has<range::callable::fold(int, std::vector<int>, plus)>::value);
    static_assert(range::has<range::callable::fold(
                      int, std::vector<int>, direction::front, plus)>::value);
    static_assert(range::has<range::callable::fold(
                      int, std::vector<int>, direction::back, plus)>::value);

    static_assert(!range::has<range::callable::fold(
                      int, std::vector<int>, weird_direction, plus)>::value);

    static_assert(!range::has<range::callable::fold(float)>::value);
    static_assert(
        !range::has<range::callable::fold(float, std::vector<int>)>::value);

    static_assert(!range::has<range::callable::fold(
                      int, std::vector<int>, float, plus)>::value);
    static_assert(!range::has<range::callable::fold(
                      int, std::vector<int>, int, plus)>::value);
}

BOOST_AUTO_TEST_CASE(test_fold_heterogeneous)
{
    using range::back;
    using range::drop;
    using range::first;
    using range::fold;
    using range::front;
    plus_fixed<int> f1;

    /* Empty sequence. */
    std::tuple<> empty_vector;

    BOOST_CHECK_EQUAL(fold(6, empty_vector, f1), 6);
    BOOST_CHECK_EQUAL(fold(6, empty_vector, front, f1), 6);
    BOOST_CHECK_EQUAL(fold(6, empty_vector, back, f1), 6);

    BOOST_CHECK_EQUAL(fold(6, unique_view(empty_vector), f1), 6);
    BOOST_CHECK_EQUAL(fold(6, unique_view(empty_vector), front, f1), 6);
    BOOST_CHECK_EQUAL(fold(6, unique_view(empty_vector), back, f1), 6);

    /* Non-empty sequences. */
    std::tuple<int> one_vector(7);
    BOOST_CHECK_EQUAL(fold(3, one_vector, f1), 10);
    BOOST_CHECK_EQUAL(fold(3, one_vector, front, f1), 10);
    BOOST_CHECK_EQUAL(fold(3, one_vector, back, f1), 10);

    /* Non-empty sequences. */
    std::tuple<int, int, int> three_vector(7, 3, 17);
    BOOST_CHECK_EQUAL(fold(7, three_vector, front, f1), 34);
    BOOST_CHECK_EQUAL(fold(7, three_vector, back, f1), 34);

    /* Non-empty sequences: float returned from initial state int. */
    std::tuple<float, float, int> three_f_vector(7.25f, 3.5f, 17);
    RIME_CHECK_EQUAL(fold(7, three_f_vector, front, plus()), 34.75f);
    RIME_CHECK_EQUAL(fold(7, three_f_vector, back, plus()), 34.75f);

    /* Non-empty sequences: double. */
    std::tuple<float, double, int> three_d_vector(7.25f, 3.5, 17);
    RIME_CHECK_EQUAL(fold(7, three_d_vector, front, plus()), 34.75);
    RIME_CHECK_EQUAL(fold(7, unique_view(three_d_vector), back, plus()), 34.75);

    // Return reference to last element
    RIME_CHECK_EQUAL(fold(0, three_d_vector, const_reference_second()), 17);
    BOOST_CHECK_EQUAL(first(three_d_vector), 7.25f);
    BOOST_CHECK_EQUAL(first(drop(three_d_vector)), 3.5);
    BOOST_CHECK_EQUAL(first(drop(drop(three_d_vector))), 17);

    /* Non-const function object */
    {
        accumulator<double> accumulate;
        fold(none(), three_d_vector, front, accumulate);
        BOOST_CHECK_EQUAL(accumulate.sum(), 27.75);
    }
    {
        accumulator<double> accumulate;
        fold(none(), three_d_vector, back, accumulate);
        BOOST_CHECK_EQUAL(accumulate.sum(), 27.75);
    }

    rime::int_<2> two;
    /* Mutable vector. */
    {
        std::tuple<int, float, double> three_mutable_vector(3, 3.5f, 4.75);
        // Using RIME_CHECK_EQUAL the operation would be executed multiple
        // times.
        auto result = fold(2, unique_view(three_mutable_vector), front, add());
        RIME_CHECK_EQUAL(result, 13.25);
        BOOST_CHECK_EQUAL(first(three_mutable_vector), 5);
        BOOST_CHECK_EQUAL(first(drop(three_mutable_vector)), 8.5f);
        BOOST_CHECK_EQUAL(first(drop(three_mutable_vector, two)), 13.25);
    }

    {
        std::tuple<double, float, float> three_mutable_vector(3., 3.5f, 4.75f);
        // Using RIME_CHECK_EQUAL the operation would be executed multiple
        // times.
        auto result = fold(2, three_mutable_vector, back, add());
        RIME_CHECK_EQUAL(result, 13.25);
        BOOST_CHECK_EQUAL(first(three_mutable_vector, back), 6.75f);
        BOOST_CHECK_EQUAL(
            first(drop(three_mutable_vector, back), back), 10.25f);
        BOOST_CHECK_EQUAL(
            first(drop(three_mutable_vector, two, back), back), 13.25);
    }
}

BOOST_AUTO_TEST_CASE(test_fold_homogeneous)
{
    using range::back;
    using range::drop;
    using range::first;
    using range::fold;
    using range::front;

    plus_fixed<int> f1;

    /* Empty sequence. */
    std::vector<int> v;
    BOOST_CHECK_EQUAL(fold(6, v, f1), 6);
    BOOST_CHECK_EQUAL(fold(6, unique_view(v), front, f1), 6);
    BOOST_CHECK_EQUAL(fold(6, v, back, f1), 6);

    /* Non-empty sequences. */
    v.push_back(7);
    BOOST_CHECK_EQUAL(fold(3, v, f1), 10);
    BOOST_CHECK_EQUAL(fold(3, v, front, f1), 10);
    BOOST_CHECK_EQUAL(fold(3, unique_view(v), back, f1), 10);

    /* Non-empty sequences. */
    v.push_back(3);
    v.push_back(17);
    BOOST_CHECK_EQUAL(fold(7, unique_view(v), f1), 34);
    BOOST_CHECK_EQUAL(fold(7, v, front, f1), 34);
    BOOST_CHECK_EQUAL(fold(7, v, back, f1), 34);

    /* Non-const function object */
    {
        accumulator<int> accumulate;
        fold(none(), v, front, accumulate);
        BOOST_CHECK_EQUAL(accumulate.sum(), 27);
    }
    {
        accumulator<int> accumulate;
        fold(none(), v, back, accumulate);
        BOOST_CHECK_EQUAL(accumulate.sum(), 27);
    }

    int const zero = 0;
    // Return reference to last element
    // This is hard to do correctly.
    RIME_CHECK_EQUAL(fold(zero, v, const_reference_second()), 17);
    {
        int const & last = fold(zero, v, const_reference_second());
        int const & real_last = first(v, back);
        BOOST_CHECK(&last == &real_last);
    }
    BOOST_CHECK_EQUAL(first(v), 7);
    BOOST_CHECK_EQUAL(first(drop(v)), 3);
    BOOST_CHECK_EQUAL(first(drop(drop(v))), 17);

    /* Mutable. */
    int two = 2;
    // Using RIME_CHECK_EQUAL the operation would be executed multiple times.
    int result = fold(two, unique_view(v), front, add());
    RIME_CHECK_EQUAL(result, 29);
    BOOST_CHECK_EQUAL(first(v), 9);
    BOOST_CHECK_EQUAL(first(drop(v)), 12);
    BOOST_CHECK_EQUAL(first(drop(v, 2)), 29);

    result = fold(two, v, back, add());
    RIME_CHECK_EQUAL(result, 52);
    BOOST_CHECK_EQUAL(first(v), 52);
    BOOST_CHECK_EQUAL(first(drop(v)), 43);
    BOOST_CHECK_EQUAL(first(drop(v, 2)), 31);
}

BOOST_AUTO_TEST_CASE(test_fold_heterogeneous_function)
{
    using range::fold;
    // Result type that settles on double after the first application.
    {
        std::vector<double> v;
        auto result = fold(1, v, plus());
        static_assert(
            std::is_same_v<decltype(result), rime::variant<int, double>>);
        BOOST_CHECK(result.contains<int>());
        BOOST_CHECK_EQUAL(rime::get<int>(result), 1);

        v.push_back(1.5);
        auto result2 = fold(1, unique_view(v), plus());
        static_assert(
            std::is_same_v<decltype(result2), rime::variant<int, double>>);
        BOOST_CHECK(result2.contains<double>());
        BOOST_CHECK_EQUAL(rime::get<double>(result2), 2.5);
    }

    // Result type that takes a bit longer to settle:
    // double -> short, short -> int &, int & -> float, float -> float.
    {
        std::vector<int> v;
        {
            auto result = fold(1., v, type_changer());
            static_assert(std::is_same_v<
                          decltype(result),
                          rime::variant<double, short, int &, float>>);
            BOOST_CHECK(result.contains<double>());
            BOOST_CHECK_EQUAL(rime::get<double>(result), 1);
        }
        {
            auto result = fold(short(1), v, type_changer());
            BOOST_CHECK(result.contains<short>());
            BOOST_CHECK_EQUAL(rime::get<short>(result), 1);
        }

        v.push_back(4);
        {
            auto result = fold(1., v, type_changer());
            BOOST_CHECK(result.contains<short>());
            BOOST_CHECK_EQUAL(rime::get<short>(result), 2);
        }
        {
            auto result = fold(short(1), v, type_changer());
            BOOST_CHECK(result.contains<int &>());
            BOOST_CHECK_EQUAL(rime::get<int &>(result), 4);
        }

        v.push_back(4);
        {
            auto result = fold(1., v, type_changer());
            BOOST_CHECK(result.contains<int &>());
            BOOST_CHECK_EQUAL(rime::get<int &>(result), 4);
        }
        {
            auto result = fold(short(1), v, type_changer());
            BOOST_CHECK(result.contains<float>());
            BOOST_CHECK_EQUAL(rime::get<float>(result), 5);
        }

        v.push_back(4);
        {
            auto result = fold(1., unique_view(v), type_changer());
            BOOST_CHECK(result.contains<float>());
            BOOST_CHECK_EQUAL(rime::get<float>(result), 5);
        }
        {
            auto result = fold(short(1), v, type_changer());
            BOOST_CHECK(result.contains<float>());
            BOOST_CHECK_EQUAL(rime::get<float>(result), 6);
        }

        v.push_back(4);
        {
            auto result = fold(1., v, type_changer());
            BOOST_CHECK(result.contains<float>());
            BOOST_CHECK_EQUAL(rime::get<float>(result), 6);
        }
        {
            auto result = fold(short(1), unique_view(v), type_changer());
            BOOST_CHECK(result.contains<float>());
            BOOST_CHECK_EQUAL(rime::get<float>(result), 7);
        }
    }

    {
        std::tuple<int, int> t(4, 7);
        {
            auto result = fold(1., unique_view(t), type_changer());
            static_assert(std::is_same_v<decltype(result), int>);
            BOOST_CHECK_EQUAL(result, 7);
        }
    }
    {
        std::tuple<int, int, int> t(4, 7, 9);
        {
            auto result = fold(1., t, type_changer());
            static_assert(std::is_same_v<decltype(result), float>);
            BOOST_CHECK_EQUAL(result, 8);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

using range::fold;

BOOST_AUTO_TEST_SUITE(test_range_fold_moving)

int get_next()
{
    static int current = 0;
    return current++;
}

struct add
{
    int operator()(int a, int b) const { return a + b; }
};

BOOST_AUTO_TEST_CASE(function_range)
{
    auto count = range::make_function_range(&get_next);

    auto result = fold(0, range::take(std::move(count), 10), add());
    BOOST_CHECK_EQUAL(result, 45);
}

// More structured test than using function_range.
// Use unique_view.
BOOST_AUTO_TEST_CASE(test_fold_moving)
{
    {
        std::vector<int> v;
        v.push_back(5);
        v.push_back(6);
        v.push_back(7);
        auto view = unique_view(v);
        // range::helper::get_underlying (view);
        int result = fold(0, std::move(view), add());
        BOOST_CHECK_EQUAL(result, 5 + 6 + 7);
    }
}

// Use one_time_view.
BOOST_AUTO_TEST_CASE(test_fold_chop)
{
    {
        std::vector<int> v;
        v.push_back(5);
        v.push_back(6);
        v.push_back(7);
        // Note: one_time_view.
        auto view = one_time_view(v);

        int result = fold(0, std::move(view), add());
        BOOST_CHECK_EQUAL(result, 5 + 6 + 7);
    }
    {
        std::tuple<int, short, int> v(5, 6, 7);
        // Note: one_time_view.
        auto view = one_time_view(v);

        int result = fold(0, std::move(view), add());
        BOOST_CHECK_EQUAL(result, 5 + 6 + 7);
    }
}

/* Test that states are only moved and never copied. */

struct add_to_unique_ptr
{
    int value;

    std::unique_ptr<int> operator()(
        std::unique_ptr<int> && current, int v) const
    {
        // Pilfer "current".
        *current += v;
        return std::move(current);
    }
};

BOOST_AUTO_TEST_CASE(test_fold_move_state)
{
    {
        std::vector<int> v;
        v.push_back(456);
        v.push_back(-123);
        v.push_back(222);

        std::unique_ptr<int> result =
            fold(std::make_unique<int>(0), v, add_to_unique_ptr());
        BOOST_CHECK_EQUAL(*result, 555);
    }
    {
        std::tuple<int, short, int> v(456, -123, 222);

        std::unique_ptr<int> result =
            fold(std::make_unique<int>(0), v, add_to_unique_ptr());
        BOOST_CHECK_EQUAL(*result, 555);
    }
    {
        std::tuple<int, short, int> v(456, -123, 222);

        std::unique_ptr<int> result =
            fold(std::make_unique<int>(0), v, add_to_unique_ptr());
        BOOST_CHECK_EQUAL(*result, 555);
    }
}

BOOST_AUTO_TEST_SUITE_END()

using range::at_c;

struct int_holder
{
    int i;
    int_holder() : i(0) {}
    int_holder(int i) : i(i) {}
};

struct add_return_const
{
    // "const" is only retained in return types for class types, so return
    // "int_holder const" and not just "int const".
    int_holder const operator()(int_holder i, int j) const
    {
        return int_holder(i.i + j);
    }
};

BOOST_AUTO_TEST_SUITE(test_range_fold_state_types)

BOOST_AUTO_TEST_CASE(test_fold_const_state)
{
    {
        std::vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        static_assert(std::is_same_v<
                      decltype(fold(int_holder(), v, add_return_const())),
                      int_holder const>);
        BOOST_CHECK_EQUAL(fold(int_holder(), v, add_return_const()).i, 6);
    }
    {
        std::tuple<int, short, int> v(1, 2, 3);
        static_assert(std::is_same_v<
                      decltype(fold(int_holder(), v, add_return_const())),
                      int_holder const>);
        BOOST_CHECK_EQUAL(fold(int_holder(), v, add_return_const()).i, 6);
    }
}

struct unassignable
{
    int value;

    explicit unassignable(int value) : value(value) {}

    unassignable(unassignable const &) = default;

    unassignable & operator=(unassignable const &) = delete;
    unassignable & operator=(unassignable &&) = delete;

    unassignable operator+(unassignable const & that) const
    {
        return unassignable(this->value + that.value);
    }

    unassignable operator+(int that) const
    {
        return unassignable(this->value + that);
    }
};

struct add_unassignable
{
    unassignable operator()(unassignable const & i, int j) const
    {
        return unassignable(i.value + j);
    }
};

BOOST_AUTO_TEST_CASE(test_fold_unassignable_state)
{
    {
        std::vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        BOOST_CHECK(fold(unassignable(0), v, add_unassignable()).value == 6);
    }
    {
        std::tuple<int, short, int> v(1, 2, 3);
        BOOST_CHECK(fold(unassignable(0), v, add_unassignable()).value == 6);
    }
}

struct return_right
{
    template <class Left, class Right>
    Right && operator()(Left &&, Right && right) const
    {
        return static_cast<Right &&>(right);
    }
};

BOOST_AUTO_TEST_CASE(test_fold_lvalue_state)
{
    int i = 7;
    {
        std::vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        static_assert(
            std::is_same_v<decltype(fold(i, v, return_right())), int &>);
        BOOST_CHECK_EQUAL(&fold(i, v, return_right()), &at_c<2>(v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL(i, 7);
        BOOST_CHECK_EQUAL(at_c<0>(v), 1);
        BOOST_CHECK_EQUAL(at_c<1>(v), 2);
        BOOST_CHECK_EQUAL(at_c<2>(v), 3);
    }
    {
        std::tuple<int, short, int> v(1, 2, 3);
        static_assert(
            std::is_same_v<decltype(fold(i, v, return_right())), int &>);
        BOOST_CHECK_EQUAL(&fold(i, v, return_right()), &at_c<2>(v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL(i, 7);
        BOOST_CHECK_EQUAL(at_c<0>(v), 1);
        BOOST_CHECK_EQUAL(at_c<1>(v), 2);
        BOOST_CHECK_EQUAL(at_c<2>(v), 3);
    }
}

BOOST_AUTO_TEST_CASE(test_fold_rvalue_state)
{
    int i = 7;
    {
        std::vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        // If 5.5 were an int &&, this should return an rvalue reference.
        // But it doesn't, because rvalue references are (sensibly) stripped
        // from the state argument.
        static_assert(std::is_same_v<
                      decltype(fold(5.5, std::move(v), return_right())),
                      rime::variant<double, int &&>>);
        // BOOST_CHECK_EQUAL (
        //     &fold (return_right(), std::move (i), std::move (v)),
        //     &at_c <2> (v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL(i, 7);
        BOOST_CHECK_EQUAL(at_c<0>(v), 1);
        BOOST_CHECK_EQUAL(at_c<1>(v), 2);
        BOOST_CHECK_EQUAL(at_c<2>(v), 3);
    }
    {
        std::tuple<int, short, int> v(1, 2, 3);
        static_assert(std::is_same_v<
                      decltype(fold(i, std::move(v), return_right())), int &&>);
        auto && result = fold(i, std::move(v), return_right());
        BOOST_CHECK_EQUAL(&result, &at_c<2>(v));

        // Check that nothing has changed.
        BOOST_CHECK_EQUAL(i, 7);
        BOOST_CHECK_EQUAL(at_c<0>(v), 1);
        BOOST_CHECK_EQUAL(at_c<1>(v), 2);
        BOOST_CHECK_EQUAL(at_c<2>(v), 3);
    }
}

struct const_or_not
{
    const_or_not() {}

    bool operator()(int, int) const { return true; }
    bool operator()(int, int) { return false; }
};

BOOST_AUTO_TEST_CASE(test_fold_function_lvalue_rvalue)
{
    const_or_not f;
    const_or_not const f_const;

    BOOST_CHECK_EQUAL(f(1, 2), false);
    BOOST_CHECK_EQUAL(std::move(f)(1, 2), false);
    BOOST_CHECK_EQUAL(f_const(1, 2), true);

    {
        std::vector<int> v;
        v.push_back(1);

        BOOST_CHECK_EQUAL(fold(true, v, f), false);
        BOOST_CHECK_EQUAL(fold(true, v, std::move(f)), false);
        BOOST_CHECK_EQUAL(fold(true, v, f_const), true);
    }
    {
        std::tuple<int> v(1);

        BOOST_CHECK_EQUAL(fold(true, v, f), false);
        BOOST_CHECK_EQUAL(fold(true, v, std::move(f)), false);
        BOOST_CHECK_EQUAL(fold(true, v, f_const), true);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_range_fold_large)

struct plus
{
    template <class Left, class Right>
    auto operator()(Left const & a, Right const & b) const RETURNS(a + b);
};

/**
Add two values but change types: char becomes float, float becomes double,
double becomes int, and then we stay with ints.
*/
struct settling_plus
{
    float operator()(char l, int r) const { return float(l + r); }
    double operator()(float l, int r) const { return l + r; }
    int operator()(double l, int r) const { return int(l + r); }
    int operator()(int l, int r) const { return l + r; }
};

struct non_assignable
{
    int i;
    non_assignable(int i) : i(i) {}
    non_assignable(non_assignable const &) = default;

    non_assignable & operator=(non_assignable const &) = delete;
};

non_assignable operator+(
    non_assignable const & left, non_assignable const & right)
{
    return non_assignable(left.i + right.i);
}

/**
One definition of fold uses recursion.
That runs the risk of causing stack overflows.
This can be prevented in various ways.
One is by evaluating the fold not in a linear tree, but in a binary tree.
The other is by re-using the memory for the state, and use a while-loop.
Whichever version is used, the below is a homogeneous fold, and the
implementation should deal with it.
*/
BOOST_AUTO_TEST_CASE(test_fold_homogeneous_stack_overflow)
{
    using range::fold;

    std::size_t const element_num = 300000;

    std::vector<int> v(element_num, 1);
    BOOST_CHECK_EQUAL(fold(0, v, plus()), int(element_num));

    std::list<int> l(element_num, 1);
    BOOST_CHECK_EQUAL(fold(0, l, plus()), int(element_num));

    {
        auto result = fold(char(0), v, settling_plus());
        BOOST_CHECK_EQUAL(rime::get<int>(result), int(element_num));
    }
}

/**
Non-assignable classes should be used in a homogeneous fold.
*/
BOOST_AUTO_TEST_CASE(test_fold_non_assignable)
{
    using range::fold;
    std::size_t const element_num = 300000;

    std::vector<non_assignable> v(element_num, non_assignable(1));

    BOOST_CHECK_EQUAL(fold(non_assignable(0), v, plus()).i, int(element_num));
}

BOOST_AUTO_TEST_SUITE_END()
