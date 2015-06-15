/*
Copyright 2013-2015 Rogier van Dalen.

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

/**
Helper to test the behaviour of a range with respect to all operations.
*/

#ifndef RANGE_TEST_CHECK_EQUAL_BEHAVIOUR_HPP_INCLUDED
#define RANGE_TEST_CHECK_EQUAL_BEHAVIOUR_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include "rime/core.hpp"
#include "rime/call_if.hpp"

#include "range/core.hpp"

/**
Check whether values are equal in the applicable sense.
If the reference is a constant, then value should be a constant of the same
type and same value.
If the reference is a run-time value, then value should be of the same type
and compare equal.
*/
template <class Value, class Reference>
    typename boost::enable_if <rime::is_constant <Reference>>::type
    check_equal_value (Value const & value, Reference const & reference)
{
    static_assert (rime::is_rime_constant <Value>::value,
        "Value must be a Rime constant");
    static_assert (rime::same_constant <Value, Reference>::value,
        "Value must be of same type and have the same value as Reference");
}

template <class Value, class Reference>
    typename boost::disable_if <rime::is_constant <Reference>>::type
    check_equal_value (Value const & value, Reference const & reference)
{
    static_assert (std::is_same <Value, Reference>::value,
        "value and reference must be of the same type");
    BOOST_CHECK_EQUAL (value, reference);
}

// Specialise for pairs, because BOOST_CHECK_EQUAL does not deal with those.
template <class Value1, class Value2, class Reference1, class Reference2>
    void check_equal_value (std::pair <Value1, Value2> const & value,
        std::pair <Reference1, Reference2> const & reference)
{
    static_assert (std::is_same <Value1, Reference1>::value,
        "value and reference must be of the same type");
    static_assert (std::is_same <Value2, Reference2>::value,
        "value and reference must be of the same type");
    BOOST_CHECK_EQUAL (value.first, reference.first);
    BOOST_CHECK_EQUAL (value.second, reference.second);
}

namespace detail {

// Do nothing.
namespace callable {
    struct nothing {
        template <class ... Arguments>
            void operator() (Arguments const & ...) const {}
    };
} // namespace callable

static const auto nothing = callable::nothing();

/* Helpers. */

// empty.
// HasBack is not used: empty (back, range) is provided automatically even if
// back is not a valid direction.
template <class Range, class Reference>
    void check_empty (Range const & range, Reference const & reference)
{
    static_assert (range::has <range::callable::empty (Range)>::value, "");
    static_assert (range::has <range::callable::empty (
        Range, direction::front)>::value, "");
    static_assert (range::has <range::callable::empty (
        Range, direction::back)>::value, "");
    check_equal_value (range::empty (range), range::empty (reference));
    check_equal_value (range::empty (range, range::front),
        range::empty (reference, range::front));
    check_equal_value (range::empty (range, range::back),
        range::empty (range, range::front));
}

// Check size.
template <class HasSize, class Range, class Reference>
    typename boost::enable_if <HasSize>::type
    check_size (Range const & range, Reference const & reference)
{
    static_assert (range::has <range::callable::size (
        Range, direction::front)>::value, "");
    static_assert (range::has <range::callable::size (
        Range, direction::back)>::value, "");
    check_equal_value (range::size (range, range::front),
        range::size (reference, range::front));
    check_equal_value (range::size (range, range::back),
        range::size (range, range::front));
}
template <class HasSize, class Range, class Reference>
    typename boost::disable_if <HasSize>::type
    check_size (Range const & range, Reference const & reference, void * = 0)
{
    static_assert (!range::has <range::callable::size (
        Range, direction::front)>::value, "");
}

/* Main loop. */

template <class HasSize, class HasBack, class InitialDirection>
struct check_equal_behaviour_recursive_from;

template <class HasSize, class HasBack, class Range, class Reference>
void check_equal_behaviour_recursive (bool use_chop,
    Range const & range, Reference const & reference)
{
    detail::check_empty (range, reference);
    detail::check_size <HasSize> (range, reference);

    // Stop if either range or reference is empty, to prevent further errors.
    // If they are different, this will have been detected above.
    rime::call_if (
        rime::and_ (!range::empty (range), !range::empty (reference)),
        check_equal_behaviour_recursive_from <
            HasSize, HasBack, direction::front>(),
        nothing,
        use_chop, range, reference);

    rime::call_if (rime::and_ (!range::empty (range), !range::empty (reference),
            HasBack()),
        check_equal_behaviour_recursive_from <
            HasSize, HasBack, direction::back>(),
        nothing,
        use_chop, range, reference);
}

/**
Check first and drop, or chop.
On one recursion depth, first and drop are used; on the next, chop.
(It would be possible to use both recursively at both depths but that seems
rather silly and would lead to exponential complexity in the length of the
sequence.)
*/
template <class HasSize, class HasBack, class InitialDirection>
struct check_equal_behaviour_recursive_from {
    template <class Range, class Reference>
        void operator() (bool use_chop,
            Range const & range, Reference const & reference) const
    {
        assert (!range::empty (range));

        InitialDirection d;

        check_equal_value (
            ::range::first (range, d), ::range::first (reference, d));
        auto first_and_rest = ::range::chop (range, d);
        check_equal_value (
            first_and_rest.forward_first(), ::range::first (reference, d));

        if (!use_chop) {
            check_equal_behaviour_recursive <HasSize, HasBack> (!use_chop,
                ::range::drop (range, d), ::range::drop (reference, d));
        } else {
            check_equal_behaviour_recursive <HasSize, HasBack> (!use_chop,
                first_and_rest.forward_rest(), ::range::drop (reference, d));
        }
    }
};

// Check that drop (n, range) is internally consistent.
/*template <class Direction> struct check_drop_n_runtime {
    template <class Range> void operator() (Range const & range) const {
        Direction d;
        auto current = range::view (range);
        std::size_t size = range::size (range, d);
        for (std::size_t i = 0; i != size; ++ i) {
            auto r = range::drop (range, i, d);
            static_assert (std::is_same <
                decltype (r), decltype (current)>::value, "");
            check_equal_value (range::empty (r, d),
                range::empty (current, d));
            check_equal_value (range::size (r, d),
                range::size (current, d));
            check_equal_value (range::first (r, d),
                range::first (current, d));

            current = range::drop (d, current);
        }

        assert (range::empty (current));
        auto r = range::drop (range, range::size (range, d), d);
        check_equal_value (range::empty (current), range::empty (r));
    }
};
*/

template <class HasSize, class HasBack, class Zero>
    struct check_drop_n
{
    Zero zero;
    check_drop_n() : zero() {}

