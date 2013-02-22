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
Define a helper for all() and any().
These two functions perform basically the same operation.
all() returns true iff all elements of a range evaluate to true.
any() returns true iff any element of a range evaluates to true.
That is, any() returns false iff all elements of a range do not evaluate to
true.

Written tersely,
    all (r)
returns
    fold (&&, true, r);
and
    any (r)
returns
    fold (||, false, r).

The reason that this file is longish is twofold.

Firstly, if the result is known at compile time, a compile time constant is
returned.

Secondly, short-circuiting is important.
If any element is known at compile time to be false (for all()) or true (for
any()), then the following elements are not instantiated, and none are
evaluated.
If any element is found at run time to false (for all()) or true (for any()),
then the following elements are not evaluated.

Note that for consistency, even if the result is known at compile time, the
elements up to the elements that decides this will be evaluated.
*/

#ifndef RANGE_DETAIL_FOLD_BOOL_HPP_INCLUDED
#define RANGE_DETAIL_FOLD_BOOL_HPP_INCLUDED

#include <utility>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/identity.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include "rime/core.hpp"
#include "rime/assert.hpp"
#include "rime/call_if.hpp"

#include "../core.hpp"

namespace range { namespace callable { namespace fold_bool_detail {

template <bool value> struct when_empty;
template <bool value> struct when_not_empty;

template <bool value> struct fold_bool {
    template <class Direction, class Range>
        auto operator() (Direction const & direction, Range && range) const
    -> decltype (rime::call_if (range::empty (direction, range),
        std::declval <when_empty <value>>(),
        std::declval <when_not_empty <value>>(),
        direction, std::declval <Range>()))
    {
        auto empty = range::empty (direction, range);
        // Sequence point here.
        return rime::call_if (empty,
            when_empty <value>(), when_not_empty <value>(),
            direction, range::view (std::forward <Range> (range)));
    }

    // Without direction.
    template <class Range> auto operator() (Range && range) const
    -> decltype (std::declval <fold_bool>() (
        range::default_direction (range), std::declval <Range>()))
    {
        auto const direction = range::default_direction (range);
        // Sequence point here.
        return (*this) (direction, range::view (std::forward <Range> (range)));
    }
};

} // namespace fold_bool_detail

namespace fold_bool_detail {

/**
Check the truth value either at compile time or run time.
*/
template <bool value> struct equal_truth_value;

template <> struct equal_truth_value <false> {
    template <class Type> auto operator() (Type const & v)
    const RETURNS (rime::not_ (v))
};

template <> struct equal_truth_value <true> {
    template <class Type> auto operator() (Type const & v)
    const RETURNS (!rime::not_ (v))
};


/**
Implementation of fold_bool when the range is empty.
This just returns
*/
template <bool value> struct when_empty {
    template <class Direction, class Range>
        auto operator() (const Direction & direction, Range && range)
    RETURNS (rime::bool_<value>())
};

/**
Return whether all elements of a non-empty range evaluate to "value".
This would not be very hard to implement normally.
This could use operator &&, or rime::and_.
However, neither of these allows proper short-circuiting both when the value
is know at compile time and when it isn't.
The below is therefore slightly more complicated than one would expect.
*/
template <bool value> struct when_not_empty {
    /**
    Implementation that computes the return type automatically.
    */
    template <class Direction, class Range, class FirstEqual>
        static auto implementation (const Direction & direction,
        Range && range, FirstEqual const & first_equal)
    RETURNS (rime::call_if (first_equal,
        fold_bool <value>(), range::operation::helper
            ::return_default_constructed <rime::bool_<!value>>(),
        direction, range::drop (direction, std::forward <Range> (range))))

    /// Compute the return type.
    template <class Direction, class Range,
        class First = typename range::result_of::first <Direction, Range>::type,
        class Enable = void>
    struct result {
        typedef decltype (implementation (
                std::declval <Direction>(), std::declval <Range>(),
                equal_truth_value <value>() (std::declval <First>())
            )) type;
        static_assert (
            std::is_same <type, bool>::value ||
            std::is_same <type, rime::false_type>::value ||
            std::is_same <type, rime::true_type>::value, "");
    };

    /**
    For homogeneous ranges, automatically determining the type would be
    recursive, and would fail.
    Therefore, set it to a sensible type.
    */
    template <class Direction, class Range, class First>
        struct result <Direction, Range, First, typename
            boost::enable_if <is_homogeneous <Direction, Range>>::type>
    // If all elements are known at compile time, the result is always the
    // truth value of this element, since the range is not empty.
    // If not, the result is not known at compile time.
    : boost::mpl::eval_if <rime::is_constant <First>,
        boost::mpl::if_ <typename std::decay <First>::type,
            rime::true_type, rime::false_type>,
        boost::mpl::identity <bool>> {};

    /**
    Result is a compile-time constant.
    Return default-constructed constant and do not recurse.
    */
    template <class Direction, class Range,
            typename Result = typename result <Direction, Range>::type>
        auto operator() (const Direction & direction, Range && range,
            typename boost::enable_if <rime::is_constant <Result>>::type * = 0)
    RETURNS (Result())

    /**
    Result not known at compile time.

    */
    template <class Direction, class Range>
        bool operator() (const Direction & direction, Range && range,
            typename boost::disable_if <rime::is_constant <
                typename result <Direction, Range>::type>>::type * = 0)
    {
        auto first_equal = equal_truth_value <value>() (
            range::first (direction, range));
        return implementation (direction, std::forward <Range> (range),
            first_equal);
    }
};

}}} // namespace range::callable::fold_bool_detail

#endif  // RANGE_DETAIL_FOLD_BOOL_HPP_INCLUDED

