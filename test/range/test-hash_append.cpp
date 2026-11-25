// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_range_hash_append
#include "boost/test/unit_test.hpp"

#include "range/hash_append.hpp"

#include <cstdint>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include "boost/hash2/fnv1a.hpp"
#include "boost/hash2/hash_append.hpp"

#include "range/core.hpp"
#include "range/std/list.hpp"
#include "range/std/vector.hpp"
#include "range/tuple.hpp"

BOOST_AUTO_TEST_SUITE(test_range_hash_append)

using boost::hash2::fnv1a_64;

// Hash a range with range::hash_append.
template <class Flavor, class... Arguments>
std::uint64_t hash_range_with_flavor(
    Flavor const & flavor, Arguments &&... arguments)
{
    fnv1a_64 hash;
    range::hash_append(hash, flavor, std::forward<Arguments>(arguments)...);
    return hash.result();
}

template <class... Arguments>
std::uint64_t hash_range(Arguments &&... arguments)
{
    return hash_range_with_flavor(
        boost::hash2::default_flavor(), std::forward<Arguments>(arguments)...);
}

// Hash an object directly with Boost.Hash2.
template <class Type, class Flavor = boost::hash2::default_flavor>
std::uint64_t hash_object(Type const & object, Flavor const & flavor = Flavor())
{
    fnv1a_64 hash;
    boost::hash2::hash_append(hash, flavor, object);
    return hash.result();
}

BOOST_AUTO_TEST_CASE(test_range_hash_append_same_as_container)
{
    // The message must be the same as Boost.Hash2 produces for a container.
    std::vector<int> empty;
    BOOST_CHECK_EQUAL(hash_range(empty), hash_object(empty));
    BOOST_CHECK_EQUAL(hash_range(empty, range::front), hash_object(empty));

    std::vector<int> v {3, 7, -2};
    BOOST_CHECK_EQUAL(hash_range(v), hash_object(v));
    BOOST_CHECK_EQUAL(hash_range(range::view(v)), hash_object(v));

    std::list<double> l {1.5, -0.25};
    BOOST_CHECK_EQUAL(hash_range(l), hash_object(l));

    std::vector<std::string> strings {"hello", "", "world"};
    BOOST_CHECK_EQUAL(hash_range(strings), hash_object(strings));

    // Also with a different flavor.
    BOOST_CHECK_EQUAL(
        hash_range_with_flavor(boost::hash2::big_endian_flavor(), v),
        hash_object(v, boost::hash2::big_endian_flavor()));
    BOOST_CHECK(
        hash_range_with_flavor(boost::hash2::big_endian_flavor(), v)
        != hash_range_with_flavor(boost::hash2::little_endian_flavor(), v));
}

BOOST_AUTO_TEST_CASE(test_range_hash_append_direction)
{
    std::vector<int> v {3, 7, -2};
    std::vector<int> reversed {-2, 7, 3};

    BOOST_CHECK_EQUAL(hash_range(v, range::front), hash_object(v));
    BOOST_CHECK_EQUAL(hash_range(v, range::back), hash_object(reversed));
    BOOST_CHECK(hash_range(v, range::front) != hash_range(v, range::back));
}

BOOST_AUTO_TEST_CASE(test_range_hash_append_heterogeneous)
{
    // Ranges with the same element types and values yield the same message.
    std::vector<int> v {3, 7};
    range::tuple<int, int> t(3, 7);
    BOOST_CHECK_EQUAL(hash_range(t), hash_object(v));
    BOOST_CHECK_EQUAL(hash_range(t, range::back), hash_range(v, range::back));

    range::tuple<> empty;
    BOOST_CHECK_EQUAL(hash_range(empty), hash_object(std::vector<int>()));

    // The size is part of the message, so this is different from v.
    range::tuple<int, int, int> t3(3, 7, 0);
    BOOST_CHECK(hash_range(t3) != hash_object(v));

    // Different element types.
    range::tuple<int, double, std::string> mixed(1, 2.5, "three");
    fnv1a_64 hash;
    boost::hash2::hash_append(hash, {}, 1);
    boost::hash2::hash_append(hash, {}, 2.5);
    boost::hash2::hash_append(hash, {}, std::string("three"));
    boost::hash2::hash_append_size(hash, {}, 3);
    BOOST_CHECK_EQUAL(hash_range(mixed), hash.result());
}

BOOST_AUTO_TEST_SUITE_END()