    /**
    Check equality of two ranges under drop with increment.
    This means that after drop, empty, size, and first function the same as a
    reference range.
    This works with empty and non-empty ranges.
    */
    class step {
    private:
        Zero zero;
        rime::size_t <1> one;

        /* check_first_equal */
        // Not known at compile time that the range is empty.
        // (It may be known to be non-empty, in which case this happily
        // compiles.)
        template <class Range, class Direction>
            typename boost::disable_if <range::always_empty <Range, Direction>
            >::type check_first_equal (
                Range const & range1, Range const & range2,
                Direction const & direction) const
        {
            if (!range::empty (range1, direction))
                check_equal_value (range::first (range1, direction),
                    range::first (range2, direction));
        }
        // Known only at compile time that the range is empty.
        template <class Range, class Direction>
            typename boost::enable_if <range::always_empty <Range, Direction>
            >::type check_first_equal (
                Range const & range1, Range const & range2,
                Direction const & direction) const {}

        template <class Range1, class Direction, class Range2>
            void check_equal (Range1 const & range1, Range2 const & range2,
                Direction const & direction) const;
        // Not defined.

        /**
        Check that range1 and range2 are equal under empty, size, and first.
        */
        template <class Range, class Direction>
            void check_equal (Range const & range1, Range const & range2,
                Direction const & direction) const
        {
            check_equal_value (range::empty (range1, direction),
                range::empty (range2, direction));
            check_size <HasSize> (range1, range2);
            check_first_equal (range1, range2, direction);
        }

        /**
        Check that range1 and range2 are equal under \c at with increment.
        \c reference must have already had \c increment elements dropped.
        */
        struct check_at {
            template <class Range1, class Range2,
                class Increment, class Direction>
            void operator() (
                Range1 const & current, Range2 const & reference, Increment gap,
                    Direction const & direction) const
            {
                check_equal_value (range::at (current, gap, direction),
                    range::first (reference, direction));
            }
        };

    public:
        step() : zero() {}

        /**
        \param direction The direction to be checked.
        \param current The current range to be checked.
        \param reference The reference range, with \c gap elements already
            dropped.
        \param gap The amount of elements to be dropped from reference.
        */
        template <class Range1, class Range2, class Increment, class Direction>
        void operator() (Range1 const & current, Range2 const & reference,
            Increment gap, Direction const & direction) const
        {
            auto new_gap = gap + one;
            auto new_current = range::drop (current, new_gap, direction);
            auto new_reference = range::drop (reference, direction);

            // Check current position: empty, size, first, and then at.
            check_equal (new_current, new_reference, direction);

            rime::call_if (!range::empty (new_current), check_at(), nothing,
                current, reference, gap, direction);

            // Recursively check combinations of drop with increment.
            // Recurse with drop (n + 1).
            rime::call_if (!range::empty (new_current), *this, nothing,
                current, new_reference, new_gap, direction);

            // Recurse starting at this point.
            rime::call_if (!range::empty (new_current), *this, nothing,
                new_current, new_reference, zero, range::front);
            rime::call_if (rime::and_ (!range::empty (new_current), HasBack()),
                *this, nothing,
                new_current, new_reference, zero, range::back);
        }
    };

    template <class Range, class Direction> void operator() (
        Range const & range, Direction const & direction) const
    {
        rime::call_if (range::empty (range, Direction()),
            nothing, step(), range, range, zero, direction);
    }
};

} // namespace detail

template <class HasSize, class HasBack,
    class HasDropConstantN, class HasDropRuntimeN,
    class Range, class Reference>
void check_equal_behaviour (Range const & range, Reference const & reference)
{
    BOOST_MPL_ASSERT ((range::is_range <Reference>));
    BOOST_MPL_ASSERT ((range::is_range <Range>));

    // Start off with first and drop.
    detail::check_equal_behaviour_recursive <HasSize, HasBack> (
        false, range, reference);
    // Start off with chop.
    detail::check_equal_behaviour_recursive <HasSize, HasBack> (
        true, range, reference);

    rime::call_if (HasDropRuntimeN(),
        detail::check_drop_n <HasSize, HasBack, std::size_t>(), detail::nothing,
        range, range::front);
    rime::call_if (rime::and_ (HasDropRuntimeN(), HasBack()),
        detail::check_drop_n <HasSize, HasBack, std::size_t>(), detail::nothing,
        range, range::back);

    rime::call_if (HasDropConstantN(),
        detail::check_drop_n <HasSize, HasBack, rime::size_t <0>>(),
        detail::nothing,
        range, range::front);
    rime::call_if (rime::and_ (HasDropConstantN(), HasBack()),
        detail::check_drop_n <HasSize, HasBack, rime::size_t <0>>(),
        detail::nothing,
        range, range::back);
}

#endif  // RANGE_TEST_CHECK_EQUAL_BEHAVIOUR_HPP_INCLUDED
