// SPDX-FileCopyrightText: Copyright 2013, 2015, 2017 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define container tags and implement_make_view to help turn them into
iterator_range.
*/

#ifndef RANGE_STD_CONTAINER_DETAIL_CONTAINER_TAG_HPP_INCLUDED
#define RANGE_STD_CONTAINER_DETAIL_CONTAINER_TAG_HPP_INCLUDED

#include <type_traits>

#include "rime/enable_if_constant.hpp"

#include "range/core.hpp"
#include "range/heavyweight.hpp"
#include "range/iterator_range.hpp"

namespace range {

namespace std_container_operation {

    struct std_front_container_tag : heavyweight::heavyweight_tag {};
    struct std_front_back_container_tag : std_front_container_tag {};

}  // namespace std_container_operation

namespace std_container_operation {

    /* make_iterator_range_with_once. */

    /*
    make_iterator_range only accepts lvalue containers, since an iterator_range
    over a temporary would contain dangling iterators.
    Here, however, the container is guaranteed to be alive during the operation
    (e.g. "first" or "empty") that is applied to the view.
    Therefore, pass the container as an lvalue.
    An rvalue container is passed as a const lvalue, so that the resulting
    iterator_range does not allow its elements to be modified.
    */
    template <class Container> struct container_as_lvalue
    {
        typedef typename std::conditional<
            std::is_lvalue_reference<Container>::value, Container,
            typename std::remove_reference<Container>::type const &>::type type;
    };

    // If Once is not compile-time true.
    template <
        class Once, class Container,
        class Enable = typename rime::disable_if_constant_true<Once>::type>
    inline auto make_iterator_range_with_once(Once once, Container && container)
        RETURNS(make_iterator_range(
            static_cast<typename container_as_lvalue<Container>::type>(
                container)));

    // If Once is compile-time true.
    template <
        class Once, class Container,
        class Enable = typename rime::enable_if_constant_true<Once>::type>
    inline auto make_iterator_range_with_once(Once once, Container && container)
        RETURNS(make_move_iterator_range(std::forward<Container>(container)));

    /* implement_make_view. */
    /* Allow the correct combinations of front and back. */
    template <class Once, class Container> inline auto implement_make_view(
        std_front_container_tag, Once once, Container && container,
        direction::front)
        RETURNS(make_iterator_range_with_once(
            once, std::forward<Container>(container)));

    template <class Once, class Container> inline auto implement_make_view(
        std_front_back_container_tag, Once once, Container && container,
        direction::back)
        RETURNS(make_iterator_range_with_once(
            once, std::forward<Container>(container)));

    template <class Once, class Container> inline auto implement_make_view(
        std_front_back_container_tag, Once once, Container && container,
        direction::front, direction::back)
        RETURNS(make_iterator_range_with_once(
            once, std::forward<Container>(container)));

    template <class Once, class Container> inline auto implement_make_view(
        std_front_back_container_tag, Once once, Container && container,
        direction::back, direction::front)
        RETURNS(make_iterator_range_with_once(
            once, std::forward<Container>(container)));

}  // namespace std_container_operation

}  // namespace range

#endif  // RANGE_STD_CONTAINER_DETAIL_CONTAINER_TAG_HPP_INCLUDED
