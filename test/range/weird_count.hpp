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

struct weird_count_tag;

namespace range {

template<> struct tag_of_qualified <weird_count>
{ typedef weird_count_tag type; };

namespace operation {

    template <class WeirdCount>
        struct default_direction <weird_count_tag, WeirdCount>
    {
        forgotten_to_define_direction operator() (weird_count const &) const;
    };

    template <class WeirdCount>
        struct empty <weird_count_tag, weird_direction, WeirdCount>
    {
        rime::bool_ <false>
            operator() (weird_direction, weird_count const &) const
        { return rime::false_; }
    };

    // size is not defined.

    template <class WeirdCount>
        struct first <weird_count_tag, weird_direction, WeirdCount>
    {
        int operator() (weird_direction, weird_count const & c) const
        { return c.current; }
    };

    template <class Increment, class WeirdCount>
        struct drop <weird_count_tag, weird_direction, Increment, WeirdCount>
    {
        weird_count operator() (weird_direction, Increment const & increment,
            weird_count const & c) const
        { return weird_count (c.current + increment); }
    };

}} // namespace range::operation

#endif  // RANGE_TEST_WEIRD_COUNT_HPP_INCLUDED
