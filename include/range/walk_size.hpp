/*
Copyright 2013 Rogier van Dalen.

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

#ifndef RANGE_WALK_SIZE_HPP_INCLUDED
#define RANGE_WALK_SIZE_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include "core.hpp"

namespace range {

namespace callable {

    struct walk_size {
        template <class Direction, class Range> typename
            result_of_or <range::callable::size (Direction, Range)>::type
        operator() (Direction const & direction, Range && range) const
        { return range::size (direction, std::forward <Range> (range)); }

        template <class Direction, class Range>
            typename boost::disable_if <
                range::has <range::callable::size (Direction, Range)>,
                std::size_t>::type
        operator() (Direction const & direction, Range && range) const {
            std::size_t size = 0;
            auto current = range::view (range);
            while (!range::empty (current)) {
                current = range::drop (direction, current);
                ++ size;
            }
            return size;
        }

        // No direction: use default_direction.
        template <class Range> auto operator() (Range && range) const
        -> decltype (std::declval <walk_size>() (
            range::default_direction (range), std::declval <Range>()))
        {
            return (*this) (
                range::default_direction (range), std::forward <Range> (range));
        }
    };

} // namespace callable

/**
walk_size (range) or walk_size (direction, range) computes the number of
elements in any type of range.
If the range has a size() operation, that is used.
If not, then the drop() operation is used until the range is empty, and the
number of steps is counted.
\todo Currently only works on homogeneous ranges; use fold to remedy this.
*/
static const auto walk_size = callable::walk_size();

} // namespace range

#endif // RANGE_WALK_SIZE_HPP_INCLUDED

