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
#include "detail/underlying.hpp"

namespace range {

template <class Function, class Underlying> struct transform_view;
template <class UnderlyingTag> struct transform_view_tag;

template <class Function, class Underlying>
    struct tag_of_qualified <transform_view <Function, Underlying>>
{ typedef transform_view_tag <typename tag_of <Underlying>::type> type; };

template <class Function, class Underlying> struct transform_view {
public:
    typedef Function function_type;
    typedef Underlying underlying_type;

    template <class Function_, class Underlying_>
        transform_view (Function_ && function_, Underlying_ && underlying_)
    : function_ (std::forward <Function_> (function_)),
        underlying_ (std::forward <Underlying_> (underlying_)) {}

    transform_view (transform_view const & that)
    : function_ (that.function()), underlying_ (that.underlying_) {}

    transform_view (transform_view && that)
    : function_ (utility::storage::get <Function, transform_view &&>() (
            that.function_.content())),
        underlying_ (std::move (that.underlying_)) {}

    transform_view & operator= (transform_view const & that) {
        function_ = that.function();
        underlying_ = that.underlying_;
        return *this;
    }

    transform_view & operator= (transform_view && that) {
        function_ = utility::storage::get <Function, transform_view &&>() (
            that.function_.content());
        underlying_ = std::move (that.underlying_);
        return *this;
    }

    typename utility::storage::get <Function, transform_view const &>::type
        function() const { return function_.content(); }

    Underlying const & underlying() const { return underlying_; }

private:
    friend class ::range::detail::callable::get_underlying;

    // The function is not necessarily assignable.
    utility::assignable <typename utility::storage::store <Function>::type>
        function_;
    // Underlying should be assignable already.
    Underlying underlying_;

    friend class operation::member_access;

    auto default_direction() const
    RETURNS (range::default_direction (underlying_));

    template <class Direction> typename result_of_or <
        callable::empty (Direction, Underlying const &)>::type
            empty (Direction const & direction) const
    { return range::empty (direction, underlying_); }

    template <class Direction> typename result_of_or <
        callable::size (Direction, Underlying const &)>::type
            size (Direction const & direction) const
    { return range::size (direction, underlying_); }

    template <class Direction> struct chop_in_place_result
    : std::result_of <Function (typename result_of <
        callable::chop_in_place (Direction, Underlying &)>::type)> {};

    template <class Direction> typename boost::lazy_enable_if <
        has <callable::chop_in_place (Direction, Underlying &)>,
        chop_in_place_result <Direction>
    >::type chop_in_place (Direction const & direction)
    {
        return function_.content() (
            range::chop_in_place (direction, underlying_));
    }
};

namespace operation {

    namespace transform_detail {

        struct make_transform_view {
            template <class Function, class Range> auto
                operator() (Function && function, Range && range) const
            RETURNS (range::transform_view <
                    typename std::decay <Function>::type,
                    typename std::decay <Range>::type>
                (std::forward <Function> (function),
                    std::forward <Range> (range)));
        };

    } // namespace transform_detail

    template <class RangeTag, class Directions, class Function>
        struct transform
    : helper::call_with_last <2, Directions,
        transform_detail::make_transform_view> {};

} // namespace callable

namespace apply {
    template <class ... Arguments> struct transform;
} // namespace apply

namespace callable {
    struct transform : generic <apply::transform> {};
} // namespace callable

/**
Transform each of the elements of a range using a function.
\return A wrapper range that computes its elements lazily.
The range is converted into a view before it is stored.

\param directions
    (optional) Directions that should be used to convert the range into a view.
\param function
    Function that is applied to elements of the underlying range.
    Its return value is used as an element of the transformed range.
    This function is called every time first() is used.
\param range
    The range to wrap.
*/
static const auto transform = callable::transform();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct transform : operation::unimplemented {};

        template <class Directions, class Function, class Range>
            struct transform <Directions, meta::vector<Function>,
                meta::vector <Range>>
        : operation::transform <typename range::tag_of <Range>::type,
            Directions, Function> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct transform
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view <automatic_arguments::transform
            >::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

namespace operation {

    /* Forward to underlying and then wrap: first, drop. */

    // first: transform first element of underlying.
    template <class UnderlyingTag, class Direction, class Range>
        struct first <transform_view_tag <UnderlyingTag>, Direction, Range,
            typename boost::enable_if <
                is_implemented <first <UnderlyingTag, Direction,
                    typename underlying <Range>::type>>>::type>
    {
        auto operator() (Direction const & direction, Range && range) const
        RETURNS (range.function() (range::first (direction,
            range::detail::get_underlying (std::forward <Range> (range)))));
    };

    // drop: forward to underlying and re-wrap.
    template <class UnderlyingTag, class Direction, class Increment,
            class Range>
        struct drop <transform_view_tag <UnderlyingTag>,
            Direction, Increment, Range,
            typename boost::enable_if <is_implemented <
                drop <UnderlyingTag, Direction, Increment,
                    typename underlying <Range>::type>>>::type>
    {
        auto operator() (Direction const & direction,
            Increment const & increment, Range && range) const
        RETURNS (range::transform (direction, range.function(),
            range::drop (direction, increment,
                range::detail::get_underlying (std::forward <Range> (range)))));
    };

    // chop: forward to underlying and re-wrap.
    // Note that this is only implemented if chop<> is implemented natively for
    // the underlying range, not if it is synthesised.
    template <class UnderlyingTag, class Direction, class Range>
        struct chop <transform_view_tag <UnderlyingTag>, Direction, Range,
            typename boost::enable_if <is_implemented <
                chop <UnderlyingTag, Direction,
                    typename underlying <Range>::type>>>::type>
    {
        template <class Function, class Underlying, class Enable = typename
                boost::enable_if <has <callable::chop (Direction, Underlying)>
            >::type>
        struct result {
            typedef typename decayed_result_of <
                    callable::chop (Direction, Underlying)
                >::type underlying_chopped;

            typedef typename underlying_chopped::first_type underlying_first;
            typedef typename std::decay <
                typename underlying_chopped::rest_type>::type underlying_rest;

            typedef typename std::result_of <Function const & (
                underlying_first)>::type first_type;
            typedef transform_view <Function, underlying_rest> rest_type;

            typedef chopped <first_type, rest_type> type;
        };

        template <class Result, class TransformView>
            static typename Result::type
            compute (Direction const & direction, TransformView && range)
        {
            auto chopped = range::chop (range::detail::get_underlying (
                std::forward <TransformView> (range)));
            return typename Result::type (
                range.function() (chopped.move_first()),
                typename Result::rest_type (range.function(),
                    chopped.move_rest()));
        }

        // Compute chop (n, underlying) and then re-wrap it in a transform_view.
        template <class Function, class Underlying>
            typename result <Function, Underlying &&>::type
        operator() (Direction const & direction,
            transform_view <Function, Underlying> && range) const
        {
            return compute <result <Function, Underlying &&>> (
                direction, std::move (range));
        }
        template <class Function, class Underlying>
            typename result <Function, Underlying const &>::type
        operator() (Direction const & direction,
            transform_view <Function, Underlying> const & range) const
        {
            return compute <result <Function, Underlying const &>> (
                direction, range);
        }
    };

} // namespace operation

} // namespace range

#endif // RANGE_TRANSFORM_HPP_INCLUDED
