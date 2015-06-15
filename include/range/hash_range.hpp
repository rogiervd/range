/*
Copyright 2015 Rogier van Dalen.

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

#ifndef RANGE_HASH_RANGE_HPP_INCLUDED
#define RANGE_HASH_RANGE_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include <boost/functional/hash_fwd.hpp>

#include "utility/overload_order.hpp"
#include "rime/core.hpp"

#include "core.hpp"
#include "for_each.hpp"

namespace range {

namespace callable {

    namespace hash_range_detail {

        class accumulate_hash {
            std::size_t seed_;
        public:
            accumulate_hash (std::size_t seed) : seed_ (seed) {}

            template <class Element> void operator() (Element const & e)
            { boost::hash_combine (seed_, e); }

            std::size_t seed() const { return seed_; }
        };

    } // namespace hash_range_detail

    /** \brief
    A hash function class for ranges that can be used standalone or as a
    template argument to a hash-based container.

    The hash value that is returned uses all elements of the range.
    */
    struct hash_range {
    private:
        /// A fixed random number, used as the hash value for empty ranges.
        static std::size_t constexpr empty_hash
            = std::size_t (0x919af373af67e813);

        template <class Element>
            static std::size_t compute_element_hash (Element const & element)
        { return boost::hash <Element>() (element); }

        template <class Range, class Direction>
            static std::size_t
            compute_hash (Range && range, Direction const & direction,
                rime::true_type empty, overload_order <1> *)
        { return empty_hash; }

        template <class Range, class Direction>
            static std::size_t
            compute_hash (Range && range, Direction const & direction,
                bool empty, overload_order <2> *)
        {
            if (empty)
                return empty_hash;
            // Compute the hash value for the first element.
            hash_range_detail::accumulate_hash accumulate (
                compute_element_hash (range::first (range, direction)));
            // Combine with the hash value of the rest of the elements.
            range::for_each (
                range::drop (std::forward <Range> (range), direction),
                direction, accumulate);
            return accumulate.seed();
        }

    public:
        template <class Range, class Direction> typename
            std::enable_if <is_direction <Direction>::value, std::size_t>::type
            operator() (Range && range, Direction const & direction) const
        {
            auto empty = range::empty (range);
            return compute_hash (
                range::view_once (std::forward <Range> (range), direction),
                direction, empty, pick_overload());
        }

        template <class Range> std::size_t operator() (Range && range) const {
            return operator() (std::forward <Range> (range),
                range::default_direction (range));
        }
    };

    /** \brief
    A hash function class that updates a seed value by taking in all elements
    of a range.
    */
    struct hash_range_combine {

        template <class Range, class Direction>
            typename std::enable_if <is_direction <Direction>::value>::type
            operator() (Range && range, Direction const & direction,
                std::size_t & seed) const
        {
            hash_range_detail::accumulate_hash accumulate (seed);
            range::for_each (
                range::view_once (std::forward <Range> (range), direction),
                direction, accumulate);
            seed = accumulate.seed();
        }

        template <class Range>
            void operator() (Range && range, std::size_t & seed) const
        {
            operator() (std::forward <Range> (range),
                range::default_direction (range), seed);
        }

    };

} // namespace callable


/** \brief
Calculate the combined hash value of the elements of a range.

If the range is empty, a fixed value is returned.
Otherwise, the hash value of the first element is computed, and combined with
the rest of the elements using boost::hash_combine.
boost/functional/hash.hpp must be included to use this.

Any range that has the same types of elements and the same values will yield
the same hash value.
Empty ranges yield a different value than ranges with one zero-valued POD
element.
If the range has a length of 1, then the hash value of its element is returned.
(Note that Boost.Hash is an identity function for POD elements with fewer bits
than std::size_t.)

\param range The range of elements to compute the hash for.
\param direction (optional)
    The direction to traverse the range in.
    If this is not given the range will be traversed in the default direction.
*/
static auto constexpr hash_range = callable::hash_range();

/** \brief
Update the seed to include the take into account the hash value of the elements
of the range.

boost::hash_combine is called for each element.
boost/functional/hash.hpp must be included to use this.

\note
This returns a different value from \ref hash_range with seed \c 0, just like
boost::hash returns a different value from boost::hash_combine.

\param range The range of elements to compute the hash for.
\param direction (optional)
    The direction to traverse the range in.
    If this is not given the range will be traversed in the default direction.
\param seed
    The seed value for computing the hash, which will be updated to include the
    elements of the range.
*/
static auto constexpr hash_range_combine = callable::hash_range_combine();

} // namespace range

#endif // RANGE_HASH_RANGE_HPP_INCLUDED
