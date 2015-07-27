/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_SCAN_HPP_INCLUDED
#define RANGE_SCAN_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>

#include "utility/enable_if_compiles.hpp"
#include "utility/returns.hpp"
#include "utility/storage.hpp"

#include "rime/core.hpp"
#include "rime/variant.hpp"
#include "rime/enable_if_constant.hpp"
#include "rime/call_if.hpp"
#include "rime/always.hpp"
#include "rime/cast.hpp"

#include "core.hpp"

#include "helper/underlying.hpp"
#include "helper/with_direction.hpp"

namespace range {

/** \brief
Range that implements a "scan".

The elements of the range are the result of applying a function to the last
element, and the next element of an underlying range.
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
    class Function = void, class State = void, class Underlying = void>
class scan_range;

/* Implementation of scan_range. */

// Case 1: known to be empty.
template <class Direction>
    class scan_range <Direction, rime::true_type>
: public helper::with_default_direction <Direction>
{
public:
    scan_range (Direction const & direction)
    : helper::with_default_direction <Direction> (direction) {}

    rime::true_type empty (Direction const & direction) const {
        this->direction_must_be_equal (direction);
        return rime::true_;
    }
};

// Case 2: known to be non-empty.
template <class Direction,
        class Function, class State, class Underlying>
    class scan_range <Direction, rime::false_type, Function, State, Underlying>
: public helper::with_default_direction <Direction>
{
public:
    typedef rime::false_type empty_type;
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
    : helper::with_default_direction <Direction> (direction),
        function_ (std::forward <QFunction> (function)),
        state_ (std::forward <QState> (state)),
        underlying_ (std::forward <QUnderlying> (underlying)) {}

    // Copy-construction.
    scan_range (scan_range const & that)
    : helper::with_default_direction <Direction> (that.direction()),
        function_ (that.function_), state_ (that.state_),
        underlying_ (that.underlying_) {}

    scan_range (scan_range && that)
    : helper::with_default_direction <Direction> (std::move (that.direction())),
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

    rime::false_type empty (Direction const & direction) const {
        this->direction_must_be_equal (direction);
        return rime::false_;
    }

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
template <class Direction,
        class Function, class State, class Underlying>
    class scan_range <Direction, bool, Function, State, Underlying>
: public helper::with_default_direction <Direction>
{
public:
    typedef bool empty_type;
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
    : helper::with_default_direction <Direction> (direction) {}

    template <class QFunction, class QState, class QUnderlying>
    scan_range (Direction const & direction,
        QFunction && function, QState && state, QUnderlying && underlying)
    : helper::with_default_direction <Direction> (direction),
        content_ (content_type (
            std::forward <QFunction> (function), std::forward <QState> (state),
            std::forward <QUnderlying> (underlying)))
    {}

    // Construction.
    scan_range (scan_range const & that)
    : helper::with_default_direction <Direction> (that.direction()),
        content_ (that.content_) {}

    scan_range (scan_range && that)
    : helper::with_default_direction <Direction> (std::move (that.direction())),
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

    bool empty (Direction const & direction) const {
        this->direction_must_be_equal (direction);
        return content_.template contains <void>();
    }

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

namespace scan_operation {

    /**
    Tag for scan_range.
    */
    template <class Direction> struct scan_tag {};

} // namespace scan_operation

template <class Direction, class Empty, class Function, class State,
    class Underlying>
struct tag_of_qualified <scan_range <
    Direction, Empty, Function, State, Underlying>>
{ typedef scan_operation::scan_tag <Direction> type; };

namespace scan_detail {

    // (This is a function class to prevent a compiler error on GCC 4.6.)
    struct next {
        template <class Type> auto operator() (Type const & value) const
        RETURNS (rime::cast_value <Type> (value + rime::size_t <1>()));
    };

} // namespace scan_detail

namespace scan_operation {

    template <class Direction, class ScanRange> inline
        auto implement_size (scan_tag <Direction> const &,
            ScanRange && r, Direction const & direction)
    RETURNS (r.direction_must_be_equal (direction),
        scan_detail::next() (range::size (r.underlying())));

    template <class Direction, class ScanRange> inline
        auto implement_first (scan_tag <Direction> const &,
            ScanRange && r, Direction const & direction)
    RETURNS (r.direction_must_be_equal (direction),
        utility::storage::get <
            typename std::decay <ScanRange>::type::state_type,
            ScanRange &&>() (r.state()));

} // namespace scan_operation

/* drop_one. */
namespace scan_detail {

    /*
    Compute the return type.
    This depends on whether the underlying range is empty, and (therefore)
    whether the result of "drop_one" is empty.
    */

    /**
    Return an type, constant false if empty (direction, range) is constant,
    or "bool" if it is not.
    */
    template <class Range, class Direction, class Empty = typename
        result_of <range::callable::empty (Range, Direction)>::type,
        class Enable = void>
    struct normalise_empty_type
    { typedef bool type; };

    template <class Range, class Direction, class Empty>
    struct normalise_empty_type <Range, Direction, Empty, typename
        std::enable_if <rime::is_constant <Empty>::value>::type>
    { typedef rime::bool_ <Empty::value> type; };

    // Default: disabled.
    template <class Direction,
        class ScanRange,
        class Empty = typename std::decay <ScanRange>::type::empty_type,
        class Function = typename std::decay <ScanRange>::type::function_type,
        class State = typename std::decay <ScanRange>::type::state_type,
        class Underlying = decltype (
            helper::get_underlying <ScanRange> (std::declval <ScanRange &>())),
        class UnderlyingEmpty = typename
        normalise_empty_type <Underlying, Direction>::type,
        class Enable1 = void, class Enable2 = void>
    struct drop_one_result {
        // No "type" member.
    };

    // A new non-empty scan_range: only if first() is available.
    template <class Direction, class ScanRange, class Empty,
        class Function, class State, class Underlying,
        class UnderlyingEmpty>
    struct drop_one_result <Direction, ScanRange, Empty,
        Function, State, Underlying, UnderlyingEmpty,
        typename utility::enable_if_compiles <decltype (first (
            std::declval <Underlying>(), std::declval <Direction>()))>::type,
        typename utility::enable_if_compiles <decltype (drop (
            std::declval <Underlying>(), std::declval <Direction>()))>::type>
    {
        typedef decltype (
                first (std::declval <Underlying>(), std::declval <Direction>()))
            underlying_first;
        typedef typename result_of <
            Function (State, underlying_first)>::type new_state;
        typedef typename result_of <
            callable::drop (Underlying, Direction)>::type new_underlying;
        typedef scan_range <Direction, UnderlyingEmpty,
            Function, new_state, new_underlying> type;
    };

    /*
    Compute the result value.
    This is separate from the computation of the result type, because whether
    the underlying range is empty can be known at compile time, or only at
    run time.
    */

    // If the underlying range is known to be empty.
    template <class Direction, class ScanRange, class Empty, class Function,
        class State, class Underlying>
    struct drop_one_result <Direction, ScanRange, Empty, Function, State,
        Underlying, rime::true_type>
    { typedef scan_range <Direction, rime::true_type> type; };

    template <class Result, class DecayedScanRange> struct when_empty {
        template <class Direction> Result operator() (
            DecayedScanRange const &, Direction const & direction) const
        { return Result (direction); }
    };

    template <class Result, class DecayedScanRange> struct when_not_empty {
        // Const reference: use "drop".
        template <class Direction> Result operator() (
            DecayedScanRange const & r, Direction const & direction) const
        {
            return Result (r.direction(), r.function(),
                r.function() (r.state(),
                    range::first (r.underlying(), direction)),
                range::drop (r.underlying(), direction));
        }

        // Rvalue: use "chop".
        template <class Direction> Result operator() (
            DecayedScanRange && r, Direction const & direction) const
        {
            auto chopped = range::chop (std::move (r.underlying()), direction);
            return Result (r.direction(), std::move (r.function()),
                r.function() (std::move (r.state()),
                    chopped.move_first()), chopped.move_rest());
        }
    };

} // namespace scan_detail

namespace scan_operation {

    template <class Direction, class ScanRange, class Result =
        typename scan_detail::drop_one_result <Direction, ScanRange>::type,
        class DecayedScanRange = typename std::decay <ScanRange>::type>
    inline Result implement_drop_one (scan_tag <Direction> const &,
        ScanRange && range, Direction const & direction)
    {
        range.direction_must_be_equal (direction);
        return rime::call_if (range::empty (range.underlying(), direction),
            scan_detail::when_empty <Result, DecayedScanRange>(),
            scan_detail::when_not_empty <Result, DecayedScanRange>(),
            std::forward <ScanRange> (range), direction);
    }

    // chop.
    template <class Direction, class ScanRange,
        class First = typename std::decay <ScanRange>::type::state_type,
        class Rest = decltype (range::callable::drop_direct() (
            std::declval <ScanRange>(), rime::size_t <1>(),
            std::declval <Direction>(), pick_overload())),
        class Result = chopped <First, Rest>>
    inline Result implement_chop (scan_tag <Direction> const &,
        ScanRange && range, Direction const & direction)
    {
        range.direction_must_be_equal (direction);
        // Get the current state.
        First first = range::first (range, direction);
        return Result (std::forward <First> (first),
            range::drop (std::forward <ScanRange> (range), direction));
    }

    // chop_in_place.
    template <class Direction, class ScanRange,
        class Result = typename std::decay <ScanRange>::type::state_type>
    inline Result implement_chop_in_place (scan_tag <Direction> const &,
        ScanRange & range, Direction const & direction)
    {
        range.direction_must_be_equal (direction);
        // Find the current state.
        Result first = range::first (range, direction);
        range = range::drop (std::move (range), direction);
        return std::forward <Result> (first);
    }

} // namespace scan_operation

namespace callable {

    struct scan {
    private:
        /**
        Compute type that indicates whether the resulting scan_range will be
        empty.
        The value of this will always be false, of course; but the type can be
        rime::false_type or bool.
        */
        template <class Underlying, class Direction>
        struct scan_range_empty
        : boost::mpl::if_<rime::is_constant <typename
                result_of <range::callable::empty (Underlying, Direction)
            >::type>, rime::false_type, bool> {};

        struct apply {
            template <class State, class Range, class Direction, class Function,
                class Result = scan_range <Direction, typename
                    scan_range_empty <Range, Direction>::type,
                    Function, State, typename std::decay <Range>::type>>
            Result operator() (State && state, Range && range,
                Direction const & direction, Function && function) const
            {
                return Result (direction, std::forward <Function> (function),
                    std::forward <State> (state), std::forward <Range> (range));
            }
        };

    public:
        template <class State, class Range, class Direction, class Function,
            // Implemented if "empty" is implemented.
            class Enable = decltype (range::empty (
                std::declval <Range>(), std::declval <Direction>()))>
        auto operator() (State && state, Range && range,
            Direction const & direction, Function && function) const
        RETURNS (apply() (std::forward <State> (state),
            range::view (std::forward <Range> (range), direction),
            direction,
            std::forward <Function> (function)));

        // Without direction: use default_direction.
        template <class State, class Range, class Function,
            // Implemented if "empty" is implemented.
            class Enable = decltype (range::empty (std::declval <Range>()))>
        auto operator() (State && state, Range && range,
            Function && function) const
        RETURNS (apply() (std::forward <State> (state),
            range::view (std::forward <Range> (range)),
            range::default_direction (range),
            std::forward <Function> (function)));
    };

} // namespace callable

/** \brief
Return a lazy "prefix sum", i.e. all the intermediate step of an accumulation.

This is often called a "scan", and seen as a lazy version of fold().
Like fold(), scan() takes a function, a current state, and a range.
Like fold(), it applies the function to the state and the first element of the
range, then to the result of that and the second element, et cetera.
However, fold() performs the whole computation at once; scan() returns the
intermediate values lazily.

The state passed in forms the first element of the resulting range.
The second element is the result of the function applied to the start state and
the range passed in.
This forms the next element of the resulting range.
This continues until the underlying range is empty; the last element of the
range that scan() returns is the result of applying the function to each
element of the underlying range.
The number of elements in the resulting range is therefore one more than the
number of elements in the range that is passed in.

Though the return type of "first" is a reference to the first element, the
return type of "chop" has as its first element the value returned by the first
function.

\param state
    The initial state, i.e. the first element.
    This is saved exactly as qualified.
\param range
    The underlying range.
    This will be converted into a view.
\param direction
    (optional) The direction of traversal of both the underlying and the
    resulting range.
    If this is not given, the default direction of the underlying range is used.
\param function
    The function to be applied.
    This is saved exactly as qualified.
    Its result type is stored exactly as qualified.
*/
static const auto scan = callable::scan();

} // namespace range

#endif // RANGE_SCAN_HPP_INCLUDED
