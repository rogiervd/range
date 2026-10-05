/*
Copyright 2013, 2015, 2017 Rogier van Dalen.

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

/** \file
Define container tags and implement_make_view to help turn them into
iterator_range.
*/

#ifndef RANGE_STD_CONTAINER_DETAIL_CONTAINER_TAG_HPP_INCLUDED
#define RANGE_STD_CONTAINER_DETAIL_CONTAINER_TAG_HPP_INCLUDED

#include "rime/enable_if_constant.hpp"

#include "range/core.hpp"
#include "range/heavyweight.hpp"
#include "range/iterator_range.hpp"

namespace range {

namespace std_container_operation {

    struct std_front_container_tag : heavyweight::heavyweight_tag
    {};
    struct std_front_back_container_tag : std_front_container_tag
    {};

}  // namespace std_container_operation

namespace std_container_operation {

    /* make_iterator_range_with_once. */

    // If Once is not compile-time true.
    template <
        class Once, class Container,
        class Enable = typename rime::disable_if_constant_true<Once>::type>
    inline auto make_iterator_range_with_once(Once once, Container && container)
        RETURNS(make_iterator_range(std::forward<Container>(container)));

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
