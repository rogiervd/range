/*
Copyright 2014 Rogier van Dalen.

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

#ifndef RANGE_SCAN_HPP_INCLUDED
#define RANGE_SCAN_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>

#include "utility/returns.hpp"
#include "utility/storage.hpp"

#include "rime/variant.hpp"
#include "rime/enable_if_constant.hpp"
#include "rime/call_if.hpp"
#include "rime/always.hpp"
#include "rime/cast.hpp"

#include "core.hpp"

#include "detail/with_direction.hpp"

namespace range {

/**
Range that implements a "scan".
Its elements are the result of applying a function to the last element, and the
next element of an underlying range.
The first element is the initial state.
The length of the scan_range is therefore one more than the underlying range.
The last element of the scan_range is given when the underlying range is already
empty.
Therefore, a past-the end state must be introduced.
This is indicated with a boolean value of type \a Empty.
This can be a constant type, if the underlying range is known to be empty or
non-empty.
The implementation is therefore specialised for three cases: \a Empty is known
at compile time to be true, it is known to be false, or it is a runtime value.
*/
template <class Direction, class Empty,
    class Function = void, class State = void, class Underlying = void,
    class Enable = void>
class scan_range;

/*
Interface.
The structure of this is the same as some of detail/core_*.hpp.
*/

namespace operation {

    template <class RangeTag, class Direction, class Enable = void>
        struct scan;

    template <class RangeTag, class Direction, class Enable /* = void*/>
        struct scan
    {
        /**
        Return an type, constant false if empty (direction, range) is constant,
        or "bool" if it is not.
        */
        template <class Range> struct not_empty
        : boost::mpl::if_<rime::is_constant <typename
            std::result_of <range::callable::empty (Direction, Range)>::type>,
            rime::false_type, bool> {};

        template <class Function, class State, class Range>
        auto operator() (Direction const & direction, Function && function,
            State && state, Range && range) const
        RETURNS (scan_range <Direction, typename not_empty <Range>::type,
            Function, State, typename std::decay <Range>::type> (
                direction, std::forward <Function> (function),
                std::forward <State> (state), std::forward <Range> (range)));
    };

} // namespace operation

namespace apply {

    namespace automatic_arguments {

        // scan.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct scan : operation::unimplemented {};

        template <class Direction, class Function, class State, class Range>
            struct scan <meta::vector <Direction>,
                meta::vector <Function, State>, meta::vector <Range>>
        : operation::scan <typename range::tag_of <Range>::type, Direction> {};

    } // namespace automatic_arguments

