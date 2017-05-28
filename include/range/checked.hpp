/*
Copyright 2017 Rogier van Dalen.

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

#ifndef RANGE_CHECKED_HPP_INCLUDED
#define RANGE_CHECKED_HPP_INCLUDED

#include <cassert>
#include <type_traits>
#include <utility>
#include <stdexcept>

#include <boost/exception/exception.hpp>

#include <boost/mpl/and.hpp>

#include "utility/disable_if_same.hpp"
#include "utility/overload_order.hpp"

#include "rime/core.hpp"

#include "core.hpp"
#include "helper/underlying.hpp"

namespace range {

struct out_of_range_error
: public virtual std::runtime_error, public virtual boost::exception {
public:
    explicit out_of_range_error()
    : std::runtime_error ("Range operation out of range") {}
};

/** \brief
A wrapped range that is equivalent to the underlying range but checks
operations.

This is mainly useful for debugging.

It asserts that the object has not been pilfered in a move.

It throws \a out_of_range_error when invalid operations are used.
For "drop" with increments, this is only checked if "size()" is implemented.

\tparam Underlying
    The underlying range.
    This must (currently) be a view.
\todo It would be useful to have this class, or another one, wrap heavyweight
classes and give completely separate behaviour, namely of checking that the
underlying heavyweight object still exists when the view is used.
*/
template <class Underlying> class checked_range;

/// Evaluate to "true" iff Range is a checked_range.
template <class Range> struct is_checked_range;

namespace checked_range_detail {
    template <class Range> struct is_checked_range : rime::false_type {};
    template <class Underlying>
        struct is_checked_range <checked_range <Underlying>>
    : rime::true_type {};
} // namespace checked_range_detail

template <class Range> struct is_checked_range
: checked_range_detail::is_checked_range <typename std::decay <Range>::type> {};

template <class Underlying> class checked_range {
public:
    static_assert (is_range <Underlying>::value,
        "Underlying must be a range.");
    static_assert (!is_checked_range <Underlying>::value,
        "Underlying must not be a checked_range itself.");

    typedef Underlying underlying_type;

private:
    Underlying underlying_;
    bool valid_;

    template <class Wrapper> friend class helper::callable::get_underlying;

public:
    template <class CVUnderlying, class Enable = typename
        utility::disable_if_same_or_derived <checked_range, CVUnderlying
            >::type>
    explicit checked_range (CVUnderlying && underlying)
    : underlying_ (std::forward <CVUnderlying> (underlying)), valid_ (true) {}

    checked_range (checked_range const &) = default;

    /// Move construction: invalidate \a other.
    checked_range (checked_range && other)
    : underlying_ (std::move (other.underlying_)), valid_ (other.valid_)
    { other.valid_ = false; }

    ~checked_range() { valid_ = false; }

    checked_range & operator= (checked_range const &) = default;

    checked_range & operator= (checked_range && other) {
        this->underlying_ = std::move (other.underlying_);
        this->valid_ = other.valid_;
        other.valid_ = false;
        return *this;
    }

    void assert_valid() const { assert (valid_); }

private:
    friend class helper::member_access;

    auto default_direction() const {
        assert_valid();
        return range::default_direction (underlying_);
    }

    // The return type is specified explicitly so that the specialisation gets
    // disabled when the operation is not available on the underlying range.

    template <class Direction> auto empty (Direction const & direction) const
    -> decltype (range::empty (underlying_, direction))
    {
        // It is checked only here that Underlying is a view.
        // Only here do we know the direction.
        static_assert (range::is_view <Underlying, Direction>::value,
            "Underlying range must be a view.");
        assert_valid();
        return range::empty (underlying_, direction);
    }

    template <class Direction> auto size (Direction const & direction) const
    -> decltype (range::size (underlying_, direction))
    {
        assert_valid();
        return range::size (underlying_, direction);
    }

    template <class Direction> auto chop_in_place (Direction const & direction)
    -> decltype (range::chop_in_place (underlying_, direction))
    {
        assert_valid();
        if (empty (direction))
            throw out_of_range_error();
        return range::chop_in_place (underlying_, direction);
    }
};

namespace checked_operation {
    struct checked_range_tag {};
} // namespace checked_operation

