/*
Copyright 2015 Rogier van Dalen.

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

        template <class Direction, class Range>
            static std::size_t
            compute_hash (Direction const & direction, Range && range,
                rime::true_type empty, utility::overload_order <1> *)
        { return empty_hash; }

        template <class Direction, class Range>
            static std::size_t
            compute_hash (Direction const & direction, Range && range,
                bool empty, utility::overload_order <2> *)
        {
            if (empty)
                return empty_hash;
            // Compute the hash value for the first element.
            hash_range_detail::accumulate_hash accumulate (
                compute_element_hash (range::first (direction, range)));
            // Combine with the hash value of the rest of the elements.
            range::for_each (direction, accumulate,
                range::drop (std::forward <Range> (range)));
            return accumulate.seed();
        }

    public:
        template <class Direction, class Range> typename
            std::enable_if <is_direction <Direction>::value, std::size_t>::type
            operator() (Direction const & direction, Range && range) const
        {
            auto empty = range::empty (range);
            return compute_hash (direction,
                range::view_once (std::forward <Range> (range)), empty,
                utility::pick_overload());
        }

        template <class Range> std::size_t operator() (Range && range) const {
            return operator() (range::default_direction (range),
                std::forward <Range> (range));
        }
    };

    /** \brief
    A hash function class that updates a seed value by taking in all elements
    of a range.
    */
    struct hash_range_combine {

        template <class Direction, class Range>
            typename std::enable_if <is_direction <Direction>::value>::type
            operator() (Direction const & direction, std::size_t & seed,
                Range && range) const
        {
            hash_range_detail::accumulate_hash accumulate (seed);
            range::for_each (direction, accumulate,
                range::view_once (std::forward <Range> (range)));
            seed = accumulate.seed();
        }

        template <class Range>
            void operator() (std::size_t & seed, Range && range) const
        {
            operator() (range::default_direction (range), seed,
                std::forward <Range> (range));
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

\param direction (optional)
    The direction to traverse the range in.
    If this is not given the range will be traversed in the default direction.
\param Range The range of elements to compute the hash for.
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

\param direction (optional)
    The direction to traverse the range in.
    If this is not given the range will be traversed in the default direction.
\param Range The range of elements to compute the hash for.
*/
static auto constexpr hash_range_combine = callable::hash_range_combine();

} // namespace range

#endif // RANGE_HASH_RANGE_HPP_INCLUDED