    /** scan */
    template <class ... Arguments> struct scan
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view_once <
            automatic_arguments::scan>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

namespace callable {
    struct scan : generic <apply::scan> {};
} // namespace callable

/** \brief
Return a lazy "prefix sum", i.e. all the intermediate step of an accumulation.

This is often called a "scan", and seen as a step-by-step version of \a fold.
Like \a fold, \a scan takes a function, a current state, and a range.
Like \a fold, it applies the function to the state and the first element of the
range, then to the result of that and the second element, et cetera.
However, \a fold performs the whole computation at once; \a scan returns the
intermediate values lazily.

The state passed in forms the first element of the resulting range.
The second element is the result of the function applied to the start state and
the range passed in.
This forms the next element of the resulting range.
This continues until the underlying range is empty; the last element of the
range that \a scan returns is the result of applying the function to each
element of the underlying range.
The number of elements in the resulting range is therefore one more than the
number of elements in the range that is passed in.

Though the return type of "first" is a reference to the first element, the
return type of "chop" has as its first element the value returned by the first
function.

\param direction
    (optional) The direction of traversal of both the underlying and the
    resulting range.
    If this is not given, the default direction of the underlying range is used.
\param function
    The function to be applied.
    This is saved exactly as qualified.
    Its result type is stored exactly as qualified.
\param state
    The initial state, i.e. the first element.
    This is saved exactly as qualified.
\param range
    The underlying range.
    This will be converted into a view.
*/
static const auto scan = callable::scan();

/* Implementation of scan_range. */

// Case 1: known to be empty.
template <class Direction, class Empty>
    class scan_range <Direction, Empty, typename
        rime::enable_if_constant_true <Empty>::type>
: public detail::with_direction <Direction>
{
public:
    scan_range (Direction const & direction)
    : detail::with_direction <Direction> (direction) {}

    Empty empty() const { return Empty(); }
};

// Case 2: known to be non-empty.
template <class Direction, class Empty,
        class Function, class State, class Underlying>
    class scan_range <Direction, Empty, Function, State, Underlying, typename
        rime::enable_if_constant_false <Empty>::type>
: public detail::with_direction <Direction>
{
public:
    typedef Function function_type;
    typedef State state_type;
    typedef Underlying underlying_type;

private:
    typedef typename utility::storage::store <Function>::type
        stored_function_type;
    typedef typename utility::storage::store <State>::type stored_state_type;
    stored_function_type function_;
    stored_state_type state_;
    Underlying underlying_;

public:
    template <class QFunction, class QState, class QUnderlying>
    scan_range (Direction const & direction, QFunction function,
        QState && state, QUnderlying && underlying)
    : detail::with_direction <Direction> (direction),
        function_ (std::forward <QFunction> (function)),
        state_ (std::forward <QState> (state)),
        underlying_ (std::forward <QUnderlying> (underlying)) {}

    // Copy-construction.
    scan_range (scan_range const & that)
    : detail::with_direction <Direction> (that.direction()),
        function_ (that.function_), state_ (that.state_),
        underlying_ (that.underlying_) {}

    scan_range (scan_range && that)
    : detail::with_direction <Direction> (std::move (that.direction())),
        function_ (std::move (that.function_)),
        state_ (utility::storage::get <State, scan_range &&>() (that.state_)),
        underlying_ (std::move (that.underlying_)) {}

    // Assignment. (Default operators will not do.)
    scan_range & operator = (scan_range const & that) {
        function_ = that.function_;
        state_ = that.state_;
        underlying_ = that.underlying_;
        return *this;
    }

    scan_range & operator = (scan_range && that) {
        function_ = std::move (that.function_);
        state_ = utility::storage::get <State, scan_range &&>() (that.state_);
        underlying_ = std::move (that.underlying_);
        return *this;
    }

    Empty empty() const { return Empty(); }

    Function const & function() const { return function_; }
    Function & function() { return function_; }

    typename utility::storage::get <State, scan_range const &>::type
        state() const
    { return state_; }

    typename utility::storage::get <State, scan_range &>::type state()
    { return state_; }

    Underlying const & underlying() const { return underlying_; }
    Underlying & underlying() { return underlying_; }
};

// Case 2: not known whether it is empty.
template <class Direction, class Empty,
        class Function, class State, class Underlying>
    class scan_range <Direction, Empty, Function, State, Underlying, typename
        rime::disable_if_constant <Empty>::type>
: public detail::with_direction <Direction>
{
public:
    typedef Function function_type;
    typedef State state_type;
    typedef Underlying underlying_type;

private:
    typedef typename utility::storage::store <Function>::type
        stored_function_type;
    typedef typename utility::storage::store <State>::type stored_state_type;

    struct content_type {
        stored_function_type function_;
        stored_state_type state_;
        Underlying underlying_;

        template <class QFunction, class QState, class QUnderlying>
        content_type (
            QFunction && function, QState && state, QUnderlying && underlying)
        : function_ (std::forward <QFunction> (function)),
            state_ (std::forward <QState> (state)),
            underlying_ (std::forward <QUnderlying> (underlying)) {}

        content_type (content_type const & that)
        : function_ (that.function_), state_ (that.state_),
            underlying_ (that.underlying_) {}

        content_type (content_type && that)
        : function_ (std::move (that.function_)),
            state_ (utility::storage::get <State, content_type &&>() (
                that.state_)),
            underlying_ (std::move (that.underlying_)) {}
    };

    /**
    Hold either the function, state, and underlying range, or, if this is empty,
    nothing at all.
    This should use boost::optional, really, but that does not currently allow
    moving.
    */
    rime::variant <content_type, void> content_;

public:
    scan_range (Direction const & direction)
    : detail::with_direction <Direction> (direction) {}

    template <class QFunction, class QState, class QUnderlying>
    scan_range (Direction const & direction,
        QFunction && function, QState && state, QUnderlying && underlying)
    : detail::with_direction <Direction> (direction),
        content_ (content_type (
            std::forward <QFunction> (function), std::forward <QState> (state),
            std::forward <QUnderlying> (underlying)))
    {}

    // Construction.
    scan_range (scan_range const & that)
    : detail::with_direction <Direction> (that.direction()),
        content_ (that.content_) {}

    scan_range (scan_range && that)
    : detail::with_direction <Direction> (std::move (that.direction())),
        content_ (std::move (that.content_)) {}

    // Assignment must be explicit because rime::variant could not do the right
    // thing.
    scan_range & operator = (scan_range const & that) {
        content_.replace (that.content_);
        return *this;
    }

    scan_range & operator = (scan_range && that) {
        content_.replace (std::move (that.content_));
        return *this;
    }

    bool empty() const { return content_.template contains <void>(); }

    Function const & function() const
    { return rime::get <content_type> (content_).function_; }
    Function & function()
    { return rime::get <content_type> (content_).function_; }

    typename utility::storage::get <State, scan_range const &>::type
        state() const
    { return rime::get <content_type> (content_).state_; }

    typename utility::storage::get <State, scan_range &>::type state()
    { return rime::get <content_type> (content_).state_; }

    Underlying const & underlying() const
    { return rime::get <content_type> (content_).underlying_; }
    Underlying & underlying()
    { return rime::get <content_type> (content_).underlying_; }
};

/**
Tag for scan_range.
\tparam Direction The direction type.
\tparam KnownEmpty
    Whether the scan_range is known to be empty.
    This, of course, occurs one element after the underlying range becomes
    empty.
\tparam Homogeneous
    Whether the scan_range is homogeneous, i.e. whether the underlying range is
    homogeneous and the function returns the same type.
\tparam DropImplementable
    Whether \a drop is implementable on the range as qualified.
    (Either first and drop, or just chop, need to be implemented for the right
    qualifications.)
\tparam Reference
    Whether the range is an lvalue reference (so that chop_in_place can be
    implemented).
*/
template <class Direction, bool KnownEmpty,
    bool Homogeneous, bool DropImplementable, bool Reference>
struct scan_tag;

namespace scan_range_detail {

