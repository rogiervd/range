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

#ifndef RANGE_DETAIL_CORE_CHOP_HPP_INCLUDED
#define RANGE_DETAIL_CORE_CHOP_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include "meta/vector.hpp"

#include "utility/storage.hpp"

#include "core_base.hpp"

namespace range {

/**
Result type for \a chop, which contains the first element of a range, and the
rest of the range.
This class is extremely simple.
It is assignable if \a First and \a Rest are assignable.
This is also the case if \a First is a reference type, a function type, or an
array.

\tparam First
    The type of the first element of a range.
    This will often be the exact return type of \c first().
\tparam Rest
    The type of the rest of the range.
    This will often be an unqualified value.
*/
template <class First, class Rest> class chopped {
public:
    typedef First first_type;
    typedef Rest rest_type;

private:
    typename utility::storage::store <First>::type first_;
    Rest rest_;

public:
    chopped (First && first, Rest && rest)
    : first_ (std::forward <First> (first)), rest_ (std::forward <Rest> (rest))
    {}

    chopped (chopped const & that)
    : first_ (that.first()), rest_ (that.rest()) {}

    chopped (chopped && that)
    : first_ (that.forward_first()), rest_ (that.forward_rest()) {}

    chopped & operator = (chopped const & that) {
        first_ = that.first_;
        rest_ = that.rest_;
        return *this;
    }

    chopped & operator = (chopped && that) {
        first_ = that.forward_first();
        rest_ = that.forward_rest();
        return *this;
    }

    /**
    \return The first element of the range, as a const reference (depending on
    the type of \a First).
    */
    typename utility::storage::get <First, chopped const &>::type first() const
    { return first_; }

    /**
    \return The first element of the range, as an rvalue reference (depending on
    the type of \a First).
    The lifetime of the result is restricted to the lifetime of this object.
    */
    typename utility::storage::get <First, chopped &&>::type move_first() {
        utility::storage::get <First, chopped &&> extract;
        return extract (first_);
    }

    /**
    \return The first element of the range, as an object of type \a First.
    The object is moved into the result.
    The lifetime of the result is not restricted to the lifetime of this object.
    */
    First forward_first() { return move_first(); }

    /**
    \return The rest of the range, as a const reference.
    */
    Rest const & rest() const { return rest_; }
    /**
    \return The rest of the range, as a reference.
    */
    Rest & rest() { return rest_; }

    /**
    \return The rest of the range, as an rvalue reference.
    The lifetime of the result is restricted to the lifetime of this object.
    */
    Rest && move_rest() { return std::forward <Rest> (rest_); }

    /**
    \return The rest of the range, as an object of type \a Rest.
    The object is moved into the result.
    The lifetime of the result is not restricted to the lifetime of this object.
    */
    Rest forward_rest() { return std::forward <Rest> (rest_); }
};

namespace operation {

    /**
    Return a chopped of the first element and the rest of the range.

    If this is not specialised, a default implementation is provided using
    \a first and \a drop, if they are implemented.

    \internal The default implementation is provided in namespace apply; see
    below.
    */
    template <class RangeTag, class Direction, class Enable>
        struct chop
    : unimplemented {/*
        template <class Range>
            ... operator() (Direction const & direction, Range && range) const;
    */};

    /**
    Return the first element of the range, and replace the range in memory by
    the range without the first element.

    The range must be homogeneous.

    If this is not specialised, a default implementation is provided if either
    \a first and \a drop are implemented, or if \a chop is implemented.

    Usually, it is necessary to make sure that only non-const reference
    ranges are picked up.
    When specialising this struct, the range tag must then differentiate between
    non-const references and other qualificatiers.

    \internal The default implementation is provided in namespace apply; see
    below.
    */
    template <class RangeTag, class Direction, class Enable>
        struct chop_in_place
    : unimplemented {/*
        template <class Range>
            ... operator() (Direction const & direction, Range && range) const;
    */};

} // namespace operation

namespace apply {

    template <class ... Arguments> struct chop;
    template <class ... Arguments> struct chop_in_place;

} // namespace apply

namespace callable {

    struct chop : generic <apply::chop> {};
    struct chop_in_place : generic <apply::chop_in_place> {};

} // namespace callable

/**
Return the first element and the range without that first element, as an object
of type \a chopped.
This is an operation that ranges that cannot be copied will often implement.

\param direction
    (optional) The direction from which the first element is taken.
\param range
    The range to operate on.
    This is often required to be an rvalue.
*/
static const auto chop = callable::chop();

/**
Return the first element of the range, and remove this element from the range
itself.

\param direction
    (optional) The direction from which the first element is taken.
\param range
    The range to operate on.
    It must be homogeneous, so that without the first element it has the same
    type.
    This is normally required to be a reference.
*/
static const auto chop_in_place = callable::chop_in_place();

namespace apply {

    namespace detail {

        /*
        Synthesising implementations for chop and chop_in_place.
        This is in case they are not explicitly defined.
        */

        template <class Direction, class Range> struct chop_can_be_emulated
        : boost::mpl::and_ <
            // Even if Range is an rvalue, it will be used as an lvalue
            // reference first, and then forwarded to "drop".
            has <callable::first (Direction, Range &)>,
            has <callable::drop (Direction, operation::one_type, Range)>>
        {};

        template <class Direction, class Range, class Enable = void>
            class chop_synthesise
        : operation::unimplemented {};

