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

/** \file
Provide a range wrapper for testing the Range library.
*/

#ifndef RANGE_TEST_UNIQUE_RANGE_HPP_INCLUDED
#define RANGE_TEST_UNIQUE_RANGE_HPP_INCLUDED

#include "utility/returns.hpp"

#include "rime/assert.hpp"

#include "range/core.hpp"
#include "range/detail/underlying.hpp"

namespace unique_range_detail { namespace callable { struct get_underlying; }}

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
private:
    bool valid_;
    typedef Underlying underlying_type;
    Underlying underlying_;

    friend struct unique_range_detail::callable::get_underlying;

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

namespace unique_range_detail {

namespace callable {

    struct get_underlying {
    public:
        template <class UniqueRange>
            typename UniqueRange::underlying_type const &
            operator() (UniqueRange const & range) const
        {
            assert (range.valid());
            return range.underlying_;
        }

        template <class UniqueRange> typename UniqueRange::underlying_type &
            operator() (UniqueRange & range) const
        {
            assert (range.valid());
            return range.underlying_;
        }

        template <class UniqueRange> typename UniqueRange::underlying_type &&
            operator() (UniqueRange && range) const
        {
            assert (range.valid());
            range.invalidate();
            return std::move (range.underlying_);
        }
    };

} // namespace unique_range_detail

static const auto get_underlying = callable::get_underlying();

} // namespace callable

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

template <class UnderlyingTag, bool OneTime, bool RValue>
    struct unique_range_tag;

namespace range {

template <class Underlying, bool OneTime>
    struct tag_of_qualified <unique_range <Underlying, OneTime>>
{
    typedef unique_range_tag <typename
        tag_of <Underlying &>::type, OneTime, false> type;
};

template <class Underlying, bool OneTime>
    struct tag_of_qualified <unique_range <Underlying, OneTime>, temporary>
{
    typedef unique_range_tag <typename
        tag_of <Underlying &>::type, OneTime, true> type;
};

namespace operation {

    template <class UnderlyingTag, bool OneTime, bool RValue>
        struct default_direction <
            unique_range_tag <UnderlyingTag, OneTime, RValue>>
    {
        template <class UniqueRange>
            typename result_of <callable::default_direction (
                unique_range_detail::callable::get_underlying (UniqueRange))
            >::type operator() (UniqueRange && range) const
        {
            return range::default_direction (
                unique_range_detail::get_underlying (range));
        }
    };

    template <class UnderlyingTag, bool OneTime, bool RValue, class Direction>
        struct empty <unique_range_tag <UnderlyingTag, OneTime, RValue>,
            Direction>
    {
        template <class UniqueRange>
            typename result_of <callable::empty (Direction,
                unique_range_detail::callable::get_underlying (UniqueRange))
            >::type
            operator() (Direction const & direction, UniqueRange && range) const
        {
            return range::empty (direction,
                unique_range_detail::get_underlying (range));
        }
    };

    template <class UnderlyingTag, bool OneTime, bool RValue, class Direction>
        struct size <unique_range_tag <UnderlyingTag, OneTime, RValue>,
            Direction>
    {
        template <class UniqueRange>
            typename result_of <callable::size (Direction,
                unique_range_detail::callable::get_underlying (UniqueRange))
            >::type
            operator() (Direction const & direction, UniqueRange && range) const
        {
            return range::size (direction,
                unique_range_detail::get_underlying (range));
        }
    };

    // first: only if OneTime = false.
    // Otherwise this is implementented automatically through "chop".
    template <class UnderlyingTag, bool RValue, class Direction>
        struct first <unique_range_tag <UnderlyingTag, false, RValue>,
            Direction>
    {
        template <class UniqueRange>
            typename result_of <callable::first (Direction,
                unique_range_detail::callable::get_underlying (UniqueRange))
            >::type
            operator() (Direction const & direction, UniqueRange && range) const
        {
            return range::first (direction,
                unique_range_detail::get_underlying (range));
        }
    };

    /**
    drop() only takes an rvalue range, pilfers it, and deactivates the original
    range.
    */
    template <class UnderlyingTag, bool OneTime, class Direction,
            class Increment>
        struct drop <unique_range_tag <UnderlyingTag, OneTime, true>, Direction,
            Increment, typename boost::enable_if <is_implemented <
                drop <UnderlyingTag, Direction, Increment>>>::type>
    {
        template <class Underlying>
            unique_range <typename result_of <
                    callable::drop (Direction, Increment, Underlying)>::type,
                OneTime>
            operator() (Direction const & direction,
                Increment const & increment,
                unique_range <Underlying, OneTime> && r) const
        {
            typedef unique_range <typename result_of <
                    callable::drop (Direction, Increment, Underlying)>::type,
                OneTime>
                result_type;

            return result_type (
                range::drop (direction, increment,
                    unique_range_detail::get_underlying (std::move (r))));

        }
    };

    /**
    chop() only takes an rvalue range, pilfers it, and deactivates the original
    range.

    This is enabled if "drop<...>" is implemented, since "chop" is usually
    implemented automatically in "apply".
    */
    template <class UnderlyingTag, bool OneTime, class Direction>
        struct chop <unique_range_tag <UnderlyingTag, OneTime, true>, Direction,
            typename boost::enable_if <boost::mpl::or_ <
                is_implemented <drop <UnderlyingTag, Direction, one_type>>,
                is_implemented <chop <UnderlyingTag, Direction>>
            >>::type>
    {
        template <class Underlying,
            class UnderlyingChopped = typename range::result_of <
                range::callable::chop (Direction, Underlying)>::type,
            class First = typename UnderlyingChopped::first_type,
            class UnderlyingRest = typename UnderlyingChopped::rest_type,
            class ResultRange = unique_range <UnderlyingRest, OneTime>,
            class Result = chopped <First, ResultRange>>
        Result operator() (Direction const & direction,
            unique_range <Underlying, OneTime> && r) const
        {
            UnderlyingChopped underlying_chopped = range::chop (
                direction, unique_range_detail::get_underlying (std::move (r)));
            return Result (underlying_chopped.move_first(),
                ResultRange (underlying_chopped.move_rest()));
        }
    };

}} // namespace range::operation

#endif  // RANGE_TEST_UNIQUE_RANGE_HPP_INCLUDED