    template <class Direction, class Function, class State, class Underlying>
        struct function_homogeneous
    : std::is_same <typename result_of <Function (
            State, callable::first (Direction, Underlying))>::type,
        State>
    {};

    template <class Direction, class Function, class State, class Underlying,
        qualification qualifier>
    struct tag_of_scan_range
    {
        template <class Underlying2> struct can_iterate
        : boost::mpl::or_<
            boost::mpl::and_<
                has <callable::first (Direction, Underlying2)>,
                has <callable::drop (Direction, Underlying2)>>,
            has <callable::chop (Direction, Underlying2)>
        > {};

        static bool constexpr homogeneous = boost::mpl::and_<
                ::range::is_homogeneous <Direction, Underlying>,
                function_homogeneous <Direction, Function, State, Underlying>
            >::value;

        static bool constexpr drop_implementable =
            boost::mpl::or_<
                always_empty <Direction, Underlying>,
                can_iterate <
                    typename std::conditional <(qualifier == temporary),
                        Underlying, Underlying const &>::type>
            >::value;

        typedef scan_tag <Direction, false, homogeneous, drop_implementable,
            (qualifier == reference)> type;
    };

    template <class Direction, qualification qualifier>
        struct tag_of_empty_scan_range
    {
        typedef scan_tag <
            Direction, true, false, false, (qualifier == reference)> type;
    };

} // namespace scan_range_detail

template <class Direction, class Empty, class Function, class State,
    class Underlying, qualification qualifier>
struct tag_of_qualified <scan_range <
    Direction, Empty, Function, State, Underlying>, qualifier>
: boost::mpl::eval_if <
    boost::mpl::and_<rime::is_constant <Empty>, Empty>,
    scan_range_detail::tag_of_empty_scan_range <Direction, qualifier>,
    scan_range_detail::tag_of_scan_range <
        Direction, Function, State, Underlying, qualifier>> {};

namespace operation {

