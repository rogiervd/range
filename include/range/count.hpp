/*
Copyright 2013, 2014 Rogier van Dalen.

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
#include "rime/assert.hpp"
#include "rime/cast.hpp"
#include "rime/always.hpp"

#include "core.hpp"

namespace range {

template <class Begin, class End> class count_range {
    Begin begin_;
    End end_;
public:
    count_range (Begin const & begin, End const & end)
    : begin_ (begin), end_ (end) {
        rime::assert_ (!rime::less (end, begin));
    }

    count_range (count_range const &) = default;
    count_range (count_range &&) = default;
    count_range & operator= (count_range const &) = default;
    count_range & operator= (count_range &&) = default;

    Begin begin() const { return begin_; }
    End end() const { return end_; }
};

template <class Begin> class infinite_count_range {
    Begin begin_;
public:
    infinite_count_range (Begin const & begin)
    : begin_ (begin) {}

    infinite_count_range (infinite_count_range const &) = default;
    infinite_count_range (infinite_count_range &&) = default;
    infinite_count_range & operator= (infinite_count_range const &) = default;
    infinite_count_range & operator= (infinite_count_range &&) = default;

    Begin begin() const { return begin_; }
};

template <bool Infinite> struct count_range_tag;

template <class Begin, class End>
    struct tag_of_qualified <count_range <Begin, End>>
{ typedef count_range_tag <false> type; };
template <class Begin> struct tag_of_qualified <infinite_count_range <Begin>>
{ typedef count_range_tag <true> type; };

/**
Return an infinite range that contains an arithmetic progression of integers.
It starts with \a begin, the next element is <c>begin + 1</c>, etc.
If \a begin is a run-time value, then the range is homogeneous, i.e., the other
elements have the same type.
If \a begin is a constant, the other elements are also constants of the same
type.
However, if \a begin is a constant and <c>drop (increment, r)</c> is called with
non-constant increment, then the resulting range will have a run-time value
of the same type as the value type of the original constant.
*/
template <class Begin> inline
    infinite_count_range <Begin> count_from (Begin const & begin)
{ return infinite_count_range <Begin> (begin); }

/**
Return an infinite range starting from 0 (as a \c std::size_t).
Essentially equivalent to <c>count_from (0)</c>.
*/
inline auto count()
RETURNS (count_from (std::size_t (0)));

/**
Return a range that contains an arithmetic progression of integers.
It starts with \a begin, and finishes one before \a end.
\a begin and \a end can be run-time values or compile-time values.
If they are run-time values, like \c int, then the resulting range is
homogeneous.
If not, then the range is heterogeneous.
\param begin First value in the range.
\param end One-before-last value in the range.
\pre <c>begin \<= end</c>.
*/
template <class Begin, class End>
    inline auto count (Begin const & begin, End const & end)
RETURNS (count_range <Begin, End> (begin, end));

/**
Return a range with \a end elements that starts with 0.
If \a end is a run-time value, then the range is homogeneous.
If \a end is a constant, then the range is heterogeneous.
\param end One-before-last value in the range.
\pre <c>end >= 0</c>.
*/
// Case where End is a run-time value.
template <class End> inline
    typename boost::disable_if <rime::is_constant <End>, count_range <End, End>
>::type count (End const & end)
{ return count (End(), end); }

// Case where End is a constant.
template <class End> inline
    typename boost::enable_if <rime::is_constant <End>,
        count_range <rime::constant <typename rime::value <End>::type, 0>, End>
>::type count (End const & end)
{
    rime::constant <typename rime::value <End>::type, 0> begin;
    return count (begin, end);
}

namespace operation {

    // default_direction just returns front.

    // empty.
    template <> struct empty <count_range_tag <false>, direction::front> {
        template <class CountRange>
            auto operator() (direction::front, CountRange const & r) const
        RETURNS (r.begin() == r.end());
    };
    template <> struct empty <count_range_tag <true>, direction::front>
    : rime::callable::always_default <rime::false_type> {};

    // size.
    // Not implemented for infinite_count_range.
    template <> struct size <count_range_tag <false>, direction::front> {
        template <class Begin, class End>
            auto operator() (direction::front,
                count_range <Begin, End> const & r) const
        RETURNS (rime::cast_value <End> (rime::minus (r.end(), r.begin())));
    };

    // first.
    // front: the same for finite and infinite.
    template <bool Infinite>
        struct first <count_range_tag <Infinite>, direction::front>
    {
        template <class CountRange>
            auto operator() (direction::front, CountRange const & r) const
        RETURNS (r.begin());
    };
    // back: only for finite count ranges.
    template <> struct first <count_range_tag <false>, direction::back>
    {
        template <class Begin, class End>
            auto operator() (direction::back,
                count_range <Begin, End> const & r) const
        RETURNS (rime::cast_value <End> (rime::minus (r.end(), one_type())));
    };

    // drop.
    template <class Increment>
        struct drop <count_range_tag <true>, direction::front, Increment>
    {
        // Constant: allow a change of type.
        template <class Begin>
            auto operator() (direction::front, Increment const & increment,
                infinite_count_range <Begin> const & r) const
        RETURNS (range::count_from (
            rime::cast_value <Begin> (rime::plus (r.begin(), increment))));
    };

    template <class Increment>
        struct drop <count_range_tag <false>, direction::front, Increment>
    {
        // Constant: allow a change of type.
        template <class Begin, class End>
            auto operator() (direction::front, Increment const & increment,
                count_range <Begin, End> const & r) const
        RETURNS (range::count (
            rime::cast_value <Begin> (rime::plus (r.begin(), increment)),
            r.end()));
    };
    template <class Increment>
        struct drop <count_range_tag <false>, direction::back, Increment>
    {
        // Constant: allow a change of type.
        template <class Begin, class End>
            auto operator() (
                direction::back, Increment const & increment,
                count_range <Begin, End> const & r) const
        RETURNS (range::count (r.begin(),
            rime::cast_value <End> (rime::minus (r.end(), increment))));
    };

} // namespace operation

} // namespace range

#endif // RANGE_COUNT_HPP_INCLUDED
