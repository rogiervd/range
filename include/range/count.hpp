/*
Copyright 2013 Rogier van Dalen.

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

#ifndef RANGE_COUNT_HPP_INCLUDED
#define RANGE_COUNT_HPP_INCLUDED

#include "rime/core.hpp"
#include "rime/cast.hpp"

#include "core.hpp"

namespace range {

template <class First> class count_range {
    First first_;
public:
    count_range (First const & first)
    : first_ (first) {}

    count_range (count_range const &) = default;
    count_range (count_range &&) = default;
    count_range & operator= (count_range const &) = default;
    count_range & operator= (count_range &&) = default;

    First const & first() const { return first_; }
};

struct count_range_tag;

template <class First> struct tag_of_unqualified <count_range <First>>
{ typedef count_range_tag type; };

/**
Return an infinite range that contains an arithmetic progression of integers.
It starts with "first", the nexte element is first + 1, etc.
If first is a run-time value, then the range is homogeneous, i.e., the other
elements have the same type.
If first is a constant, the other elements are also constants of the same
type.
However, if first is a constant and the drop (increment, r) is called with
non-constant increment, then the resulting range will have a run-time value
of the same type as the value type of the original constant.
*/
template <class First> inline
    count_range <First> count (First const & first)
{ return count_range <First> (first); }

inline auto count()
RETURNS (count (std::size_t (0)));

namespace operation {

    // default_direction just returns front.

    template <> struct empty <count_range_tag, direction::front> {
        template <class CountRange>
            auto operator() (direction::front, CountRange const &) const
        RETURNS (rime::false_);
    };

    // size is not implemented: count_range has infinite length

    template <> struct first <count_range_tag, direction::front> {
        template <class First>
            First const & operator() (
                direction::front, count_range <First> const & r) const
        { return r.first(); }
    };

    template <class Increment>
        struct drop <count_range_tag, direction::front, Increment>
    {
        // Constant: allow a change of type.
        template <class First>
            auto operator() (
                direction::front, Increment const & increment,
                count_range <First> const & r) const
        RETURNS (range::count (
            rime::cast_value <First> (rime::plus (r.first(), increment))));
    };

} // namespace operation

} // namespace range

#endif // RANGE_COUNT_HPP_INCLUDED

