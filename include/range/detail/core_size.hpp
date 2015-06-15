/*
Copyright 2011-2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED
#define RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED

#include <type_traits>
#include <stdexcept>

#include <boost/exception/exception.hpp>

#include "utility/overload_order.hpp"

#include "rime/core.hpp"

#include "core_base.hpp"

namespace range {

namespace helper {

    /** \brief
    Return the number of elements in the range.

    To allow the size of a range to be used, this needs to be implemented (by
    providing the member function or by implementing this free function), but
    only for the forward direction.
    For example, by defining it for \c direction::front, it will automatically
    also be defined for \c direction::back.

    \param tag The range tag.
    \param range
        The range itself, qualified (as an rvalue reference if an rvalue).
    \param direction The direction.
    */
    void implement_size (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_size;

        struct size {
        private:
            struct dispatch {
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <1> *) const
                RETURNS (implement_size (typename tag_of <Range>::type(),
                    range, direction));

                // Forward to member if possible.
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <2> *) const
                RETURNS (helper::member_access::size (
                    range, direction));

                // Use direction::make_forward in case "implement_size" is only
                // provided for the forward direction.
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <3> *) const
                RETURNS (implement_size (typename tag_of <Range>::type(),
                    range, direction::make_forward (direction)));

                // Member with make_forward.
                template <class Range, class Direction>
                    auto operator() (
                        Range const & range, Direction const & direction,
                        overload_order <4> *) const
                RETURNS (helper::member_access::size (
                    range, direction::make_forward (direction)));
            };

        public:
            // With direction.
            template <class Range, class Direction, class Enable = typename
                std::enable_if <is_direction <Direction>::value>::type>
            auto operator() (Range const & range, Direction const & direction)
                const
            RETURNS (dispatch() (range, direction, pick_overload()));

            // Without direction: use default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range const & range) const
            RETURNS (dispatch() (
                range, range::default_direction (range), pick_overload()));
        };

    } // namespace implementation

    using implementation::size;

} // namespace callable

/** \brief
Return the number of elements in a range.

Applying \ref drop this number of times results in the range being empty.

\param range
    The range to operate on.
\param direction
    (optional) The direction from which to count.
*/
static const auto size = callable::size();

/**
Exception class that is thrown when it is attempted to convert one range into
another but the size of the source range makes this impossible at run time.
For example, when converting a vector with 3 elements to a tuple with 2
elements.
*/
class size_mismatch
: public virtual std::runtime_error, public virtual boost::exception {
public:
    explicit size_mismatch()
    : std::runtime_error ("Mismatched size of range") {}
};

} // namespace range

#endif  // RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED
