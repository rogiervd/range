/*
Copyright 2013, 2015 Rogier van Dalen.

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
        template <class ... Arguments> struct apply;

        template <class Wrapper2> struct apply <Wrapper2> {
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

        template <class Wrapper> auto operator() (Wrapper && wrapper) const
        -> typename std::result_of <apply <Wrapper> (Wrapper)>::type
        { return apply <Wrapper>() (std::forward <Wrapper> (wrapper)); }
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
    Evaluate to the type that \c get_underlying returns for the qualified type
    \a Wrapper.
    */
    template <class Wrapper> struct underlying
    : std::result_of < ::range::detail::callable::get_underlying (Wrapper &&)>
    {};

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
            range::detail::get_underlying (std::forward <Wrapper> (wrapper))));

        template <class Direction, class Wrapper> auto
            operator() (Direction const & direction, Wrapper && wrapper) const
        RETURNS (UnderlyingOperation() (direction,
            range::detail::get_underlying (std::forward <Wrapper> (wrapper))));
    };

} // namespace operation

} // namespace range

#endif  // RANGE_RANGE_DETAIL_UNDERLYING_HPP_INCLUDED
