/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_shared_list_full
#include "utility/test/boost_unit_test.hpp"
#include "utility/test/boost_unit_test_command_line.hpp"

#include "container/shared_list.hpp"

#include <memory>
#include "utility/test/test_allocator.hpp"
#include "utility/test/tracked.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_range_shared_list_full)

typedef utility::tracked <int> object_type;
typedef utility::test_allocator <std::allocator <object_type>, false>
    allocator_type;

typedef range::shared_list <object_type, direction::front, allocator_type>
    shared_list;

BOOST_AUTO_TEST_CASE (allocators) {
    std::vector <allocator_type> allocators;
    // utility::thrower thrower;
    utility::tracked_registry r;
    {
        allocators.emplace_back();
        shared_list l1 (allocators.back());
        l1.push (object_type (r, 4));

        allocators.emplace_back();
        shared_list l2 (l1, allocators.back());
        l2.push (object_type (r, 5));
    }
}

BOOST_AUTO_TEST_SUITE_END()
