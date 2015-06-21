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

#define BOOST_TEST_MODULE test_range_chain
#include "utility/test/boost_unit_test.hpp"

#include "range/chain.hpp"

#include <vector>
#include <tuple>

#include "range/std.hpp"

template <class ... Types> struct show_types;

BOOST_AUTO_TEST_SUITE(test_range_chain)

BOOST_AUTO_TEST_CASE (detail_non_empty_indices) {
    using range::chain_detail::non_empty_types;
    using range::chain_detail::non_empty_indices;

    // Empty.
    static_assert (std::is_same <non_empty_types <
        meta::vector<>>::type,
        meta::vector<>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector<>>::type,
        meta::vector<>
        >::value, "");

    // One type.
    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple<>>>::type,
        meta::vector<>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple<>>>::type,
        meta::vector<>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple <bool>>>::type,
        meta::vector <std::tuple <bool>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple <bool>>>::type,
        meta::vector <boost::mpl::size_t <0>>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::vector <int>>>::type,
        meta::vector <std::vector <int>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::vector <int>>>::type,
        meta::vector <boost::mpl::size_t <0>>
        >::value, "");

    // Two types.
    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple<>, std::tuple<>>>::type,
        meta::vector<>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple<>, std::tuple<>>>::type,
        meta::vector<>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple<>, std::tuple <int>>>::type,
        meta::vector <std::tuple <int>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple<>, std::tuple <int>>>::type,
        meta::vector <boost::mpl::size_t <1>>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple<>, std::vector <int>>>::type,
        meta::vector <std::vector <int>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple<>, std::vector <int>>>::type,
        meta::vector <boost::mpl::size_t <1>>
        >::value, "");


    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple <float>, std::tuple<>>>::type,
        meta::vector <std::tuple <float>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple <float>, std::tuple<>>>::type,
        meta::vector <boost::mpl::size_t <0>>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple <float>, std::tuple <int>>>::type,
        meta::vector <std::tuple <float>, std::tuple <int>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple <float>, std::tuple <int>>>::type,
        meta::vector <boost::mpl::size_t <0>, boost::mpl::size_t <1>>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::tuple <float>, std::vector <int>>>::type,
        meta::vector <std::tuple <float>, std::vector <int>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::tuple <float>, std::vector <int>>>::type,
        meta::vector <boost::mpl::size_t <0>, boost::mpl::size_t <1>>
        >::value, "");


    static_assert (std::is_same <non_empty_types <
        meta::vector <std::vector <bool>, std::tuple<>>>::type,
        meta::vector <std::vector <bool>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::vector <bool>, std::tuple<>>>::type,
        meta::vector <boost::mpl::size_t <0>>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::vector <bool>, std::tuple <int>>>::type,
        meta::vector <std::vector <bool>, std::tuple <int>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::vector <bool>, std::tuple <int>>>::type,
        meta::vector <boost::mpl::size_t <0>, boost::mpl::size_t <1>>
        >::value, "");

    static_assert (std::is_same <non_empty_types <
        meta::vector <std::vector <bool>, std::vector <int>>>::type,
        meta::vector <std::vector <bool>, std::vector <int>>
        >::value, "");
    static_assert (std::is_same <non_empty_indices <
        meta::vector <std::vector <bool>, std::vector <int>>>::type,
        meta::vector <boost::mpl::size_t <0>, boost::mpl::size_t <1>>
        >::value, "");
}

BOOST_AUTO_TEST_CASE (test_chain_from) {
    using range::chain_from;
    using range::chained;
    {
        std::tuple<> sub_ranges;
        auto c = chain_from (sub_ranges);
        static_assert (std::is_same <decltype (c), chained<>>::value, "");
    }
    {
        std::tuple <std::tuple<>> sub_ranges;
        auto c = chain_from (sub_ranges);
        static_assert (std::is_same <decltype (c), chained<>>::value, "");
    }
    {
        std::tuple <std::tuple <int>> sub_ranges;
        auto c = chain_from (sub_ranges);
        static_assert (std::is_same <decltype (c), chained <std::tuple <int>>>::value, "");
    }
}

BOOST_AUTO_TEST_SUITE_END()
