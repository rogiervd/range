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

#include "core.hpp"
#include "for_each.hpp"

namespace range {

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
Calculate the combined hash value of the elements of a range.

boost::hash_combine is called for each element.
boost/functional/hash.hpp must be included to use this.

\param direction The direction to traverse the range in.
\param Range The range of elements to compute the hash for.
*/
template <class Direction, class Range> inline
    typename std::enable_if <is_direction <Direction>::value, std::size_t>::type
    hash_range (Direction const & direction, Range && range)
{
    hash_range_detail::accumulate_hash accumulate (0);
    for_each (direction, accumulate, view_once (std::forward <Range> (range)));
    return accumulate.seed();
}

/** \brief
Calculate the combined hash value of the elements of a range.

boost::hash_combine is called for each element.
boost/functional/hash.hpp must be included to use this.

The range will be traversed in horizontal direction.

\param Range The range of elements to compute the hash for.
*/
template <class Range> inline std::size_t hash_range (Range && range)
{ return hash_range (default_direction (range), std::forward <Range> (range)); }

/** \brief
Update the seed to include the take into account the hash value of the elements
of the range.

boost::hash_combine is called for each element.
boost/functional/hash.hpp must be included to use this.

\param direction The direction to traverse the range in.
\param Range The range of elements to compute the hash for.
*/
template <class Direction, class Range> inline
    typename std::enable_if <is_direction <Direction>::value>::type
    hash_range (Direction const & direction, std::size_t & seed, Range && range)
{
    hash_range_detail::accumulate_hash accumulate (seed);
    for_each (direction, accumulate, view_once (std::forward <Range> (range)));
    seed = accumulate.seed();
}

/** \brief
Update the seed to include the take into account the hash value of the elements
of the range.

boost::hash_combine is called for each element.
boost/functional/hash.hpp must be included to use this.

The range will be traversed in horizontal direction.

\param Range The range of elements to compute the hash for.
*/
template <class Range> inline
    void hash_range (std::size_t & seed, Range && range)
{ hash_range (default_direction (range), seed, std::forward <Range> (range)); }

} // namespace range

#endif // RANGE_HASH_RANGE_HPP_INCLUDED