        /* chop. */

        /**
        Implement \a chop by calling \a first and \a drop.
        In effect, this returns
        <c>operation::chopped <...> (first (direction, range),
            drop (direction, std::forward <Range> (range)))</c>

        This is defined here and not in namespace \c operation for two reasons.
        First, here, the actual range type is known, not just the range tag, so
        it is possible to determine whether this is implementable (see below).
        Second, \a first and \a drop are automatically implemented in terms of
        \a chop if it is implemented, so to prevent recursive automatic
        implementations, that happens in namespace \a operation and this here.

        There are two surprising things about this.

        First, the result type is explicitly computed.
        This way, if \a first returns a reference, then the result also contains
        a reference.

        Second, if the range is an rvalue reference, care must be taken.
        The call to \a first should not pilfer the range, but the call to
        \a drop can.
        */
        template <class Direction, class Range>
            class chop_synthesise <Direction, Range, typename boost::enable_if <
                chop_can_be_emulated <Direction, Range>
            >::type>
        {
        private:
            typedef typename result_of <
                callable::first (Direction, Range &)>::type first_type;
            typedef typename result_of <
                callable::drop (Direction, Range)>::type next_range;
        public:
            typedef chopped <first_type, next_range> result_type;
            result_type operator() (
                Direction const & direction, Range && range) const
            {
                // Don't do the following in one line: make sure that
                // range::first is called first.
                auto && first = range::first (direction, range);
                // Then forward the range to range::drop.
                // The static_cast is in case range::first returned an rvalue
                // or rvalue reference.
                // decltype (first) is then an rvalue reference.
                return result_type (static_cast <decltype (first)> (first),
                    range::drop (direction, std::forward <Range> (range)));
            }
        };

        /* chop_in_place. */

        /*
        Two strategies for synthesising the implementation can be used:
        1. Use first() and drop().
        2. Use chop().
        Both are quite straightforward.
        */

        template <class Direction, class Range, class Enable = void>
            struct chop_in_place_synthesise_2
        : operation::unimplemented {};

        template <class Direction, class Range, class Enable = void>
            struct chop_in_place_synthesise
        : chop_in_place_synthesise_2 <Direction, Range> {};

        // Strategy 1: synthesise an implementation with first() and drop().

        template <class Direction, class Range> struct is_drop_homogeneous
        : std::is_same <Range, typename
            result_of <callable::drop (Direction, Range)>::type> {};

        template <class Direction, class Range>
            class chop_in_place_synthesise <Direction, Range &, typename
                boost::enable_if <boost::mpl::and_ <
                    is_drop_homogeneous <Direction, Range>,
                    chop_can_be_emulated <Direction, Range>
                >>::type
            >
        {
        public:
            typename result_of <range::callable::first (
                Direction const &, Range &)>::type
            operator() (Direction const & direction, Range & range) const
            {
                auto && first = range::first (direction, range);
                range = range::drop (direction, std::move (range));
                return static_cast <decltype (first)> (first);
            }
        };

        // Strategy 2: synthesise an implementation with chop().

        template <class Direction, class Range> struct is_chop_homogeneous
        : std::is_same <Range, typename
            result_of <callable::chop (Direction, Range)>::type::rest_type> {};

        template <class Direction, class Range>
            struct chop_in_place_synthesise_2 <Direction, Range &, typename
            boost::enable_if <boost::mpl::and_ <
                has <callable::chop (Direction, /* note: rvalue */ Range)>,
                is_chop_homogeneous <Direction, Range>
            >>::type>
        {
            typename result_of <callable::chop (Direction, Range)
                >::type::first_type
            operator() (Direction const & direction, Range & range) const
            {
                auto result = range::chop (direction, std::move (range));
                range = result.move_rest();
                return result.forward_first();
            }
        };

    } // namespace detail

    namespace automatic_arguments {

        template <class Directions, class Increments, class Ranges,
            class Enable = void>
        struct chop : operation::unimplemented {};

        // All arguments filled in: forward to operation::chop, or to
        // chop_synthesis.
        template <class Direction, class Range>
            struct chop <meta::vector <Direction>,
                meta::vector<>, meta::vector <Range>>
        : boost::mpl::if_ <operation::is_implemented <
                operation::chop <typename tag_of <Range>::type, Direction>>,
            operation::chop <typename tag_of <Range>::type, Direction>,
            apply::detail::chop_synthesise <Direction, Range>
        >::type {};

        /* chop_in_place. */

        template <class Directions, class Others, class Ranges,
            class Enable = void>
        struct chop_in_place : operation::unimplemented {};

        template <class Direction, class Range>
            struct chop_in_place <
                meta::vector <Direction>, meta::vector<>, meta::vector <Range>>
        : boost::mpl::if_ <operation::is_implemented <
                operation::chop_in_place <typename
                    tag_of <Range>::type, Direction>>,
            operation::chop_in_place <typename tag_of <Range>::type, Direction>,
            apply::detail::chop_in_place_synthesise <Direction, Range>
        >::type {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct chop
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::chop, meta::vector <Arguments ...>>::type {};

    template <class ... Arguments> struct chop_in_place
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::chop_in_place, meta::vector <Arguments ...>>::type
    {};

}} // namespace range::apply

#endif  // RANGE_DETAIL_CORE_CHOP_HPP_INCLUDED
