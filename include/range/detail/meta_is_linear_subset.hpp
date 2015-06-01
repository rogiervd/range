/*
Copyright 2014 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_IS_META_LINEAR_SUBSET_HPP_INCLUDED
#define RANGE_DETAIL_IS_META_LINEAR_SUBSET_HPP_INCLUDED

#include <meta/vector.hpp>

namespace range { namespace detail {

    /**
    Meta-function that checks whether a meta-range is a linear subset of
    another.
    That is, it assumes that the two meta-ranges have their elements in the same
    order.
    */
    template <class Subset, class Superset> struct is_linear_subset
    : is_linear_subset <
        typename meta::as_vector <Subset>::type,
        typename meta::as_vector <Superset>::type> {};

    // Base cases.
    template <> struct is_linear_subset <meta::vector <>, meta::vector <>>
    : boost::mpl::true_ {};

    template <class ... Types> struct is_linear_subset <
        meta::vector<>, meta::vector <Types ...>>
    : boost::mpl::true_ {};

    template <class ... Types> struct is_linear_subset <
        meta::vector <Types ...>, meta::vector<>>
    : boost::mpl::false_ {};

    // Type found.
    template <class First, class ... RestSubset, class ... RestSuperset>
        struct is_linear_subset <
            meta::vector <First, RestSubset ...>,
            meta::vector <First, RestSuperset ...>>
    : is_linear_subset <meta::vector <RestSubset ...>,
        meta::vector <RestSuperset ...>> {};

    // Type not found.
    template <class FirstSubset, class ... RestSubset,
            class FirstSuperset, class ... RestSuperset>
        struct is_linear_subset <
            meta::vector <FirstSubset, RestSubset ...>,
            meta::vector <FirstSuperset, RestSuperset ...>>
    : is_linear_subset <meta::vector <FirstSubset, RestSubset ...>,
        meta::vector <RestSuperset ...>> {};

}} // namespace range::detail

#endif // RANGE_DETAIL_IS_META_LINEAR_SUBSET_HPP_INCLUDED
