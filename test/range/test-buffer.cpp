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

#define BOOST_TEST_MODULE test_range_buffer
#include "utility/test/boost_unit_test.hpp"

#include "range/buffer.hpp"

#include "utility/test/tracked.hpp"

#include "range/count.hpp"
#include "range/for_each_macro.hpp"
#include "range/std/container.hpp"

#include "unique_range.hpp"

using range::empty;
using range::first;
using range::drop;
using range::chop;
using range::chop_in_place;

BOOST_AUTO_TEST_SUITE(test_range_buffer)

BOOST_AUTO_TEST_CASE (count) {
    range::buffer <std::size_t> count (one_time_view (range::count()));

    BOOST_CHECK_EQUAL (first (count), 0);
    BOOST_CHECK_EQUAL (first (drop (count)), 1);
    BOOST_CHECK_EQUAL (first (drop (drop (count))), 2);

    RANGE_FOR_EACH (iteration, range::count (3)) {
        auto count2 = count;
        RANGE_FOR_EACH (i, range::count (1000)) {
            BOOST_CHECK_EQUAL (first (count2), i);
            count2 = drop (count2);
        }
        (void) iteration;
    }
}

BOOST_AUTO_TEST_CASE (tracked) {
    typedef utility::tracked <std::size_t> tracked;
    RANGE_FOR_EACH (size_step, range::count (20)) {
        std::size_t size = size_step * 5;
        utility::tracked_registry r;
        {
            std::vector <tracked> v;
            RANGE_FOR_EACH (i, range::count (size))
                v.push_back (tracked (r, i));

            range::buffer <tracked, 7> b (v);
            RANGE_FOR_EACH (i, range::count (size)) {
                BOOST_CHECK_EQUAL (first (b).content(), i);
                switch (i % 3)
                {
                case 0:
                    b = drop (b);
                    break;
                case 1:
                    {
                        auto chopped = chop (std::move (b));
                        BOOST_CHECK_EQUAL (chopped.first().content(), i);
                        b = chopped.move_rest();
                    }
                    break;
                default:
                    auto f = chop_in_place (b);
                    BOOST_CHECK_EQUAL (f.content(), i);
                    break;
                }

                // Check the number of elements alive at each time.
                // This should be the size of v plus the size of the buffer.
                // Only one buffer should be alive at any time.
                BOOST_CHECK (r.alive_count()
                    <= size + 7);
            }
            BOOST_CHECK (empty (b));
        }
    }
}

BOOST_AUTO_TEST_CASE (stack_overflow) {
    range::buffer <std::size_t, 1> count (range::count());
    auto count2 = count;
    // Reserve 100000 buffers.
    // It will break the stack if they are destructed recursively.
    RANGE_FOR_EACH (iteration, range::count (100000)) {
        count2 = drop (count2);
        (void) iteration;
    }
}

BOOST_AUTO_TEST_SUITE_END()
