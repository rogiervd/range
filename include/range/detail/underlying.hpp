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

#ifndef RANGE_RANGE_DETAIL_UNDERLYING_HPP_INCLUDED
#define RANGE_RANGE_DETAIL_UNDERLYING_HPP_INCLUDED

#include <boost/mpl/apply.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/pair.hpp>

#include "meta/range.hpp"

#include "core_base.hpp"

namespace range { namespace detail {

namespace callable {

    class get_underlying {
    public:
        template <class Wrapper> typename Wrapper::underlying_type const &
            operator() (Wrapper const & wrapper) const
        { return wrapper.underlying_; }

        template <class Wrapper> typename Wrapper::underlying_type &
            operator() (Wrapper & wrapper) const
        { return wrapper.underlying_; }

        template <class Wrapper> typename Wrapper::underlying_type &&
            operator() (Wrapper && wrapper) const
        { return std::move (wrapper.underlying_); }
    };

} // namespace callable

/**
\return the first argument's "underlying_" attribute of type ::underlying_type.
This differentiates between reference and const reference, and rvalues, and
returns a similarly qualified reference.
*/
static const auto get_underlying = callable::get_underlying();

} // namespace detail

namespace operation {

    /**
    Forward operation to the implementation for the underlying range.
    It works for operations that take just the range, and for operations that
    take a direction and a range.

    \tparam UnderlyingOperation
        The operation on the underlying range.
        This may be unimplemented, in which case this class is also
        unimplemented.
    */
    template <class UnderlyingOperation, class Enable = void>
    struct forward_to_underlying : unimplemented {};

    template <class UnderlyingOperation>
        struct forward_to_underlying <UnderlyingOperation, typename
            boost::enable_if <is_implemented <UnderlyingOperation>>::type>
    {
        template <class Wrapper> auto operator() (Wrapper && wrapper) const
        RETURNS (UnderlyingOperation() (
            range::detail::get_underlying (std::forward <Wrapper> (wrapper))))

        template <class Direction, class Wrapper> auto
            operator() (Direction const & direction, Wrapper && wrapper) const
        RETURNS (UnderlyingOperation() (direction,
            range::detail::get_underlying (std::forward <Wrapper> (wrapper))))
    };

} // namespace operation

} // namespace range

#endif  // RANGE_RANGE_DETAIL_UNDERLYING_HPP_INCLUDED

