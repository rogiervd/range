/*
Copyright 2013, 2015 Rogier van Dalen.

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

/** \file
Provide a range wrapper for testing the Range library.
*/

#ifndef RANGE_TEST_UNIQUE_RANGE_HPP_INCLUDED
#define RANGE_TEST_UNIQUE_RANGE_HPP_INCLUDED

#include "utility/returns.hpp"

#include "rime/assert.hpp"

#include "range/core.hpp"
#include "range/helper/underlying.hpp"

/**
Wrapper range that cannot be copied, only moved, even when the underlying range
can.
drop() is also only defined on an rvalue-reference range.
This makes it a good model for ranges such as file ranges, which one would
prefer not to hang on to parts in memory that have already been read, say.

\tparam Underlying
    The underlying range.
    first() and drop() should be defined for it.
\tparam OneTime
    Whether the first element can be produced only once.
    If this is \c true, then first is only defined on rvalue-reference ranges.
*/
template <class Underlying, bool OneTime> class unique_range {
public:
    typedef Underlying underlying_type;
private:
    bool valid_;
    Underlying underlying_;

    template <class Wrapper>
        friend class range::helper::callable::get_underlying;

public:
    explicit unique_range (Underlying const & underlying)
    : valid_ (true), underlying_ (underlying) {}

    unique_range (unique_range const &) = delete;

    unique_range (unique_range && other)
    : valid_ (true), underlying_ (std::move (other.underlying_))
    { other.valid_ = false; }

    unique_range & operator = (unique_range const &) = delete;

    unique_range & operator = (unique_range && other) {
        underlying_ = std::move (other.underlying_);
        other.valid_ = false;
        this->valid_ = true;
        return *this;
    }

    bool valid() const { return valid_; }

    void invalidate() { valid_ = false; }
};

/**
Produce a view on the range that cannot be copied, only moved.
drop() must therefore move its argument.
first() is enabled for lvalue references, though.
The original range is not exposed by rvalue reference, and can be re-used.
It is just the view that is forced to be moveable.
*/
template <class Range> inline auto unique_view (Range && range)
RETURNS (unique_range <typename range::decayed_result_of <
    range::callable::view (Range)>::type, false> (
        range::view (std::forward <Range> (range))));

/**
Produce a view on the range that cannot be copied, only moved, and chop() must
be used.
The original range is not exposed by rvalue reference, and can be re-used.
It is just the view that is forced to be moveable.
\internal
*/
template <class Range> inline auto one_time_view (Range && range)
RETURNS (unique_range <typename range::decayed_result_of <
    range::callable::view (Range)>::type, true> (
        range::view (std::forward <Range> (range))));

template <bool OneTime> struct unique_range_tag {};

namespace range {
    template <class Underlying, bool OneTime>
        struct tag_of_qualified <unique_range <Underlying, OneTime>>
    { typedef unique_range_tag <OneTime> type; };
} // namespace range

template <bool OneTime, class Range> inline
    auto implement_default_direction (
        unique_range_tag <OneTime> const &, Range && range)
RETURNS (range::default_direction (
    range::helper::get_underlying <Range> (range)));

template <bool OneTime, class Range, class Direction>
    auto implement_empty (unique_range_tag <OneTime> const &,
        Range && range, Direction const & direction)
RETURNS (range::empty (
    range::helper::get_underlying <Range> (range), direction));

template <bool OneTime, class Range, class Direction>
    auto implement_size (unique_range_tag <OneTime> const &,
        Range && range, Direction const & direction)
RETURNS (range::size (
    range::helper::get_underlying <Range> (range), direction));

// first: only if OneTime = false.
// Otherwise this is implementented automatically through "chop".
template <class Range, class Direction>
    auto implement_first (unique_range_tag <false> const &,
        Range && range, Direction const & direction)
RETURNS (range::first (
    range::helper::get_underlying <Range> (range), direction));

// drop() only takes an rvalue range, pilfers it, and deactivates the original
// range.
template <bool OneTime, class Underlying, class Increment, class Direction,
    class Result = unique_range <typename std::decay <
        decltype (range::drop (std::declval <Underlying>(),
            std::declval <Increment>(), std::declval <Direction>()))>::type,
        OneTime>>
Result implement_drop (unique_range_tag <OneTime> const &,
    unique_range <Underlying, OneTime> && r,
    Increment const & increment, Direction const & direction)
{
    return Result (range::drop (range::helper::get_underlying <
        unique_range <Underlying, OneTime>> (r), increment, direction));
}

// chop() only takes an rvalue range, pilfers it, and deactivates the original
// range.
// It is only implemented if chop is implemented for the underlying range.
template <bool OneTime, class Underlying, class Direction,
    class UnderlyingChopped = decltype (
        range::chop (std::declval <Underlying>(), std::declval <Direction>())),
    class First = typename UnderlyingChopped::first_type,
    class UnderlyingRest = typename UnderlyingChopped::rest_type,
    class ResultRange = unique_range <UnderlyingRest, OneTime>,
    class Result = range::chopped <First, ResultRange>>
Result implement_chop (unique_range_tag <OneTime> const &,
    unique_range <Underlying, OneTime> && r, Direction const & direction)
{
    UnderlyingChopped underlying_chopped = range::chop (
        range::helper::get_underlying <
            unique_range <Underlying, OneTime>> (r),
        direction);
    return Result (underlying_chopped.move_first(),
        ResultRange (underlying_chopped.move_rest()));
}

#endif  // RANGE_TEST_UNIQUE_RANGE_HPP_INCLUDED
