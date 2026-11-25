// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Support for Boost.Hash2: feed the elements of a range into a hash algorithm.
*/

#ifndef RANGE_HASH_APPEND_HPP_INCLUDED
#define RANGE_HASH_APPEND_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <utility>

#include "boost/hash2/hash_append.hpp"

#include "range/core.hpp"
#include "range/direction.hpp"
#include "range/for_each.hpp"

namespace range {

namespace callable {

    namespace hash_append_detail {

        template <class Hash, class Flavor> class append_element
        {
            Hash & hash_;
            Flavor const & flavor_;
            std::size_t & count_;

        public:
            append_element(
                Hash & hash, Flavor const & flavor, std::size_t & count)
            : hash_(hash), flavor_(flavor), count_(count)
            {}

            template <class Element>
            void operator()(Element const & element) const
            {
                boost::hash2::hash_append(hash_, flavor_, element);
                ++count_;
            }
        };

    }  // namespace hash_append_detail

    struct hash_append
    {
        template <class Hash, class Flavor, class Range, class Direction>
        std::enable_if_t<is_direction<Direction>::value> operator()(
            Hash & hash, Flavor const & flavor, Range && range,
            Direction const & direction) const
        {
            std::size_t count = 0;
            range::for_each(
                range::view_once(std::forward<Range>(range), direction),
                direction,
                hash_append_detail::append_element<Hash, Flavor>(
                    hash, flavor, count));
            boost::hash2::hash_append_size(hash, flavor, count);
        }

        template <class Hash, class Flavor, class Range> void operator()(
            Hash & hash, Flavor const & flavor, Range && range) const
        {
            operator()(
                hash, flavor, std::forward<Range>(range),
                range::default_direction(range));
        }
    };

}  // namespace callable

/** \brief
Append the elements of a range to a Boost.Hash2 hash algorithm.

Each element is passed to \c boost::hash2::hash_append, and then the number of
elements is passed to \c boost::hash2::hash_append_size.
This is the same message that Boost.Hash2 produces for a standard container,
so a range of elements yields the same hash value as, say, a \c std::vector
with the same elements.
Any ranges that have the same types of elements and the same values will also
yield the same hash value.

This can be used in a \c tag_invoke overload that adds Boost.Hash2 support to a
type that contains a range.

\param hash The hash algorithm, for example \c boost::hash2::fnv1a_64.
\param flavor
    The Boost.Hash2 flavor, for example \c boost::hash2::default_flavor().
\param range The range of elements.
\param direction (optional)
    The direction to traverse the range in.
    If this is not given the range will be traversed in the default direction.
*/
static auto constexpr hash_append = callable::hash_append();

}  // namespace range

#endif  // RANGE_HASH_APPEND_HPP_INCLUDED
