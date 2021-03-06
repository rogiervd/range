/*
Copyright 2011, 2012, 2015 Rogier van Dalen.

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

/**
Provide a simple direction class for testing the Range library.
*/

#ifndef RANGE_TEST_WEIRD_DIRECTION_HPP_INCLUDED
#define RANGE_TEST_WEIRD_DIRECTION_HPP_INCLUDED

#include "range/direction.hpp"

struct forgotten_to_define_direction;
struct weird_direction;
struct weird_opposite_direction;

struct forgotten_to_define_direction {
    // Cause linker error if this actually gets used.
    forgotten_to_define_direction();
};

struct weird_direction {
    int n;

    // Can only be constructed with an int with value 7.
    weird_direction (int n) : n (n) { assert_invariant(); }

    weird_direction (weird_direction const & other)
    : n (other.n) { assert_invariant(); }

    explicit weird_direction (weird_opposite_direction const & other);

    ~weird_direction() {
        assert_invariant();
        n = 1234;
    }

    void assert_invariant() { assert (n == 7); }
};

struct weird_opposite_direction {
    int n;

    // Can only be constructed with an int with value 7.
    weird_opposite_direction (int n) : n (n)  { assert_invariant(); }

    weird_opposite_direction (weird_opposite_direction const & other)
    : n (other.n) { assert_invariant(); }

    explicit weird_opposite_direction (weird_direction const & other)
    : n (other.n) { assert_invariant(); }

    ~weird_opposite_direction() {
        assert_invariant();
        n = 1234;
    }

    void assert_invariant() { assert (n == 7); }
};

inline weird_direction::weird_direction (weird_opposite_direction const & other)
: n (other.n) { assert_invariant(); }

inline rime::true_type operator== (
    weird_direction const &, weird_direction const &)
{ return rime::true_; }

inline rime::true_type operator== (
    weird_opposite_direction const &, weird_opposite_direction const &)
{ return rime::true_; }

namespace direction {

    template <> struct is_direction_bare <weird_direction>
    : boost::mpl::true_ {};

    template <> struct is_direction_bare <weird_opposite_direction>
    : boost::mpl::true_ {};

} // namespace direction

inline weird_direction implement_make_forward (
    weird_opposite_direction const & d)
{ return weird_direction (d); }

inline weird_opposite_direction implement_opposite (
    weird_direction const & d)
{ return weird_opposite_direction (d); }

#endif  // RANGE_TEST_WEIRD_DIRECTION_HPP_INCLUDED
