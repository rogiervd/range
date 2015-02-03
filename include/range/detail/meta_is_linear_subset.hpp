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
