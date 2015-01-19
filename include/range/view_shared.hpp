/*
Copyright 2015 Rogier van Dalen.

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

#ifndef RANGE_VIEW_SHARED_HPP_INCLUDED
#define RANGE_VIEW_SHARED_HPP_INCLUDED

#include <memory>
#include <type_traits>

#include "utility/returns.hpp"

#include "core.hpp"
#include "detail/underlying.hpp"

namespace range {

namespace operation { namespace view_shared_detail {

        struct make_view_of_shared;
        struct get_heavyweight_pointer;

}} // namespace operation::view_shared_detail

/**
Range that holds a std::shared_ptr, and also a view of the range.
All copies or derivatives of the range will hold a copy of the shared_ptr, and
the operations will effect only the view.
*/
template <class Heavyweight,
    class View = typename std::decay <typename std::result_of <
        callable::view (Heavyweight &)>::type>::type>
    struct view_of_shared
{
public:
    static_assert (std::is_same <Heavyweight, typename
        std::remove_reference <Heavyweight>::type>::value,
        "The heavyweight range should not be a reference type.");

    typedef Heavyweight heavyweight_type;
    typedef std::shared_ptr <Heavyweight> heavyweight_pointer;
    typedef View underlying_type;

private:
    heavyweight_pointer heavyweight_;
    underlying_type underlying_;

    friend class detail::callable::get_underlying;

public:
    view_of_shared (heavyweight_pointer heavyweight, View const & underlying)
    : heavyweight_ (std::move (heavyweight)), underlying_ (underlying) {}

    view_of_shared (heavyweight_pointer heavyweight, View && underlying)
    : heavyweight_ (std::move (heavyweight)),
        underlying_ (std::move (underlying)) {}

private:
    friend struct operation::view_shared_detail::make_view_of_shared;
    friend struct operation::view_shared_detail::get_heavyweight_pointer;

    heavyweight_pointer const & heavyweight() const { return heavyweight_; }
    heavyweight_pointer & heavyweight() { return heavyweight_; }

private:
    friend class operation::member_access;

    auto default_direction() const
    RETURNS (range::default_direction (underlying_));

    template <class Direction> typename
        result_of_or <range::callable::empty (
            Direction, underlying_type const &)
    >::type empty (Direction const & direction) const
    { return range::empty (direction, underlying_); }

    template <class Direction> typename
        result_of_or <range::callable::size (
            Direction, underlying_type const &)
    >::type size (Direction const & direction) const
    { return range::size (direction, underlying_); }

    template <class Direction> typename
        result_of_or <range::callable::chop_in_place (
            Direction, underlying_type &)
    >::type chop_in_place (Direction const & direction)
    { return range::chop_in_place (direction, underlying_); }
};

struct view_of_shared_tag;

template <class Heavyweight, class View>
    struct tag_of_qualified <view_of_shared <Heavyweight, View>>
{ typedef view_of_shared_tag type; };

namespace operation {

    namespace view_shared_detail {

        struct make_view_of_shared {
            template <class Heavyweight, class View>
                view_of_shared <Heavyweight, typename std::decay <View>::type>
                    operator() (std::shared_ptr <Heavyweight> heavyweight,
                        View && view) const
            {
                return view_of_shared <Heavyweight, View> (
                    std::move (heavyweight), std::forward <View> (view));
            }
        };

        struct get_heavyweight_pointer {
            template <class Range>
                typename Range::heavyweight_pointer && operator() (
                    Range && range) const
            { return std::move (range.heavyweight()); }

            template <class Range>
                typename Range::heavyweight_pointer const & operator() (
                    Range const & range) const
            { return range.heavyweight(); }
        };

    } // namespace view_shared_detail

    template <class Direction, class Range>
        struct first <view_of_shared_tag, Direction, Range, typename
            boost::enable_if <has <
                callable::first (Direction, typename underlying <Range>::type)
            >>::type>
    {
        auto operator() (Direction const & direction, Range && range) const
        RETURNS (range::first (direction, range::detail::get_underlying (
            std::forward <Range> (range))));
    };

    template <class Direction, class Increment, class Range>
        struct drop <view_of_shared_tag, Direction, Increment, Range, typename
            boost::enable_if <has <
                callable::drop (Direction, Increment,
                    typename underlying <Range>::type)
            >>::type>
    {
        auto operator() (Direction const & direction,
            Increment const & increment, Range && range) const
        RETURNS (view_shared_detail::make_view_of_shared() (
            view_shared_detail::get_heavyweight_pointer() (
                std::forward <Range> (range)),
            range::drop (direction, increment,
            range::detail::get_underlying (std::forward <Range> (range)))));
    };

