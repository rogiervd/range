/*
Copyright 2011, 2012 Rogier van Dalen.

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

/**
Provide a simple direction class for testing the Range library.
*/

#ifndef RANGE_TEST_WEIRD_DIRECTION_HPP_INCLUDED
#define RANGE_TEST_WEIRD_DIRECTION_HPP_INCLUDED

#include "range/direction.hpp"

struct forgotten_to_define_direction;
struct weird_direction;
struct weird_reverse_direction;

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

    weird_direction (weird_reverse_direction const & other);

    ~weird_direction() { assert_invariant(); }

    void assert_invariant() { assert (n == 7); }
};

struct weird_reverse_direction {
    int n;

    // Can only be constructed with an int with value 7.
    weird_reverse_direction (int n) : n (n)  { assert_invariant(); }

    weird_reverse_direction (weird_reverse_direction const & other)
    : n (other.n) { assert_invariant(); }

    weird_reverse_direction (weird_direction const & other)
    : n (other.n) { assert_invariant(); }

    ~weird_reverse_direction() { assert_invariant(); }

    void assert_invariant() { assert (n == 7); }
};

inline weird_direction::weird_direction (weird_reverse_direction const & other)
: n (other.n) { assert_invariant(); }


namespace direction {

    template <> struct is_direction_bare <weird_direction>
    : boost::mpl::true_ {};

    template <> struct is_direction_bare <weird_reverse_direction>
    : boost::mpl::true_ {};

    namespace operation {

        template <> struct make_forward <weird_reverse_direction> {
            weird_direction operator() (weird_reverse_direction const & d)
                const
            { return weird_direction (d); }
        };

        template <> struct reverse <weird_direction> {
            weird_reverse_direction operator() (weird_direction const & d)
                const
            { return weird_reverse_direction (d); }
        };

    } // namespace operation
} // namespace direction


#endif  // RANGE_TEST_WEIRD_DIRECTION_HPP_INCLUDED

