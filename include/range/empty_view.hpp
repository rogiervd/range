// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_EMPTY_VIEW_HPP_INCLUDED
#define RANGE_EMPTY_VIEW_HPP_INCLUDED

#include "rime/core.hpp"

#include "core.hpp"

namespace range {

/**
A view that is empty from all directions.
This can be used from all directions, with "front" as its default direction.
It is essentially equivalent to member_view<>, but more explicit in its
intention.
*/
class empty_view;

namespace empty_view_operation {
    struct empty_view_tag {};
}  // namespace empty_view_operation

template <> struct tag_of_qualified<empty_view>
{
    typedef empty_view_operation::empty_view_tag type;
};

class empty_view
{
private:
    friend class helper::member_access;

    template <class Direction> rime::true_type empty(Direction const &) const
    {
        return rime::true_;
    }

    template <class Direction> rime::size_t<0> size(Direction const &) const
    {
        return rime::size_t<0>();
    }
};

}  // namespace range

#endif  // RANGE_EMPTY_VIEW_HPP_INCLUDED