    template <class Direction, class Range>
        struct chop <view_of_shared_tag, Direction, Range, typename
            boost::enable_if <has <
                callable::chop (Direction,
                    typename underlying <Range>::type)
            >>::type>
    {
        typedef typename result_of <callable::chop (Direction,
            typename underlying <Range>::type)>::type underlying_chopped_type;
        typedef typename underlying_chopped_type::first_type first_type;
        typedef typename underlying_chopped_type::rest_type
            underlying_rest_type;

        typedef typename std::decay <Range>::type::heavyweight_type
            heavyweight_type;

        typedef chopped <first_type,
                view_of_shared <heavyweight_type, underlying_rest_type>>
            result_type;

        result_type operator() (Direction const & direction, Range && range)
            const
        {
            auto underlying_chopped = range::chop (direction,
                range::detail::get_underlying (std::forward <Range> (range)));
            return result_type (underlying_chopped.move_first(),
                view_shared_detail::make_view_of_shared() (
                    view_shared_detail::get_heavyweight_pointer() (
                        std::forward <Range> (range)),
                    underlying_chopped.move_rest()));
        }
    };

} // namespace operation

namespace callable {

    struct view_shared {

        /* With MakeView. */

        template <class Heavyweight, class MakeView,
            class View = typename std::decay <typename std::result_of <
                MakeView (Heavyweight &)>::type>::type,
            class Enable = typename boost::enable_if <is_range <Heavyweight>
                >::type>
        view_of_shared <Heavyweight, View>
            operator() (std::shared_ptr <Heavyweight> heavyweight,
                MakeView const & make_view) const
        {
            Heavyweight & h = *heavyweight;
            return view_of_shared <Heavyweight, View> (
                std::move (heavyweight), make_view (h));
        }

        template <class Heavyweight, class MakeView,
            class View = typename std::decay <typename std::result_of <
                MakeView (Heavyweight &)>::type>::type,
            class Enable = typename boost::enable_if <is_range <Heavyweight>
                >::type>
        view_of_shared <typename std::decay <Heavyweight>::type, View>
            operator() (Heavyweight && heavyweight, MakeView const & make_view)
            const
        {
            auto heavyweight_pointer =
                std::make_shared <typename std::decay <Heavyweight>::type> (
                    std::forward <Heavyweight> (heavyweight));
            return operator() (std::move (heavyweight_pointer), make_view);
        }

        /* Without MakeView. */

        template <class Heavyweight, class Enable
            = typename boost::enable_if <is_range <Heavyweight>>::type>
        view_of_shared <Heavyweight>
            operator() (std::shared_ptr <Heavyweight> heavyweight) const
        { return operator() (std::move (heavyweight), range::view); }

        template <class Heavyweight, class Enable
            = typename boost::enable_if <is_range <Heavyweight>>::type>
        view_of_shared <typename std::decay <Heavyweight>::type>
            operator() (Heavyweight && heavyweight) const
        {
            return operator() (
                std::make_shared <typename std::decay <Heavyweight>::type> (
                    std::forward <Heavyweight> (heavyweight)));
        }

    };

} // namespace callable

/**
\brief Return a view on a heavyweight range whose lifetime is tied to the view.

Normally, the onus is on the user to make sure that the container, say, that a
view is on, remains in scope while the view is being used.
However, sometimes this is impossible or undesirable.
For example, returning a range of which the type is an implementation detail is
fraught with difficulty.

The solution is to force the range to stay in memory as long as the view or any
copy or derived view does.
That is what this function does, by keeping the range in a std::shared_ptr.

The optional second argument is a function to convert the range into a view.
This is a function, not the view itself, because the range usually first needs
to be copied into the shared_ptr, before a view of the copy can be produced.

\param heavyweight
    The heavyweight range that the view will be on.
    If this is a std::shared_ptr, a copy of it will be kept.
    If this is a range, then a copy of it will be kept in a shared_ptr.
\param make_view (optional)
    Function that returns the right sort of view of the range.
    If this is not given, \ref view is used.
    Other simple or complex functors are possible.
    For example, use \ref reverse to produce a reversed view of the range.
*/
static auto constexpr view_shared = callable::view_shared();

} // namespace range

#endif // RANGE_VIEW_SHARED_HPP_INCLUDED