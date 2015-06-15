/*
Copyright 2013, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_all_of_any_of_heterogeneous
#include "utility/test/boost_unit_test.hpp"

#include "range/all_of.hpp"
#include "range/any_of.hpp"

#include "rime/check/check_equal.hpp"

#include <vector>
#include <tuple>
#include "range/std.hpp"

BOOST_AUTO_TEST_SUITE(test_range_all_of_any_of_heterogeneous)

BOOST_AUTO_TEST_CASE (test_range_all_of_any_of_heterogeneous) {
    using range::all_of;
    using range::any_of;
    using range::front;
    using range::back;

    {
        std::tuple <bool> t (false);
        RIME_CHECK_EQUAL (all_of (t, front), false);
        RIME_CHECK_EQUAL (all_of (t, back), false);
        RIME_CHECK_EQUAL (all_of (t), false);

        RIME_CHECK_EQUAL (any_of (t, front), false);
        RIME_CHECK_EQUAL (any_of (t, back), false);
        RIME_CHECK_EQUAL (any_of (t), false);
    }

    {
        std::tuple <bool> t (true);
        RIME_CHECK_EQUAL (all_of (t, front), true);
        RIME_CHECK_EQUAL (all_of (t, back), true);
        RIME_CHECK_EQUAL (all_of (t), true);

        RIME_CHECK_EQUAL (any_of (t, front), true);
        RIME_CHECK_EQUAL (any_of (t, back), true);
        RIME_CHECK_EQUAL (any_of (t), true);
    }

    {
        std::tuple <bool, bool> t (false, false);
        RIME_CHECK_EQUAL (all_of (t, front), false);
        RIME_CHECK_EQUAL (all_of (t, back), false);
        RIME_CHECK_EQUAL (all_of (t), false);

        RIME_CHECK_EQUAL (any_of (t, front), false);
        RIME_CHECK_EQUAL (any_of (t, back), false);
        RIME_CHECK_EQUAL (any_of (t), false);
    }

    {
        std::tuple <bool, bool> t (false, true);
        RIME_CHECK_EQUAL (all_of (t, front), false);
        RIME_CHECK_EQUAL (all_of (t, back), false);
        RIME_CHECK_EQUAL (all_of (t), false);

        RIME_CHECK_EQUAL (any_of (t, front), true);
        RIME_CHECK_EQUAL (any_of (t, back), true);
        RIME_CHECK_EQUAL (any_of (t), true);
    }

    {
        std::tuple <bool, bool> t (true, false);
        RIME_CHECK_EQUAL (all_of (t, front), false);
        RIME_CHECK_EQUAL (all_of (t, back), false);
        RIME_CHECK_EQUAL (all_of (t), false);

        RIME_CHECK_EQUAL (any_of (t, front), true);
        RIME_CHECK_EQUAL (any_of (t, back), true);
        RIME_CHECK_EQUAL (any_of (t), true);
    }

    {
        std::tuple <bool, bool> t (true, true);
        RIME_CHECK_EQUAL (all_of (t, front), true);
        RIME_CHECK_EQUAL (all_of (t, back), true);
        RIME_CHECK_EQUAL (all_of (t), true);

        RIME_CHECK_EQUAL (any_of (t, front), true);
        RIME_CHECK_EQUAL (any_of (t, back), true);
        RIME_CHECK_EQUAL (any_of (t), true);
    }
}

BOOST_AUTO_TEST_CASE (test_range_all_of_any_of_constant) {
    using range::all_of;
    using range::any_of;
    using range::front;
    using range::back;

    {
        std::tuple <> t;
        RIME_CHECK_EQUAL (all_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (all_of (t), rime::true_);

        RIME_CHECK_EQUAL (any_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (any_of (t), rime::false_);
    }

    {
        std::tuple <rime::false_type> t (rime::false_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (any_of (t), rime::false_);
    }

    {
        std::tuple <rime::true_type> t (rime::true_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (all_of (t), rime::true_);

        RIME_CHECK_EQUAL (any_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }

    {
        std::tuple <rime::false_type, rime::false_type> t (
            rime::false_, rime::false_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (any_of (t), rime::false_);
    }

    {
        std::tuple <rime::false_type, rime::true_type> t (
            rime::false_, rime::true_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }

    {
        std::tuple <rime::true_type, rime::false_type> t (
            rime::true_, rime::false_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }

    {
        std::tuple <rime::true_type, rime::true_type> t (
            rime::true_, rime::true_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (all_of (t), rime::true_);

        RIME_CHECK_EQUAL (any_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }

    // Mix of constant and run-time.
    {
        std::tuple <rime::false_type, bool> t (rime::false_, false);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), false);
        RIME_CHECK_EQUAL (any_of (t, back), false);
        RIME_CHECK_EQUAL (any_of (t), false);
    }
    {
        std::tuple <bool, rime::false_type> t (false, rime::false_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), false);
        RIME_CHECK_EQUAL (any_of (t, back), false);
        RIME_CHECK_EQUAL (any_of (t), false);
    }

    {
        std::tuple <rime::false_type, bool> t (rime::false_, true);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), true);
        RIME_CHECK_EQUAL (any_of (t, back), true);
        RIME_CHECK_EQUAL (any_of (t), true);
    }
    {
        std::tuple <bool, rime::false_type> t (true, rime::false_);
        RIME_CHECK_EQUAL (all_of (t, front), rime::false_);
        RIME_CHECK_EQUAL (all_of (t, back), rime::false_);
        RIME_CHECK_EQUAL (all_of (t), rime::false_);

        RIME_CHECK_EQUAL (any_of (t, front), true);
        RIME_CHECK_EQUAL (any_of (t, back), true);
        RIME_CHECK_EQUAL (any_of (t), true);
    }

    {
        std::tuple <rime::true_type, bool> t (rime::true_, false);
        RIME_CHECK_EQUAL (all_of (t, front), false);
        RIME_CHECK_EQUAL (all_of (t, back), false);
        RIME_CHECK_EQUAL (all_of (t), false);

        RIME_CHECK_EQUAL (any_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }
    {
        std::tuple <bool, rime::true_type> t (false, rime::true_);
        RIME_CHECK_EQUAL (all_of (t, front), false);
        RIME_CHECK_EQUAL (all_of (t, back), false);
        RIME_CHECK_EQUAL (all_of (t), false);
    }

    {
        std::tuple <rime::true_type, bool> t (rime::true_, true);
        RIME_CHECK_EQUAL (all_of (t, front), true);
        RIME_CHECK_EQUAL (all_of (t, back), true);
        RIME_CHECK_EQUAL (all_of (t), true);

        RIME_CHECK_EQUAL (any_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }
    {
        std::tuple <bool, rime::true_type> t (true, rime::true_);
        RIME_CHECK_EQUAL (all_of (t, front), true);
        RIME_CHECK_EQUAL (all_of (t, back), true);
        RIME_CHECK_EQUAL (all_of (t), true);

        RIME_CHECK_EQUAL (any_of (t, front), rime::true_);
        RIME_CHECK_EQUAL (any_of (t, back), rime::true_);
        RIME_CHECK_EQUAL (any_of (t), rime::true_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
