/*
Copyright 2016 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_container_contiguous_storage
#include "utility/test/boost_unit_test.hpp"

#include "container/contiguous_storage.hpp"

#include "utility/test/test_allocator.hpp"
#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"

BOOST_AUTO_TEST_SUITE(test_contiguous_storage)

template <class ResizePolicy>
    std::size_t resize_policy_grow_reasonable (
        std::size_t current, std::size_t minimum)
{
    assert (minimum > current);
    std::size_t new_size = ResizePolicy::grow (current, minimum);
    BOOST_CHECK_GE (new_size, minimum);
    return new_size;
}

BOOST_AUTO_TEST_CASE (test_exponential_2) {
    typedef container::exponential_2 policy;
    BOOST_CHECK_EQUAL (resize_policy_grow_reasonable <policy> (0, 1), 1);
    BOOST_CHECK_EQUAL (resize_policy_grow_reasonable <policy> (1, 2), 2);
    BOOST_CHECK_EQUAL (resize_policy_grow_reasonable <policy> (2, 3), 4);
    BOOST_CHECK_EQUAL (resize_policy_grow_reasonable <policy> (4, 6), 8);

    // \todo Test shrink too.
}

template <class Storage> void check_storage_int_0 (Storage & storage) {
    // \todo Test typedefs?
    BOOST_CHECK_EQUAL (storage.memory_begin(), storage.memory_end());
    BOOST_CHECK_EQUAL (storage.memory_size(), 0);
}

template <class Storage> void check_storage_int_2 (Storage & storage) {
    BOOST_CHECK (storage.memory_begin() != storage.memory_end());
    BOOST_CHECK_EQUAL (storage.memory_size(), 2);

    *storage.memory_begin() = 5;
    *(storage.memory_begin() + 1) = 7;

    // This should cause errors:
    // *(storage.memory_begin() + 2) = 27;

    BOOST_CHECK_EQUAL (storage.memory_begin() + 2, storage.memory_end());
    BOOST_CHECK_EQUAL (*storage.memory_begin(), 5);
    BOOST_CHECK_EQUAL (*(storage.memory_begin() + 1), 7);
}

template <class Storage> void check_resize_int_2 (Storage & storage) {
    Storage ints_4 = storage.resized (4);
    BOOST_CHECK_EQUAL (ints_4.memory_size(), 4);
    *ints_4.memory_begin() = 28;
    *(ints_4.memory_begin() + 2) = 29;
    storage = std::move (ints_4);
    BOOST_CHECK_EQUAL (*storage.memory_begin(), 28);
    BOOST_CHECK_EQUAL (*(storage.memory_begin() + 2), 29);
}

BOOST_AUTO_TEST_CASE (test_internal_storage) {
    container::internal_storage <int, 0> empty;
    check_storage_int_0 (empty);

    container::internal_storage <int, 2> ints;
    check_storage_int_2 (ints);
}

BOOST_AUTO_TEST_CASE (test_dynamic_storage) {
    container::dynamic_storage <int> empty (0);
    check_storage_int_0 (empty);

    container::dynamic_storage <int> ints (2);
    check_storage_int_2 (ints);

    check_resize_int_2 (ints);
}

BOOST_AUTO_TEST_CASE (test_allocator_storage_simple) {
    typedef std::allocator <int> allocator_type;
    allocator_type allocator;

    container::allocator_storage <allocator_type> empty (allocator, 0);
    check_storage_int_0 (empty);

    container::allocator_storage <allocator_type> ints (allocator, 2);
    check_storage_int_2 (ints);

    check_resize_int_2 (ints);
}

void check_allocator_storage (utility::thrower & thrower) {
    typedef utility::test_allocator <std::allocator <int>> allocator_type;
    allocator_type allocator (thrower);

    container::allocator_storage <allocator_type> empty (allocator, 0);
    check_storage_int_0 (empty);

    container::allocator_storage <allocator_type> ints (allocator, 2);
    check_storage_int_2 (ints);

    check_resize_int_2 (ints);
}

BOOST_AUTO_TEST_CASE (test_allocator_storage_indepth) {
    utility::check_all_throw_points (&check_allocator_storage);
}

/* Padding policies. */