template <class Underlying>
    struct tag_of_qualified <checked_range <Underlying>>
{ typedef checked_operation::checked_range_tag type; };

namespace callable {

    class checked {
        template <class Underlying, class Enable = typename
            std::enable_if <!is_checked_range <Underlying>::value>::type>
        auto dispatch (Underlying && underlying,
            utility::overload_order <1> *) const
        RETURNS (checked_range <typename std::decay <Underlying>::type> (
            std::forward <Underlying> (underlying)));

        template <class CheckedRange, class Enable = typename
            std::enable_if <is_checked_range <CheckedRange>::value>::type>
        CheckedRange dispatch (CheckedRange && r,
            utility::overload_order <2> *) const
        { return std::forward <CheckedRange> (r); }

    public:
        template <class Underlying>
            auto operator() (Underlying && underlying) const
        RETURNS (dispatch (std::forward <Underlying> (underlying),
            utility::pick_overload()));
    };

} // namespace callable

/** \brief
Return a wrapped range that is equivalent to the underlying range but checks
operations.

When this is called on a checked_range, the range itself is returned.
It is thus safe to call this multiple times without causing nested types.

\see checked_range
*/
static auto const checked = callable::checked();

/* Operations on checked_range. */
// These require take to be defined, so they are implemented down here.

namespace checked_operation {

    // The return type is specified explicitly so that the specialisation gets
    // disabled when the operation is not available on the underlying range.

    template <class CheckedRange, class Direction>
        inline auto implement_first (checked_range_tag const &,
            CheckedRange && r, Direction const & direction)
    -> decltype (range::first (range::helper::get_underlying <CheckedRange> (r),
        direction))
    {
        r.assert_valid();
        if (range::empty (r, direction))
            throw out_of_range_error();
        return range::first (range::helper::get_underlying <CheckedRange> (r),
            direction);
    }

    /**
    If the range implements size(), check that it is greater than increment.
    If not, throw out_of_range_error.
    If the range does not implement size(), then do nothing.
    */
    template <class Range, class Increment, class Direction>
        void check_size_if_possible (Range const & range,
            Increment const & increment, Direction const & direction,
            typename std::enable_if <
                has <callable::size (Range, Direction)>::value>::type * = 0)
    {
        if (range::size (range, direction) < increment)
            throw out_of_range_error();
    }

    template <class Range, class Increment, class Direction>
        void check_size_if_possible (Range const &,
            Increment const &, Direction const &,
            typename std::enable_if <
                !has <callable::size (Range, Direction)>::value>::type * = 0)
    {}

    template <class CheckedRange, class Increment, class Direction>
        inline auto implement_drop (typename std::enable_if <
            has <callable::drop (
                typename range::helper::underlying_type <CheckedRange>::type,
                Increment, Direction)>::value,
            checked_range_tag>::type,
        CheckedRange && r, Increment const & increment,
        Direction const & direction)
    {
        r.assert_valid();
        // It should always be possible to check for emptiness.
        if (increment != 0 && range::empty (r, direction))
            throw out_of_range_error();

        // Some ranges do not have size(), so be careful.
        check_size_if_possible (r, increment, direction);

        return range::checked (
            range::drop (range::helper::get_underlying <CheckedRange> (r),
            increment, direction));
    }

    template <class CheckedRange, class Direction>
        inline auto implement_chop (typename std::enable_if <
            has <callable::chop (
                typename range::helper::underlying_type <CheckedRange>::type,
                Direction)>::value,
            checked_range_tag>::type,
        CheckedRange && r,
        Direction const & direction)
    {
        r.assert_valid();
        if (range::empty (r, direction))
            throw out_of_range_error();
        auto first_and_underlying_rest = range::chop (
            range::helper::get_underlying <CheckedRange> (r), direction);
        typedef decltype (first_and_underlying_rest) underlying_result_type;

        return chopped <typename underlying_result_type::first_type,
            checked_range <typename underlying_result_type::rest_type>> (
                first_and_underlying_rest.forward_first(),
                range::checked (first_and_underlying_rest.forward_rest()));
    }

} // namespace checked_operation

} // namespace range

#endif // RANGE_CHECKED_HPP_INCLUDED
