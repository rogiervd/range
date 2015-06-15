/*
Copyright 2013-2015 Rogier van Dalen.

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

#ifndef RANGE_COUNT_HPP_INCLUDED
#define RANGE_COUNT_HPP_INCLUDED

#include "rime/core.hpp"
#include "rime/assert.hpp"
#include "rime/cast.hpp"
#include "rime/always.hpp"

#include "core.hpp"

namespace range {

template <class Begin, class End> class count_range;
template <class Begin> class infinite_count_range;

/* Functions. */
// These are used mutually recursively by the classes so they must be defined
// first.

/** \brief
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

/** \brief
Return a range that contains an arithmetic progression of integers.

It starts with \a begin, and finishes one before \a end.
\a begin and \a end can be run-time values or compile-time values.
If they are run-time values, like \c int, then the resulting range is
homogeneous.
If not, then the range is heterogeneous.
\param begin First value in the range.
\param end One-past-last value in the range.
\pre <c>begin \<= end</c>.
*/
template <class Begin, class End>
    inline auto count (Begin const & begin, End const & end)
RETURNS (count_range <Begin, End> (begin, end));

/** \brief
Return a range with \a end elements that starts with 0.

If \a end is a run-time value, then the range is homogeneous.
If \a end is a constant, then the range is heterogeneous.
\param end One-past-last value in the range.
\pre <c>end >= 0</c>.
*/
// Case where End is a run-time value.
template <class End> inline
    typename boost::disable_if <rime::is_constant <End>, count_range <End, End>
>::type count (End const & end)
{ return count (End(), end); }

// Case where End is a constant.
/// \cond DONT_DOCUMENT
template <class End> inline
    typename boost::enable_if <rime::is_constant <End>,
        count_range <rime::constant <typename rime::value <End>::type, 0>, End>
>::type count (End const & end)
{
    rime::constant <typename rime::value <End>::type, 0> begin;
    return count (begin, end);
}
/// \endcond

/* Implementation of ranges. */

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

private:
    friend class helper::member_access;

    auto empty (direction::front) const RETURNS (begin_ == end_);

    auto size (direction::front) const
    RETURNS (rime::cast_value <End> (rime::minus (end_, begin_)));

    Begin first (direction::front) const { return begin_; }

    auto first (direction::back) const
    RETURNS (rime::cast_value <End> (rime::minus (end_, rime::size_t <1>())));

    template <class Increment>
        auto drop (Increment const & increment, direction::front) const
    RETURNS (range::count (
        rime::cast_value <Begin> (rime::plus (begin_, increment)), end_));

    template <class Increment>
        auto drop (Increment const & increment, direction::back) const
    RETURNS (range::count (begin_,
        rime::cast_value <End> (rime::minus (end_, increment))));
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

private:
    friend class helper::member_access;

    rime::false_type empty (direction::front) const { return rime::false_; }

    // size is not defined.

    Begin first (direction::front) const { return begin_; }

    template <class Increment>
        auto drop (Increment const & increment, direction::front) const
    RETURNS (range::count_from (
        rime::cast_value <Begin> (rime::plus (begin_, increment))));
};

namespace operation {
    struct count_range_tag {};
} // namespace operation

template <class Begin, class End>
    struct tag_of_qualified <count_range <Begin, End>>
{ typedef operation::count_range_tag type; };

template <class Begin> struct tag_of_qualified <infinite_count_range <Begin>>
{ typedef operation::count_range_tag type; };

// count() function: needs to be defined after the classes, because
// infinite_count_range needs to be instantiated.

/** \brief
Return an infinite range starting from 0.

This is equivalent to <c>count_from (std::size_t (0))</c>.
*/
inline auto count()
RETURNS (count_from (std::size_t (0)));

} // namespace range

#endif // RANGE_COUNT_HPP_INCLUDED
