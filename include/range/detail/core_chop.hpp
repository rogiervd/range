/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_CHOP_HPP_INCLUDED
#define RANGE_DETAIL_CORE_CHOP_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include "meta/vector.hpp"

#include "utility/storage.hpp"

#include "core_base.hpp"
#include "core_first.hpp"
#include "core_drop.hpp"

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
    Implement \a chop by calling \a first and \a drop.
    In effect, this returns
    <c>operation::chopped <...> (first (direction, range),
        drop (direction, std::forward <Range> (range)))</c>

    There are two surprising things about this.

    First, the result type is explicitly computed.
    This way, if \a first returns a reference, then the result also contains
    a reference.

    Second, if the range is an rvalue reference, care must be taken.
    The call to \a first should not pilfer the range, but the call to
    \a drop can.
    */
    template <class RangeTag, class Direction, class Range, class Enable = void>
        class chop_by_first_drop
    : unimplemented {};

    template <class RangeTag, class Direction, class Range>
        class chop_by_first_drop <RangeTag, Direction, Range,
            typename boost::enable_if <boost::mpl::and_ <
                is_implemented <empty <RangeTag, Direction, Range &>>,
                is_implemented <drop <RangeTag, Direction, one_type, Range>>
            >>::type>
    {
    private:
        typedef typename result_of <
            callable::first (Direction, Range &)>::type first_type;
        typedef typename result_of <
            callable::drop (Direction, Range)>::type next_range;
    public:
        typedef chopped <first_type, next_range> result_type;

        result_type operator() (Direction const & direction, Range && range)
            const
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

    /** \brief
    Return a \c chopped of the first element and the rest of the range.

    The standard implementation forwards to the <c>.chop (Direction)</c>
    member function, if that is available.

    If this is not specialised, a default implementation is provided using
    \a first (for an lvalue range) and \a drop (the an rvalue range), if they
    are implemented.
    This happens in \c chop_automatic.
    If for some reason this needs to be switched off, then \c chop_automatic
    can be implemented as deriving from \c unimplemented.

    If \c chop_in_place is implemented, and copy or move are cheap, then this
    can be implemented simply by deriving from \c chop_by_chop_in_place.

    \tparam RangeTag The range tag.
    \tparam Direction The decayed direction type.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Direction, class Range, class Enable>
        struct chop
    : member_access::chop <Direction, Range>
    {/*
        ... operator() (Direction const & direction, Range && range) const;
    */};

    /** \brief
    Return a \c chopped of the first element and the rest of the range, using
    \c first and \c drop if necessary and possible.

    The only reason to specialise this would be disable this and derive it from
    \c unimplemented even when \c chop is implemented.
    */
    template <class RangeTag, class Direction, class Range>
        struct chop_automatic
    : try_all <chop <RangeTag, Direction, Range>,
        chop_by_first_drop <RangeTag, Direction, Range>> {};

    /**
    Helper that implements \c chop using \c chop_in_place.
    It is useful in the case where \c chop_in_place is easy to define, and chop
    harder.
    The process involves a copy/move (for lvalues/rvalues) and a call to
    \c chop_in_place.
    This must be used manually, because it involves a copy or move which might
    be expensive.

    For lvalues, this copies the range, calls chop_in_place.
    The return value of \c chop is formed by the result of chop_in_place, and
    the new range.

    For rvalues, the range is mutated in situ with chop_in_place.
    The return value of \c chop is then formed by the result of chop_in_place,
    and by moving the mutated range out.

    This is only implemented if \c chop_in_place is implemented for the lvalue
    reference type of "Range", and copy or move is implemented.
    */
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct chop_by_chop_in_place
    : unimplemented {};

    template <class RangeTag, class Direction, class Range>
        struct chop_by_chop_in_place <RangeTag, Direction, Range, typename
            boost::enable_if <boost::mpl::and_<
                is_implemented <chop_in_place <RangeTag, Direction, Range &>>,
                std::is_constructible <typename std::decay <Range>::type, Range>
            >>::type>
    {
        // callable::chop_in_place is not yet defined, so do this explicitly.
        typedef chop_in_place <RangeTag, Direction, Range &> chop_in_place_type;
        typedef typename std::result_of <
            chop_in_place_type (Direction, Range &)>::type first_type;
        typedef typename std::decay <Range>::type rest_type;

        typedef chopped <first_type, rest_type> result_type;

        typedef typename std::remove_reference <Range>::type
            range_without_reference;

        // Lvalue reference: copy the range and then apply chop_in_place.
        result_type operator() (Direction const & direction,
            range_without_reference & range) const
        {
            auto new_range = range;
            chop_in_place_type implementation;
            first_type first = implementation (direction, new_range);
            return result_type (static_cast <first_type &&> (first),
                std::move (new_range));
        }

        // Rvalue reference: mutate the range with chop_in_place and then move
        // it.
        result_type operator() (Direction const & direction,
            range_without_reference && range) const
        {
            chop_in_place_type implementation;
            first_type first = implementation (direction, range);
            return result_type (static_cast <first_type &&> (first),
                std::move (range));
        }
    };

} // namespace operation

namespace apply {
    template <class ... Arguments> struct chop;
} // namespace apply

namespace callable {
    struct chop : generic <apply::chop> {};
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

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Increments, class Ranges,
            class Enable = void>
        struct chop : operation::unimplemented {};

        // All arguments filled in: forward to operation::chop_automatic.
        template <class Direction, class Range>
            struct chop <meta::vector <Direction>,
                meta::vector<>, meta::vector <Range>>
        : operation::chop_automatic <
            typename tag_of <Range>::type,
                typename std::decay <Direction>::type, Range &&>
        {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct chop
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::chop, meta::vector <Arguments ...>>::type {};

}} // namespace range::apply

#endif  // RANGE_DETAIL_CORE_CHOP_HPP_INCLUDED
