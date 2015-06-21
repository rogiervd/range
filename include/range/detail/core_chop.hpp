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

#include "utility/returns.hpp"
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

namespace helper {

    /** \brief
    Return the first element in the range and the rest of a range.

    If this is not specialised, a default implementation is provided using
    \a first (for an lvalue range) and \a drop (the an rvalue range), if they
    are implemented.
    This happens in \c chop_automatic.
    If for some reason this needs to be switched off, then \c chop_automatic
    can be implemented as deriving from \c unimplemented.

    If \c chop_in_place is implemented, and copy or move are cheap, then this
    can be implemented simply by deriving from \c chop_by_chop_in_place.

    \param tag The range tag.
    \param range The range.
    \param direction The direction.
    */
    void implement_chop (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_chop;

        /** \brief
        Implement "chop" only by calling the direct implementation, not through
        "drop" and "first".

        Compared to chop itself, this requires an additional argument
        \c pick_overload() to be passed in, and no argument is optional.

        \param range
        \param direction
        \param overload_order
        */
        struct chop_direct {
            template <class Range, class Direction>
                auto operator() (
                    Range && range, Direction const & direction,
                    overload_order <1> *) const
            RETURNS (implement_chop (typename tag_of <Range>::type(),
                std::forward <Range> (range), direction));

            // Forward to member if possible.
            template <class Range, class Direction>
                auto operator() (
                    Range && range, Direction const & direction,
                    overload_order <2> *) const
            RETURNS (helper::member_access::chop (
                std::forward <Range> (range), direction));
        };

        struct chop {
        private:
            struct dispatch : chop_direct {
                using chop_direct::operator();

                /**
                Synthesise \a chop by calling \a first and \a drop.
                In effect, this returns
                <c>operation::chopped <...> (first (direction, range),
                    drop (direction, std::forward <Range> (range)))</c>

                There are two surprising things about this.

                First, the result type is explicitly computed.
                This way, if \a first returns a reference, then the result also
                contains a reference.

                Second, if the range is an rvalue reference, care must be taken.
                The call to \a first should not pilfer the range, but the call
                to \a drop can.
                */
                template <class Range, class Direction,
                    class Element = decltype (std::declval <first_direct>() (
                        std::declval <Range const &>(),
                        std::declval <Direction>(), pick_overload())),
                    class Rest = decltype (std::declval <drop_direct>() (
                        std::declval <Range>(), one_type(),
                        std::declval <Direction>(), pick_overload()))>
                    chopped <Element, Rest>
                operator() (Range && range, Direction const & direction,
                    overload_order <3> *) const
                {
                    // Don't do the following in one line: make sure that
                    // first_direct is called first.
                    auto && element = first_direct() (range, direction,
                        pick_overload());
                    // Then forward the range to drop.
                    // The static_cast is in case first returned an rvalue or
                    // rvalue reference.
                    // decltype (element) is then an rvalue reference.
                    return chopped <Element, Rest> (
                        static_cast <Element> (element),
                        drop_direct() (std::forward <Range> (range),
                            one_type(), direction, pick_overload()));
                }
            };

        public:
            // With direction.
            template <class Range, class Direction, class Enable = typename
                std::enable_if <is_range <Range>::value
                    && is_direction <Direction>::value>::type>
            auto operator() (Range && range, Direction const & direction)
                const
            RETURNS (dispatch() (std::forward <Range> (range), direction,
                pick_overload()));

            // Without direction: use default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range && range) const
            RETURNS (dispatch() (
                std::forward <Range> (range), range::default_direction (range),
                pick_overload()));
        };

    } // namespace implementation

    using implementation::chop;
    using implementation::chop_direct;

} // namespace callable

/** \brief
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

} // namespace range

#endif  // RANGE_DETAIL_CORE_CHOP_HPP_INCLUDED
