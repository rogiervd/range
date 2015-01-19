/*
Copyright 2011-2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED
#define RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED

#include <type_traits>
#include <stdexcept>

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "core_base.hpp"

namespace range {

namespace operation {

    /// Turn Direction into the forward direction and apply "size".
    template <class RangeTag, class Direction, class Range, class Enable = void>
        struct size_by_forward
    : unimplemented {};

    template <class RangeTag, class Direction, class Range>
        struct size_by_forward <RangeTag, Direction, Range, typename
            boost::enable_if <boost::mpl::and_ <
                has <direction::callable::make_forward (Direction)>,
                range_detail::is_implemented_forward <size,
                    RangeTag, Direction, Range>
            >>::type>
    : range_detail::forward_operation <size, RangeTag, Direction, Range> {};

    /** \brief
    Return the number of elements in the range.

    The standard implementation forwards to the <c>.size (Direction)</c>
    member function.
    If that is not available, it will forward to the forward direction, that is,
    <c>size (make_forward (direction), range)</c>, if that is defined.

    This needs to be implemented (by providing the member function or by
    specialising this) for any range, but only for the forward direction.
    For example, by defining it for \c direction::front, it will automatically
    also be defined for \c direction::back.

    \tparam RangeTag The range tag.
    \tparam Direction The decayed direction type.
    \tparam Range The range itself, qualified (as an rvalue reference if an
        rvalue).
    */
    template <class RangeTag, class Direction, class Range, class Enable>
        struct size
    : try_all <member_access::size <Direction, Range>,
        size_by_forward <RangeTag, Direction, Range>>
    {/*
        ... operator() (Direction const & direction, Range && range) const;
    */};

} // namespace operation

namespace apply {
    template <class ... Arguments> struct size;
} // namespace apply

namespace callable {
    struct size : generic <apply::size> {};
} // namespace callable

static const auto size = callable::size();

namespace apply {

    namespace automatic_arguments {

        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct size : operation::unimplemented {};

        template <class Direction, class Range>
            struct size <meta::vector <Direction>, meta::vector<>,
                meta::vector <Range>>
        : operation::size <typename range::tag_of <Range>::type,
            typename std::decay <Direction>::type, Range &&> {};

    } // namespace automatic_arguments

    template <class ... Arguments> struct size
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::size, meta::vector <Arguments ...>>::type {};

} // namespace apply

/**
Exception class that is thrown when it is attempted to convert one range into
another but the size of the source range makes this impossible at run time.
For example, when converting a vector with 3 elements to a tuple with 2
elements.
*/
class size_mismatch : public std::runtime_error {
public:
    explicit size_mismatch()
    : std::runtime_error ("Mismatched size of range") {}
};

} // namespace range

#endif  // RANGE_DETAIL_CORE_SIZE_HPP_INCLUDED