    template <class Direction, bool KnownEmpty,
            bool Homogeneous, bool DropImplementable, bool IsReference>
        struct empty <scan_tag <Direction, KnownEmpty,
            Homogeneous, DropImplementable, IsReference>, Direction>
    {
        template <class ScanRange> auto operator() (
            Direction const & direction, ScanRange const & r) const
        -> decltype (r.empty())
        {
            r.direction_must_be_equal (direction);
            return r.empty();
        }
    };

    // size.
    // Case where the scan_range is empty.
    template <class Direction,
            bool Homogeneous, bool DropImplementable, bool IsReference>
        struct size <scan_tag <Direction, true,
            Homogeneous, DropImplementable, IsReference>, Direction>
    : rime::callable::always_default <rime::size_t <0>> {};

    // Case where the scan_range is non-empty.
    template <class Direction,
            bool Homogeneous, bool DropImplementable, bool IsReference>
        struct size <scan_tag <Direction, false,
            Homogeneous, DropImplementable, IsReference>, Direction>
    {
        // (This is a function class to prevent a compiler error on GCC 4.6.)
        struct next {
            template <class Type> auto operator() (Type const & value) const
            RETURNS (rime::cast_value <Type> (value + one_type()));
        };

        template <class ScanRange> auto operator() (
            Direction const & direction, ScanRange const & r) const
        -> decltype (next() (range::size (r.underlying())))
        {
            r.direction_must_be_equal (direction);
            return next() (range::size (r.underlying()));
        }
    };

    // first.
    template <class Direction, bool KnownEmpty,
            bool Homogeneous, bool DropImplementable, bool IsReference>
        struct first <scan_tag <Direction, KnownEmpty,
            Homogeneous, DropImplementable, IsReference>,
        Direction>
    {
        template <class ScanRange>
            typename utility::storage::get <typename
                std::decay <ScanRange>::type::state_type, ScanRange &&>::type
        operator() (Direction const & direction, ScanRange && r) const
        {
            r.direction_must_be_equal (direction);
            return utility::storage::get <typename std::decay <ScanRange>::type
                ::state_type, ScanRange &&>() (r.state());
        }
    };

