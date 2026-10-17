// SPDX-FileCopyrightText: Copyright 2011-2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

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
struct weird_count
{
    int current;

    weird_count() : current(0) {}
    weird_count(int current) : current(current) {}
};

struct weird_count_tag {};

namespace range {
template <> struct tag_of_qualified<weird_count>
{
    typedef weird_count_tag type;
};
}  // namespace range

inline forgotten_to_define_direction implement_default_direction(
    weird_count_tag, weird_count const &);

inline rime::false_type implement_empty(
    weird_count_tag, weird_count const &, weird_direction)
{
    return rime::false_;
}

// size is not defined.

inline int implement_first(
    weird_count_tag, weird_count const & c, weird_direction)
{
    return c.current;
}

inline weird_count implement_drop(
    weird_count_tag, weird_count const & c, std::size_t increment,
    weird_direction)
{
    return weird_count(c.current + int(increment));
}

#endif  // RANGE_TEST_WEIRD_COUNT_HPP_INCLUDED
