/*
Copyright 2011-2013, 2015 Rogier van Dalen.

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

struct weird_count_tag {};

namespace range {
    template<> struct tag_of_qualified <weird_count>
    { typedef weird_count_tag type; };
} // namespace range

inline forgotten_to_define_direction implement_default_direction (
    weird_count_tag, weird_count const &);

inline rime::false_type implement_empty (
    weird_count_tag, weird_count const &, weird_direction)
{ return rime::false_; }

// size is not defined.

inline int implement_first (
    weird_count_tag, weird_count const & c, weird_direction)
{ return c.current; }

inline weird_count implement_drop (weird_count_tag, weird_count const & c,
    std::size_t increment, weird_direction)
{ return weird_count (c.current + increment); }

#endif // RANGE_TEST_WEIRD_COUNT_HPP_INCLUDED
