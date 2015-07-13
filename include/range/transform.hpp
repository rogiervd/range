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

#ifndef RANGE_TRANSFORM_HPP_INCLUDED
#define RANGE_TRANSFORM_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include "utility/returns.hpp"
#include "utility/assignable.hpp"
#include "utility/storage.hpp"

#include "core.hpp"
#include "helper/underlying.hpp"

namespace range {

template <class Underlying, class Function> struct transform_view;

namespace transform_operation {
    struct transform_view_tag {};
} // namespace transform_operation

template <class Underlying, class Function>
    struct tag_of_qualified <transform_view <Underlying, Function>>
{ typedef transform_operation::transform_view_tag type; };

template <class Underlying, class Function> struct transform_view {
public:
    typedef Underlying underlying_type;
    typedef Function function_type;

    template <class Underlying_, class Function_>
        transform_view (Underlying_ && underlying_, Function_ && function_)
    : underlying_ (std::forward <Underlying_> (underlying_)),
        function_ (std::forward <Function_> (function_)) {}

    transform_view (transform_view const & that)
    : underlying_ (that.underlying_), function_ (that.function()) {}

    transform_view (transform_view && that)
    : underlying_ (std::move (that.underlying_)),
        function_ (utility::storage::get <Function, transform_view &&>() (
            that.function_.content())) {}

    transform_view & operator= (transform_view const & that) {
        underlying_ = that.underlying_;
        function_ = that.function();
        return *this;
    }

    transform_view & operator= (transform_view && that) {
        underlying_ = std::move (that.underlying_);
        function_ = utility::storage::get <Function, transform_view &&>() (
            that.function_.content());
        return *this;
    }

    typename utility::storage::get <Function, transform_view const &>::type
        function() const { return function_.content(); }

    Underlying const & underlying() const { return underlying_; }

private:
    template <class Wrapper> friend class helper::callable::get_underlying;

    // Underlying should be assignable already.
    Underlying underlying_;
    // The function is not necessarily assignable.
    utility::assignable <typename utility::storage::store <Function>::type>
        function_;

    friend class helper::member_access;

    auto default_direction() const
    RETURNS (range::default_direction (underlying_));

    template <class Direction> typename result_of <
        callable::empty (Underlying const &, Direction)>::type
            empty (Direction const & direction) const
    { return range::empty (underlying_, direction); }

    template <class Direction> typename result_of <
        callable::size (Underlying const &, Direction)>::type
            size (Direction const & direction) const
    { return range::size (underlying_, direction); }

    template <class Direction> struct chop_in_place_result
    : result_of <Function (typename result_of <
        callable::chop_in_place (Direction, Underlying &)>::type)> {};

    template <class Direction> auto chop_in_place (Direction const & direction)
    RETURNS (function_.content() (
        range::chop_in_place (underlying_, direction)));
};

namespace callable {

    struct transform {
    private:
        struct dispatch {
            template <class View, class Function, class ... Directions> auto
                operator() (View && view, Function && function) const
            RETURNS (range::transform_view <
                    typename std::decay <View>::type,
                    typename std::decay <Function>::type>
                (std::forward <View> (view),
                    std::forward <Function> (function)));
        };

    public:
        template <class Range, class Function, class ... Directions> auto
            operator() (Range && range, Function && function,
                Directions const & ... directions) const
        RETURNS (dispatch() (
            range::view (std::forward <Range> (range), directions ...),
            std::forward <Function> (function)));
    };

} // namespace callable

/**
Transform each of the elements of a range using a function.
\return A wrapper range that computes its elements lazily.
The range is converted into a view before it is stored.

\param range
    The range to wrap.
\param function
    Function that is applied to elements of the underlying range.
    Its return value is used as an element of the transformed range.
    This function is called every time first() is used.
\param directions
    (optional) Directions that should be used to convert the range into a view.
*/
static const auto transform = callable::transform();

/* Forward to underlying and then wrap: first, drop. */

namespace transform_operation {

    // first: transform first element of underlying.
    template <class View, class Direction> inline
        auto implement_first (transform_view_tag const &, View && view,
            Direction const & direction)
    RETURNS (view.function() (range::first (
        range::helper::get_underlying <View> (view), direction)));

    // drop: forward to underlying and re-wrap.
    template <class View, class Increment, class Direction>
        inline auto implement_drop (transform_view_tag const &,
            View && view, Increment const & increment,
            Direction const & direction)
    RETURNS (range::transform (
        range::drop (range::helper::get_underlying <View> (view),
            increment, direction),
        view.function(), direction));

    // chop: forward to underlying and re-wrap.
    // Note that this is only implemented if chop<> is implemented natively for
    // the underlying range, not if it is synthesised.
    template <class View, class Direction,
        class UnderlyingChopped = decltype (
            callable::chop_direct() (
                range::helper::get_underlying <View> (std::declval <View &>()),
                std::declval <Direction>(), pick_overload())),
        class Result = chopped <
            typename std::decay <decltype (
                std::declval <View>().function() (
                    std::declval <UnderlyingChopped>().move_first())
            )>::type,
            typename std::decay <decltype (
                range::transform (
                    std::declval <UnderlyingChopped>().move_rest(),
                    std::declval <View>().function(),
                    std::declval <Direction>())
            )>::type>
        >
    inline Result implement_chop (transform_view_tag const &, View && view,
        Direction const & direction)
    {
        auto chopped = range::chop (
            range::helper::get_underlying <View> (view), direction);
        return Result (
            view.function() (chopped.move_first()),
            range::transform (chopped.move_rest(), view.function(), direction));
    }

} // namespace transform_operation

} // namespace range

#endif // RANGE_TRANSFORM_HPP_INCLUDED