template <class Padding> void check_flush_left_up_to_3 (Padding & padding) {
    BOOST_CHECK_EQUAL (padding.size(), 0);

    padding.extend (range::back, 97);
    BOOST_CHECK_EQUAL (padding.size(), 1);
    BOOST_CHECK_EQUAL (*padding.begin(), 97);

    padding.extend (range::back, 201);
    BOOST_CHECK_EQUAL (padding.size(), 2);
    BOOST_CHECK_EQUAL (*padding.begin(), 97);
    BOOST_CHECK_EQUAL (*(padding.begin() + 1), 201);

    padding.extend (range::back, 127);
    BOOST_CHECK_EQUAL (padding.size(), 3);
    BOOST_CHECK_EQUAL (*padding.begin(), 97);
    BOOST_CHECK_EQUAL (*(padding.begin() + 1), 201);
    BOOST_CHECK_EQUAL (*(padding.begin() + 2), 127);
}

void check_flush_left_throw_quick (utility::thrower & thrower) {
    typedef utility::test_allocator <std::allocator <int>> allocator_type;

    typedef container::allocator_storage <allocator_type> storage_type;

    container::flush_left <storage_type, container::exponential_2> f (allocator_type (thrower), 0);
    check_flush_left_up_to_3 (f);
}

BOOST_AUTO_TEST_CASE (test_flush_left_quick) {
    utility::check_all_throw_points (&check_flush_left_throw_quick);

    {
        typedef container::internal_storage <int, 3> storage_type;
        container::flush_left <storage_type, container::exponential_2> f;
        check_flush_left_up_to_3 (f);
    }
}

template <int ThrowBits> void check_flush_left_exception_safety (utility::thrower & thrower) {
    typedef utility::tracked <int> tracked;
    typedef utility::throwing <tracked,
        bool (ThrowBits & 1),
        bool (ThrowBits & 2),
        bool (ThrowBits & 4),
        bool (ThrowBits & 8),
        bool (ThrowBits & 16),
        bool (ThrowBits & 32)> throwing;

    utility::tracked_registry r;

    {
        typedef utility::test_allocator <std::allocator <throwing>> allocator_type;
        typedef container::allocator_storage <allocator_type> storage_type;

        container::flush_left <storage_type, container::exponential_2> f (
            allocator_type (thrower), 0);

        // \todo This test is not convincing enough.

        int base = 17;
        int i = 0;
        for (; i != 10; ++ i) {
            BOOST_CHECK_EQUAL (f.size(), i);
            f.extend (range::back, throwing (thrower, tracked (r, base + i)));
            BOOST_CHECK_EQUAL (f.size(), i + 1);
            BOOST_CHECK_EQUAL ((f.begin() + i)->content().content(), base + i);
            BOOST_CHECK_EQUAL ((f.end() - 1)->content().content(), base + i);
        }
        for (; i > 0; -- i) {
            BOOST_CHECK_EQUAL (f.size(), i);
            BOOST_CHECK_EQUAL ((f.end() - 1)->content().content(), base + i - 1);
            f.shorten (range::back);
            BOOST_CHECK_EQUAL (f.size(), i - 1);
        }
    }
}

BOOST_AUTO_TEST_CASE (test_flush_left_no_exceptions) {
    std::cout << "Without exceptions:" << std::endl;
    utility::thrower thrower;
    check_flush_left_exception_safety <0> (thrower);
}

template <int ThrowBits> void check_flush_left_exception_safety_all() {
    utility::check_all_throw_points (
        &check_flush_left_exception_safety <ThrowBits>);
    check_flush_left_exception_safety_all <ThrowBits - 1> ();
}

template <> void check_flush_left_exception_safety_all<-1>() {}

BOOST_AUTO_TEST_CASE (test_flush_left_exception_safety) {
    std::cout << "With exceptions:" << std::endl;
    // \todo Re-enable
    // check_flush_left_exception_safety_all <63>();
    utility::check_all_throw_points (
        &check_flush_left_exception_safety <63>);
}

BOOST_AUTO_TEST_SUITE_END()
