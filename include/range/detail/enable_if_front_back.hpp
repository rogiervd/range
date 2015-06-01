/*
Copyright 2013 Rogier van Dalen.

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

#ifndef RANGE_RANGE_DETAIL_ENABLE_IF_FRONT_BACK_HPP_INCLUDED
#define RANGE_RANGE_DETAIL_ENABLE_IF_FRONT_BACK_HPP_INCLUDED

namespace range { namespace operation { namespace detail {

    /**
    Contains type "type" iff the list of directions is front, back, or a
    combination.
    */
    template <class Directions> struct enable_if_front_back {};

    template <>
        struct enable_if_front_back <meta::vector <direction::front>>
    { typedef void type; };

    template <>
        struct enable_if_front_back <meta::vector <direction::back>>
    { typedef void type; };

    template <> struct enable_if_front_back <
        meta::vector <direction::front, direction::back>>
    { typedef void type; };

    template <> struct enable_if_front_back <
        meta::vector <direction::back, direction::front>>
    { typedef void type; };

}}} // namespace range::operation::detail

#endif  // RANGE_RANGE_DETAIL_ENABLE_IF_FRONT_BACK_HPP_INCLUDED

