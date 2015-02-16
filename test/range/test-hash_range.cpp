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

#define BOOST_TEST_MODULE test_range_hash_range
#include "utility/test/boost_unit_test.hpp"

#include "range/hash_range.hpp"

#include <vector>
#include <tuple>
#include <string>

#include <boost/functional/hash.hpp>

#include "range/tuple.hpp"
#include "range/std.hpp"

using range::tuple;

BOOST_AUTO_TEST_SUITE(test_range_hash_range)

BOOST_AUTO_TEST_CASE (test_range_hash_homogeneous) {
    // boost::hash_range has strictly defined behaviour so this can be used.
    std::vector <float> v;

    {
        BOOST_CHECK_EQUAL (range::hash_range (range::front, v),
            boost::hash_range (v.begin(), v.end()));
        BOOST_CHECK_EQUAL (range::hash_range (v),
            boost::hash_range (v.begin(), v.end()));

        std::size_t seed = 27;

        range::hash_range (range::front, seed, v);
        BOOST_CHECK_EQUAL (seed, 27);

        range::hash_range (seed, v);
        BOOST_CHECK_EQUAL (seed, 27);
    }

    v.push_back (7.5);

    {
        BOOST_CHECK_EQUAL (range::hash_range (range::front, v),
            boost::hash_range (v.begin(), v.end()));
        BOOST_CHECK_EQUAL (range::hash_range (v),
            boost::hash_range (v.begin(), v.end()));

        {
            std::size_t seed = 27;
            std::size_t reference_seed = 27;

            boost::hash_range (reference_seed, v.begin(), v.end());

            range::hash_range (range::front, seed, v);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 27;
            range::hash_range (seed, v);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }

    v.push_back (-17);

    {
        BOOST_CHECK_EQUAL (range::hash_range (range::front, v),
            boost::hash_range (v.begin(), v.end()));
        BOOST_CHECK_EQUAL (range::hash_range (v),
            boost::hash_range (v.begin(), v.end()));

        {
            std::size_t seed = 27;
            std::size_t reference_seed = 27;

            boost::hash_range (reference_seed, v.begin(), v.end());

            range::hash_range (range::front, seed, v);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 27;
            range::hash_range (seed, v);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }
}

BOOST_AUTO_TEST_CASE (test_range_hash_heterogeneous) {
    // boost::hash_range has strictly defined behaviour so this can be used.
    {
        std::tuple <> t;

        BOOST_CHECK_EQUAL (range::hash_range (range::front, t), 0);
        BOOST_CHECK_EQUAL (range::hash_range (t), 0);
    }

    {
        std::tuple <double> t (6.3);

        {
            std::size_t reference_seed = 0;
            boost::hash_combine (reference_seed, double (6.3));

            BOOST_CHECK_EQUAL (range::hash_range (range::front, t),
                reference_seed);
            BOOST_CHECK_EQUAL (range::hash_range (t), reference_seed);
        }

        {
            std::size_t reference_seed = 81;
            boost::hash_combine (reference_seed, double (6.3));

            std::size_t seed = 81;
            range::hash_range (range::front, seed, t);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 81;
            range::hash_range (seed, t);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }

    {
        range::tuple <double, std::string> const t (10.4, "hello");

        {
            std::size_t reference_seed = 0;
            boost::hash_combine (reference_seed, double (10.4));
            boost::hash_combine (reference_seed, std::string ("hello"));

            BOOST_CHECK_EQUAL (range::hash_range (range::front, t),
                reference_seed);
            BOOST_CHECK_EQUAL (range::hash_range (t), reference_seed);
        }

        {
            std::size_t reference_seed = 81;
            boost::hash_combine (reference_seed, double (10.4));
            boost::hash_combine (reference_seed, std::string ("hello"));

            std::size_t seed = 81;
            range::hash_range (range::front, seed, t);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 81;
            range::hash_range (seed, t);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
