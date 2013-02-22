/*
Copyright 2013 Rogier van Dalen.

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
    // \todo Without direction.
    static_assert (range::has::empty <direction::front, Range>::value, "");
    static_assert (range::has::empty <direction::back, Range>::value, "");
    check_equal_value (range::empty (range::front, range),
        range::empty (range::front, reference));
    check_equal_value (range::empty (range::back, range),
        range::empty (range::front, range));
}

// Check size.
template <class HasSize, class Range, class Reference>
    typename boost::enable_if <HasSize>::type
    check_size (Range const & range, Reference const & reference)
{
    static_assert (range::has::size <direction::front, Range>::value, "");
    static_assert (range::has::size <direction::back, Range>::value, "");
    check_equal_value (range::size (range::front, range),
        range::size (range::front, reference));
    check_equal_value (range::size (range::back, range),
        range::size (range::front, range));
}
template <class HasSize, class Range, class Reference>
    typename boost::disable_if <HasSize>::type
    check_size (Range const & range, Reference const & reference, void * = 0)
{ static_assert (!range::has::size <direction::front, Range>::value, ""); }

/* Main loop. */

template <class HasSize, class HasBack, class InitialDirection>
struct check_equal_behaviour_recursive_from;

template <class HasSize, class HasBack, class Range, class Reference>
void check_equal_behaviour_recursive (
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
        range, reference);

    rime::call_if (rime::and_ (!range::empty (range), !range::empty (reference),
            HasBack()),
        check_equal_behaviour_recursive_from <
            HasSize, HasBack, direction::back>(),
        nothing,
        range, reference);
}

// Check first and drop
template <class HasSize, class HasBack, class InitialDirection>
struct check_equal_behaviour_recursive_from {
    template <class Range, class Reference>
        void operator() (Range const & range, Reference const & reference) const
    {
        assert (!range::empty (range));

        InitialDirection d;

        check_equal_value (
            ::range::first (d, range), ::range::first (d, reference));

        check_equal_behaviour_recursive <HasSize, HasBack> (
            ::range::drop (d, range), ::range::drop (d, reference));
    }
};

// Check that drop (n, range) is internally consistent.
/*template <class Direction> struct check_drop_n_runtime {
    template <class Range> void operator() (Range const & range) const {
        Direction d;
        auto current = range::view (range);
        std::size_t size = range::size (d, range);
        for (std::size_t i = 0; i != size; ++ i) {
            auto r = range::drop (d, i, range);
            static_assert (std::is_same <
                decltype (r), decltype (current)>::value, "");
            check_equal_value (range::empty (d, r),
                range::empty (d, current));
            check_equal_value (range::size (d, r),
                range::size (d, current));
            check_equal_value (range::first (d, r),
                range::first (d, current));

            current = range::drop (d, current);
        }

        assert (range::empty (current));
        auto r = range::drop (d, range::size (d, range), range);
        check_equal_value (range::empty (current), range::empty (r));
    }
};
*/

template <class HasSize, class HasBack, class Zero>
    struct check_drop_n
{
    Zero zero;
    check_drop_n() : zero() {}

    struct step {
        Zero zero;
        rime::size_t <1> one;

        step() : zero() {}

        template <class Direction, class Range>
            typename boost::disable_if <rime::equal_constant <
                typename range::result_of::empty <Direction, Range>::type,
                rime::true_type>>::type
            check_first_equal (Direction const & direction,
                Range const & range1, Range const & range2) const
        {
            if (!range::empty (direction, range1))
                check_equal_value (range::first (direction, range1),
                    range::first (direction, range2));
        }
        template <class Direction, class Range>
            typename boost::enable_if <rime::equal_constant <
                typename range::result_of::empty <Direction, Range>::type,
                rime::true_type>>::type
            check_first_equal (Direction const & direction,
                Range const & range1, Range const & range2) const {}

        template <class Direction, class Range1, class Range2>
            void check_equal (Direction const & direction,
                Range1 const & range1, Range2 const & range2) const;
        // Not defined.

        template <class Direction, class Range>
            void check_equal (Direction const & direction,
                Range const & range1, Range const & range2) const
        {
            check_equal_value (range::empty (direction, range1),
                range::empty (direction, range2));
            check_size <HasSize> (range1, range2);
            check_first_equal (direction, range1, range2);
        }

        template <class Direction, class Range1, class Range2, class Increment>
        void operator() (Direction const & direction, Range1 const & current,
            Range2 const & reference, Increment gap) const
        {
            auto new_gap = gap + one;
            auto new_current = range::drop (direction, new_gap, current);
            auto new_reference = range::drop (direction, reference);

            check_equal (direction, new_current, new_reference);

            // Recurse with drop (n + 1).
            rime::call_if (!range::empty (new_current), *this, nothing,
                direction, current, new_reference, new_gap);

            // Recurse starting at this point.
            rime::call_if (!range::empty (new_current), *this, nothing,
                range::front, new_current, new_reference, zero);
            rime::call_if (rime::and_ (!range::empty (new_current), HasBack()),
                *this, nothing,
                range::back, new_current, new_reference, zero);
        }
    };

    template <class Direction, class Range> void operator() (
        Direction const & direction, Range const & range) const
    {
        rime::call_if (range::empty (Direction(), range),
            nothing, step(), direction, range, range, zero);
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

    detail::check_equal_behaviour_recursive <HasSize, HasBack> (
        range, reference);

    rime::call_if (HasDropRuntimeN(),
        detail::check_drop_n <HasSize, HasBack, std::size_t>(), detail::nothing,
        range::front, range);
    rime::call_if (rime::and_ (HasDropRuntimeN(), HasBack()),
        detail::check_drop_n <HasSize, HasBack, std::size_t>(), detail::nothing,
        range::back, range);

    rime::call_if (HasDropConstantN(),
        detail::check_drop_n <HasSize, HasBack, rime::size_t <0>>(),
        detail::nothing,
        range::front, range);
    rime::call_if (rime::and_ (HasDropConstantN(), HasBack()),
        detail::check_drop_n <HasSize, HasBack, rime::size_t <0>>(),
        detail::nothing,
        range::back, range);
}

#endif  // RANGE_TEST_CHECK_EQUAL_BEHAVIOUR_HPP_INCLUDED

