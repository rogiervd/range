/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_hash_range
#include "utility/test/boost_unit_test.hpp"

#include "range/hash_range.hpp"

#include <vector>
#include <tuple>
#include <string>
#include <unordered_set>

#include <boost/functional/hash.hpp>

#include "range/tuple.hpp"
#include "range/std.hpp"

using range::tuple;
using range::hash_range;
using range::hash_range_combine;

BOOST_AUTO_TEST_SUITE(test_range_hash_range)

BOOST_AUTO_TEST_CASE (test_range_hash_homogeneous) {
    std::vector <float> v;

    boost::hash <float> hash_float;

    {
        BOOST_CHECK (hash_range (v, range::front) != 0);
        BOOST_CHECK (hash_range (v) != 0);

        std::size_t seed = 27;

        hash_range_combine (v, range::front, seed);
        BOOST_CHECK_EQUAL (seed, 27);

        hash_range_combine (v, seed);
        BOOST_CHECK_EQUAL (seed, 27);
    }

    v.push_back (7.5);

    {
        BOOST_CHECK_EQUAL (hash_range (v, range::front), hash_float (7.5));
        BOOST_CHECK_EQUAL (hash_range (v), hash_float (7.5));

        {
            std::size_t seed = 27;
            std::size_t reference_seed = 27;

            boost::hash_range (reference_seed, v.begin(), v.end());

            hash_range_combine (v, range::front, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 27;
            hash_range_combine (v, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }

    v.push_back (-17);

    {
        std::size_t reference = hash_float (7.5);
        boost::hash_combine (reference, float (-17));
        BOOST_CHECK_EQUAL (hash_range (v, range::front), reference);
        BOOST_CHECK_EQUAL (hash_range (v), reference);

        {
            std::size_t seed = 27;
            std::size_t reference_seed = 27;

            // boost::hash_range has strictly defined behaviour.
            boost::hash_range (reference_seed, v.begin(), v.end());

            hash_range_combine (v, range::front, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 27;
            hash_range_combine (v, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }

    v.push_back (35.25);

    {
        std::size_t reference = hash_float (7.5);
        boost::hash_combine (reference, float (-17));
        boost::hash_combine (reference, float (35.25));
        BOOST_CHECK_EQUAL (hash_range (v, range::front), reference);
        BOOST_CHECK_EQUAL (hash_range (v), reference);
    }
}

BOOST_AUTO_TEST_CASE (test_range_hash_heterogeneous) {
    boost::hash <double> hash_double;
    {
        std::tuple <> t;

        BOOST_CHECK (hash_range (t, range::front) != 0);
        BOOST_CHECK (hash_range (t) != 0);
    }

    {
        std::tuple <double> t (6.3);

        {
            BOOST_CHECK_EQUAL (hash_range (t, range::front), hash_double (6.3));
            BOOST_CHECK_EQUAL (hash_range (t), hash_double (6.3));
        }

        {
            std::size_t reference_seed = 81;
            boost::hash_combine (reference_seed, double (6.3));

            std::size_t seed = 81;
            hash_range_combine (t, range::front, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 81;
            hash_range_combine (t, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }

    {
        range::tuple <double, std::string> const t (10.4, "hello");

        {
            std::size_t reference_seed = hash_double (10.4);
            boost::hash_combine (reference_seed, std::string ("hello"));

            BOOST_CHECK_EQUAL (hash_range (t, range::front),
                reference_seed);
            BOOST_CHECK_EQUAL (hash_range (t), reference_seed);
        }

        {
            std::size_t reference_seed = 81;
            boost::hash_combine (reference_seed, double (10.4));
            boost::hash_combine (reference_seed, std::string ("hello"));

            std::size_t seed = 81;
            hash_range_combine (t, range::front, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);

            seed = 81;
            hash_range_combine (t, seed);
            BOOST_CHECK_EQUAL (seed, reference_seed);
        }
    }
}

BOOST_AUTO_TEST_CASE (test_range_hash_properties) {
    std::vector <double> v;

    range::tuple <> empty;
    range::tuple <double> zero (0);
    range::tuple <double> four_half (4.5);
    range::tuple <double, double> four_half_seven (4.5, 7);
    range::tuple <double, double, double> four_half_seven_twenty (4.5, 7, 20);

    // An empty range has a different hash value from a range with one element
    // with the value 0.
    BOOST_CHECK (hash_range (empty) != hash_range (zero));
    BOOST_CHECK (hash_range (empty) != hash_range (four_half));
    BOOST_CHECK (hash_range (zero) != hash_range (four_half));
    BOOST_CHECK (hash_range (four_half) != hash_range (four_half_seven));
    BOOST_CHECK (hash_range (four_half) != hash_range (four_half_seven_twenty));
    BOOST_CHECK (hash_range (four_half_seven)
        != hash_range (four_half_seven_twenty));

    BOOST_CHECK_EQUAL (hash_range (v), hash_range (empty));

    v.push_back (0);
    BOOST_CHECK_EQUAL (hash_range (v), hash_range (zero));
    v[0] = 4.5;
    BOOST_CHECK_EQUAL (hash_range (v), hash_range (four_half));

    v.push_back (7);
    BOOST_CHECK_EQUAL (hash_range (v), hash_range (four_half_seven));

    v.push_back (20);
    BOOST_CHECK_EQUAL (hash_range (v), hash_range (four_half_seven_twenty));
}

BOOST_AUTO_TEST_CASE (test_range_hash_unordered_set) {
    std::unordered_set <range::tuple <int, std::string>,
        range::callable::hash_range> s;

    auto t1 = range::make_tuple (5, "test");
    auto t2 = range::make_tuple (7, "test");
    auto t3 = range::make_tuple (7, "hello");

    BOOST_CHECK (hash_range (t1) != hash_range (t2));
    BOOST_CHECK (hash_range (t1) != hash_range (t3));
    BOOST_CHECK (hash_range (t2) != hash_range (t3));

    s.insert (t1);
    BOOST_CHECK (s.find (t1) != s.end());
    BOOST_CHECK (s.find (t2) == s.end());

    s.insert (t2);
    BOOST_CHECK (s.find (t1) != s.end());
    BOOST_CHECK (s.find (t2) != s.end());
    BOOST_CHECK (s.find (t3) == s.end());

    s.insert (t3);
    BOOST_CHECK (s.find (t1) != s.end());
    BOOST_CHECK (s.find (t2) != s.end());
    BOOST_CHECK (s.find (t3) != s.end());

    s.erase (t2);
    BOOST_CHECK (s.find (t1) != s.end());
    BOOST_CHECK (s.find (t2) == s.end());
    BOOST_CHECK (s.find (t3) != s.end());
}

BOOST_AUTO_TEST_SUITE_END()
