/*
Copyright 2011, 2012, 2013 Rogier van Dalen.

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
Provide a simple range for testing the Range library.
Its default direction does not actual work, so it is great for checking whether
algorithms actually propagate the direction.
*/

#ifndef RANGE_TEST_WEIRD_COUNT_HPP_INCLUDED
#define RANGE_TEST_WEIRD_COUNT_HPP_INCLUDED

#include "range/core.hpp"

#include "weird_direction.hpp"

/**
Test range that uses weird_direction.
*/
struct weird_count {
    int current;

    weird_count() : current (0) {}
    weird_count (int current) : current (current) {}
};

struct weird_count_tag;

namespace range {

template<> struct tag_of_bare <weird_count> { typedef weird_count_tag type; };

namespace operation {

    template<> struct default_direction <weird_count_tag>
    {
        forgotten_to_define_direction operator() (weird_count const &) const;
    };

    template<> struct empty <weird_count_tag, weird_direction> {
        rime::bool_ <false>
            operator() (weird_direction, weird_count const &) const
        { return rime::false_; }
    };

    // size is not defined

    template<> struct first <weird_count_tag, weird_direction> {
        int operator() (weird_direction, weird_count const & c) const
        { return c.current; }
    };

    template <typename Increment>
        struct drop <weird_count_tag, weird_direction, Increment>
    {
        weird_count operator() (weird_direction, Increment const & increment,
            weird_count const & c) const
        { return weird_count (c.current + increment); }
    };

}} // namespace range::operation

#endif  // RANGE_TEST_WEIRD_COUNT_HPP_INCLUDED

