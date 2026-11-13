// SPDX-FileCopyrightText: Copyright 2013-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_COUNT_HPP_INCLUDED
#define RANGE_COUNT_HPP_INCLUDED

#include <cstddef>

#include "boost/core/enable_if.hpp"

#include "rime/assert.hpp"
#include "rime/cast.hpp"
#include "rime/core.hpp"

#include "utility/returns.hpp"

#include "range/direction.hpp"

#include "core.hpp"

namespace range {

template <class Start, class Finish> class count_range;
template <class Start> class infinite_count_range;

/* Functions. */
// These are used mutually recursively by the classes so they must be defined
// first.

/** \brief
Return an infinite range that contains an arithmetic progression of integers.

It starts with \a start, the next element is <c>start + 1</c>, etc.
If \a start is a run-time value, then the range is homogeneous, i.e., the other
elements have the same type.
If \a start is a constant, the other elements are also constants of the same
type.
However, if \a start is a constant and <c>drop (increment, r)</c> is called with
non-constant increment, then the resulting range will have a run-time value
of the same type as the value type of the original constant.
*/
template <class Start>
inline infinite_count_range<Start> count_from(Start const & start)
{
    return infinite_count_range<Start>(start);
}

/** \brief
Return a range that contains an arithmetic progression of integers.

It starts with \a start, and finishes one before \a finish.
\a start and \a finish can be run-time values or compile-time values.
If they are run-time values, like \c int, then the resulting range is
homogeneous.
If not, then the range is heterogeneous.
\param start First value in the range.
\param finish One-past-last value in the range.
\pre <c>start \<= finish</c>.
*/
template <class Start, class Finish>
inline auto count(Start const & start, Finish const & finish)
    RETURNS(count_range<Start, Finish>(start, finish));

/** \brief
Return a range with \a finish elements that starts with 0.

If \a finish is a run-time value, then the range is homogeneous.
If \a finish is a constant, then the range is heterogeneous.
\param finish One-past-last value in the range.
\pre <c>finish >= 0</c>.
*/
// Case where Finish is a run-time value.
template <class Finish> inline typename boost::disable_if<
    rime::is_constant<Finish>, count_range<Finish, Finish>>::type
    count(Finish const & finish)
{
    return count(Finish(), finish);
}

// Case where Finish is a constant.
/// \cond DONT_DOCUMENT
template <class Finish> inline typename boost::enable_if<
    rime::is_constant<Finish>,
    count_range<
        rime::constant<typename rime::value<Finish>::type, 0>, Finish>>::type
    count(Finish const & finish)
{
    rime::constant<typename rime::value<Finish>::type, 0> start;
    return count(start, finish);
}
/// \finishcond

/* Implementation of ranges. */

template <class Start, class Finish> class count_range
{
    Start start_;
    Finish finish_;

public:
    count_range(Start const & start, Finish const & finish)
    : start_(start), finish_(finish)
    {
        rime::assert_(!rime::less(finish, start));
    }

    count_range(count_range const &) = default;
    count_range(count_range &&) = default;
    count_range & operator=(count_range const &) = default;
    count_range & operator=(count_range &&) = default;

    Start start() const { return start_; }
    Finish finish() const { return finish_; }

private:
    friend class helper::member_access;

    auto empty(direction::front) const RETURNS(start_ == finish_);

    auto size(direction::front) const
        RETURNS(rime::cast_value<Finish>(rime::minus(finish_, start_)));

    Start first(direction::front) const { return start_; }

    auto first(direction::back) const RETURNS(
        rime::cast_value<Finish>(rime::minus(finish_, rime::size_t<1>())));

    template <class Increment>
    auto drop(Increment const & increment, direction::front) const RETURNS(
        range::count(
            rime::cast_value<Start>(rime::plus(start_, increment)), finish_));

    template <class Increment>
    auto drop(Increment const & increment, direction::back) const RETURNS(
        range::count(
            start_, rime::cast_value<Finish>(rime::minus(finish_, increment))));
};

template <class Start> class infinite_count_range
{
    Start start_;

public:
    infinite_count_range(Start const & start) : start_(start) {}

    infinite_count_range(infinite_count_range const &) = default;
    infinite_count_range(infinite_count_range &&) = default;
    infinite_count_range & operator=(infinite_count_range const &) = default;
    infinite_count_range & operator=(infinite_count_range &&) = default;

    Start start() const { return start_; }

private:
    friend class helper::member_access;

    rime::false_type empty(direction::front) const { return rime::false_; }

    // size is not defined.

    Start first(direction::front) const { return start_; }

    template <class Increment>
    auto drop(Increment const & increment, direction::front) const RETURNS(
        range::count_from(
            rime::cast_value<Start>(rime::plus(start_, increment))));
};

namespace count_operation {
    struct count_range_tag {};
}  // namespace count_operation

template <class Start, class Finish>
struct tag_of_qualified<count_range<Start, Finish>>
{
    typedef count_operation::count_range_tag type;
};

template <class Start> struct tag_of_qualified<infinite_count_range<Start>>
{
    typedef count_operation::count_range_tag type;
};

// count() function: needs to be defined after the classes, because
// infinite_count_range needs to be instantiated.

/** \brief
Return an infinite range starting from 0.

This is equivalent to <c>count_from (std::size_t (0))</c>.
*/
inline auto count() RETURNS(count_from(std::size_t(0)));

}  // namespace range

#endif  // RANGE_COUNT_HPP_INCLUDED
