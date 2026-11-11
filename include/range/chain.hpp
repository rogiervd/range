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


#ifndef RANGE_CHAIN_HPP_INCLUDED
#define RANGE_CHAIN_HPP_INCLUDED

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/pair.hpp>

#include "meta/range.hpp"
#include "meta/any.hpp"
#include "meta/vector.hpp"
#include "meta/enumerate.hpp"
#include "meta/filter.hpp"
#include "meta/transform.hpp"

#include "utility/returns.hpp"

#include "rime/assert.hpp"
#include "rime/always.hpp"

#include "core.hpp"
#include "tuple.hpp"
#include "element_types.hpp"

#include "detail/enable_if_front_back.hpp"

namespace range {

// \todo Test with subrange with "back" as default direction.

template <class ... SubRanges> class chained {
    tuple <SubRanges ...> sub_ranges_;

    static_assert (!meta::any <meta::vector <
            always_empty <direction::front, SubRanges> ...>
        >::value, "Known-empty ranges should have been removed.");
public:
    chained (SubRanges const & ... sub_ranges) 
    : sub_ranges_ (sub_ranges ...) {}

    tuple <SubRanges ...> const & sub_ranges() const { return sub_ranges_; }
};

template <class ... SubTags> struct chained_tag;

template <class ... SubRanges> struct tag_of_qualified <chained <SubRanges ...>>
{ typedef chained_tag <typename tag_of <SubRanges>::type ...> type; };

namespace chain_detail {

    // \todo Make views here.

    /**
    Return a meta::vector with the types that are not known to be empty.
    */
    template <class Types> struct non_empty_types
    : meta::as_vector <meta::filter <
        boost::mpl::not_<always_empty <direction::front, boost::mpl::_1>>,
        Types>> {};

    /**
    Return a meta::vector with the indices of the types that are not known to be
    empty.
    */
    template <class Types> struct non_empty_indices {
        typedef typename meta::enumerate <Types>::type numbered_types;
        typedef meta::filter <boost::mpl::not_<always_empty <direction::front,
                boost::mpl::second <boost::mpl::_1>>>, numbered_types>
            non_empty_numbered_types;
        typedef typename meta::as_vector <
            meta::transform <boost::mpl::first <boost::mpl::_1>,
                non_empty_numbered_types>>::type type;
    };

    /**
    Produce a chained <Types ...> type without the types from Types that are
    known to be empty.
    */
    template <class Types,
        class NonEmptyTypes = typename non_empty_types <Types>::type>
    struct make_chained;

    template <class Types, class ... NonEmptyTypes>
        struct make_chained <Types, meta::vector <NonEmptyTypes ...>>
    { typedef chained <NonEmptyTypes ...> type; };

    template <class Range> struct non_empty_indices_from
    : non_empty_indices <typename element_types <Range>::type> {};

    template <class Range> struct make_chained_from
    : make_chained <typename element_types <Range>::type> {};

    template <class Range, class ... Indices,
        class Result = typename chain_detail::make_chained_from <Range>::type>
    inline Result chain_from (Range && range, meta::vector <Indices ...>)
    { return Result (range::at (Indices(), std::forward <Range> (range)) ...); }

} // namespace chain_detail

template <class Range,
    class Result = typename chain_detail::make_chained_from <Range>::type>
inline Result chain_from (Range && range)
{
    return chain_detail::chain_from (std::forward <Range> (range),
        typename chain_detail::non_empty_indices_from <Range>::type());
}

namespace operation {

    // empty.
    // No ranges: always empty.
    template <> struct empty <chained_tag<>, direction::front>
    : rime::callable::always_default <rime::true_type> {};

    // One range: forward to that range.
    template <class SubTag>
        struct empty <chained_tag <SubTag>, direction::front>
    {
        template <class Chained> auto operator() (
            direction::front const & direction, Chained const & chained) const
        RETURNS (range::empty (direction, range::first (chained.sub_ranges())));
    };

    // More ranges: never empty.
    template <class ... SubTags>
        struct empty <chained_tag <SubTags ...>, direction::front>
    : rime::callable::always_default <rime::false_type> {};

    // size: not (currently) implemented.

    // first.
    template <class ... SubTags, class Direction>
        struct first <chained_tag <SubTags ...>, Direction, typename
            detail::enable_if_front_back <meta::vector <Direction>>::type>
    {
        template <class Chained> auto operator() (
            Direction const & direction, Chained const & chained) const
        RETURNS (range::first (direction,
            range::first (direction, chained.sub_ranges())));
    };

    // drop.
    /*template <class ... SubTags>
        struct drop_one <chained_tag <SubTags ...>, direction::front>
    {
        template <class FirstSubRange> struct new_first_sub_range
        : std::result_of <
            range::callable::drop (direction::front, FirstSubRange)> {};

        template <class FirstSubRange> struct will_be_empty
        : always_empty <typename new_first_sub_range <FirstSubRange>::type> {};

        template <class One, class FirstSubRange, class SubRanges ...,
            class Enable = typename std::enable_if <
                !will_be_empty <FirstSubRange>::value>::type>
            ... operator() (direction::front const & direction, One const & one,
                chained <FirstSubRange, SubRanges ...> const & c) const
        {
            typedef typename new_first_sub_range <FirstSubRange>::type
                new_first_type;
            auto new_first = range::drop (direction,
                range::first (direction, chained.sub_ranges()));
            return chained <new_first_type, SubRanges ...> (new_first, )
        }

        template <class One, class FirstSubRange, class SubRanges ...,
            class Enable = typename std::enable_if <
                will_be_empty <FirstSubRange>::value>::type>
            ... operator() (direction::front const & direction, One const & one,
                chained <FirstSubRange, SubRanges ...> const & c) const
        {
            auto new_first = range::drop (direction,
                range::first (direction, chained.sub_ranges()));

        }
    };*/

} // namespace operation

} // namespace range

#endif // RANGE_CHAIN_HPP_INCLUDED
