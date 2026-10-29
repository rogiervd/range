// SPDX-FileCopyrightText: Copyright 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_WALK_SIZE_HPP_INCLUDED
#define RANGE_WALK_SIZE_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include "core.hpp"

namespace range {

namespace callable {

    struct walk_size
    {
        // Use size.
        template <class Range, class Direction>
        typename result_of<range::callable::size(Range, Direction)>::type
            operator()(Range && range, Direction const & direction) const
        {
            return range::size(std::forward<Range>(range), direction);
        }

        template <class Range, class Direction> typename boost::disable_if<
            range::has<range::callable::size(Range, Direction)>,
            std::size_t>::type
            operator()(Range && range, Direction const & direction) const
        {
            std::size_t size = 0;
            auto current = range::view(range, direction);
            while (!range::empty(current, direction)) {
                current = range::drop(current, direction);
                ++size;
            }
            return size;
        }

        // No direction: use default_direction.
        template <class Range> auto operator()(Range && range) const
            -> decltype(std::declval<walk_size>()(
                std::declval<Range>(), range::default_direction(range)))
        {
            return (*this)(
                std::forward<Range>(range), range::default_direction(range));
        }
    };

}  // namespace callable

/** \brief
Computes the number of elements in a range.

If the range has a \c size() operation, that is used.
If not, then the \c drop() operation is used until the range is empty, and the
number of steps is counted.

\todo Currently only works on homogeneous ranges; use fold to remedy this.

\param range The range to count the number of elements of.
\param direction The direction to traverse the range in.
*/
static auto const walk_size = callable::walk_size();

}  // namespace range

#endif  // RANGE_WALK_SIZE_HPP_INCLUDED