    // drop_one.
    template <class Direction, bool Homogeneous, bool IsReference>
        struct drop_one <scan_tag <Direction, false,
            Homogeneous, true, IsReference>, Direction>
    {
        /* Compute the result type. */
        // Case 1: the current range is not known to be empty.
        template <class Function, class State, class Underlying,
            class UnderlyingEmpty = typename std::result_of <
                callable::empty (Direction, Underlying)>::type,
            class Enable = void>
        struct result_implementation
        {
            typedef typename std::result_of <
                callable::first (Direction, Underlying)>::type underlying_first;
            typedef typename std::result_of <
                Function (State, underlying_first)>::type new_state;
            typedef typename std::result_of <
                callable::drop (Direction, Underlying)>::type new_underlying;
            typedef scan_range <Direction, UnderlyingEmpty,
                Function, new_state, new_underlying> type;
        };

        // Case 2: the current range is known to be empty.
        template <class Function, class State, class Underlying,
            class UnderlyingEmpty>
        struct result_implementation <
            Function, State, Underlying, UnderlyingEmpty,
            typename rime::enable_if_constant_true <UnderlyingEmpty>::type>
        { typedef scan_range <Direction, UnderlyingEmpty> type; };

        template <class ScanRange> struct result
        : result <typename std::decay <ScanRange>::type> {};

        template <class Empty, class Function, class State, class Underlying>
            struct result <
                scan_range <Direction, Empty, Function, State, Underlying>>
        : result_implementation <Function, State, Underlying> {};

        struct when_underlying_empty {
            template <class ScanRange,
                class Result = typename result <ScanRange>::type>
            Result operator() (Direction const & direction, ScanRange const & r)
                const
            { return Result (direction); }
        };

        struct when_underlying_not_empty {
            // Const reference: use "first" and "drop"
            template <class Empty,
                class Function, class State, class Underlying,
                class Result = typename result_implementation <
                    Function, State, Underlying>::type>
            Result operator() (Direction const & direction,
                scan_range <Direction, Empty, Function, State, Underlying>
                    const & r) const
            {
                return Result (r.direction(), r.function(),
                    r.function() (r.state(),
                        range::first (direction, r.underlying())),
                    range::drop (direction, r.underlying()));
            }

            // Rvalue: use "chop".
            template <class Empty,
                class Function, class State, class Underlying,
                class Result = typename
                    result_implementation <Function, State, Underlying>::type>
            Result operator() (Direction const & direction,
                scan_range <Direction, Empty, Function, State, Underlying> && r)
                const
            {
                auto chopped = range::chop (direction,
                    std::move (r.underlying()));
                return Result (r.direction(),
                    std::forward <Function> (r.function()),
                    r.function() (std::forward <State> (r.state()),
                        chopped.move_first()), chopped.move_rest());
            }
        };

        template <class One, class ScanRange, class Result =
            typename result <ScanRange>::type> Result
            operator() (Direction const & direction, One, ScanRange && range)
            const
        {
            range.direction_must_be_equal (direction);
            return rime::call_if (range::empty (direction, range.underlying()),
                when_underlying_empty(), when_underlying_not_empty(),
                direction, std::forward <ScanRange> (range));
        }
    };

    // chop.
    // Must be implemented because "first" should not be a reference.
    template <class Direction, bool Homogeneous, bool IsReference>
        struct chop <scan_tag <Direction, false,
            Homogeneous, true, IsReference>, Direction>
    {
        template <class ScanRange> struct result {
            typedef typename std::decay <ScanRange>::type::state_type
                first_type;
            typedef typename decayed_result_of <
                    callable::drop (Direction, one_type, ScanRange)>::type
                rest_type;

            typedef chopped <first_type, rest_type> type;
        };

        template <class ScanRange,
        class Result = typename result <ScanRange>::type,
            class FirstType = typename result <ScanRange>::first_type>
        Result operator() (Direction const & direction, ScanRange && range)
            const
        {
            FirstType first = range::first (direction, range);
            return Result (std::forward <FirstType> (first),
                range::drop (direction, std::forward <ScanRange> (range)));
        }
    };

    // chop_in_place: must be implemented explicitly because it cannot return a
    // reference to the state.
    template <class Direction, bool KnownEmpty, bool DropImplementable>
        struct chop_in_place <scan_tag <Direction, KnownEmpty,
            true, DropImplementable, true>, Direction>
    {
        template <class ScanRange,
            class Result = typename std::decay <ScanRange>::type::state_type>
        Result operator() (Direction const & direction, ScanRange & range)
            const
        {
            Result first = range::first (direction, range);
            range = ::range::drop (direction, std::move (range));
            return std::forward <Result> (first);
        }
    };

} // namespace operation

} // namespace range

#endif // RANGE_SCAN_HPP_INCLUDED
