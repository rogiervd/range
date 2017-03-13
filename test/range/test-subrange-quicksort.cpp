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

#define BOOST_TEST_MODULE test_range_subrange_quicksort
#include "utility/test/boost_unit_test.hpp"

#include "range/subrange.hpp"

#include <random>

#include <vector>
#include <cassert>
#include <utility> // for std::swap.
#include <algorithm> // for std::sort, as comparison.

#include "range/std/container.hpp"
#include "range/for_each_macro.hpp"
#include "range/count.hpp"
#include "range/zip.hpp"
#include "range/transform.hpp"
#include "range/call_unpack.hpp"
#include "range/all_of.hpp"

template <class Range, class Predicate>
    void sort_in_place (Range const & range, Predicate && predicate)
{
    using range::empty;
    using range::first;
    using range::drop;
    using range::front;
    using range::back;

    using range::make_subrange;

    using std::swap;

    // std::cout << "Sorting a range of " << range::size (range) << " elements..." << std::endl;

    if (empty (range, front))
        return;

    auto & pivot = first (range, back);
    assert (!predicate (pivot, pivot));

    auto unpartitioned = make_subrange (make_subrange (range, back), front);
    // Leave the pivot out.
    unpartitioned = drop (unpartitioned, back);

    if (empty (unpartitioned, front))
        // One element: sorted.
        return;

    while (true) {
        // Check the front of the range.
        // Here, predicate (element, pivot) should be true.
        while (!empty (unpartitioned, front) &&
            predicate (first (unpartitioned, front), pivot))
        {
            unpartitioned = drop (unpartitioned, front);
        }
        if (empty (unpartitioned))
            break;

        // Swap first (unpartitioned, front) with something.
        // Since it is known that it should go at the other end, why not put
        // it there.
        // \todo Problem when size() == 1?
        swap (first (unpartitioned, front), first (unpartitioned, back));
        assert (!predicate (first (unpartitioned, back), pivot));

        // Check the back of the range. 
        // Here, predicate (element, pivot) should be false.
        while (!empty (unpartitioned, back) &&
            !predicate (first (unpartitioned, back), pivot))
        {
            unpartitioned = drop (unpartitioned, back);
        }
        if (empty (unpartitioned))
            break;

        swap (first (unpartitioned, back), first (unpartitioned, front));
        assert (predicate (first (unpartitioned, front), pivot));
    }

    // Sort the elements less than the pivot.
    // std::cout << "Sorting left of " << range::size (unpartitioned.before().underlying()) << " elements..." << std::endl;
    sort_in_place (unpartitioned.before().underlying(), predicate);

    auto right = unpartitioned.underlying().before();
    // Move the pivot to the front of the right-hand range.
    swap (first (right, front), first (right, back));
    // And leave the pivot where it was.
    right = drop (right, front);
    // std::cout << "Sorting right of " << range::size (right) << " elements..." << std::endl;
    // Sort the elements not less than the pivot.
    sort_in_place (right, predicate);
}

template <class Range> bool is_sorted (Range const & range) {
    if (range::empty (range))
        return true;

    auto consecutive_pairs = range::zip (range, range::drop (range));
    auto are_sorted = range::transform (consecutive_pairs,
        range::call_unpack (std::less_equal <int>()));
    return range::all_of (are_sorted);
}

BOOST_AUTO_TEST_SUITE(test_range_subrange_quicksort)

BOOST_AUTO_TEST_CASE (test_quicksort) {
    std::default_random_engine random_engine (2);
    std::uniform_int_distribution <int> uniform (-128, +128);

    RANGE_FOR_EACH (size, range::count (128)) {
        (void) size;
        std::vector <int> v;
        RANGE_FOR_EACH (i, range::count (size)) {
            v.push_back (uniform (random_engine));
            (void) i;
        }
        std::vector <int> reference = v;

        std::cout << "Unsorted: [ ";
        RANGE_FOR_EACH (e, v)
            std::cout << e << ' ';
        std::cout << "]" << std::endl;

        if (size > 10) {
            // This is exceedingly unlikely to be sorted.
            BOOST_CHECK (!is_sorted (v));
        }

        sort_in_place (range::view (v), std::less <int>());

        std::cout << "Sorted:   [ ";
        RANGE_FOR_EACH (e, v)
            std::cout << e << ' ';
        std::cout << "]" << std::endl;

        BOOST_CHECK (is_sorted (v));

        std::sort (reference.begin(), reference.end());

        BOOST_CHECK_EQUAL_COLLECTIONS (v.begin(), v.end(), reference.begin(), reference.end());

        if (size > 0) {
            RANGE_FOR_EACH (i, range::count (size-1)) {
                BOOST_CHECK (v [i] <= v [i+1]);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
