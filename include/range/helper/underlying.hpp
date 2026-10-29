// SPDX-FileCopyrightText: Copyright 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_RANGE_DETAIL_UNDERLYING_HPP_INCLUDED
#define RANGE_RANGE_DETAIL_UNDERLYING_HPP_INCLUDED

#include <type_traits>

#include "utility/returns.hpp"
#include "utility/storage.hpp"

namespace range { namespace helper {

    namespace callable {

        template <class Wrapper> class get_underlying
        {
        public:
            template <
                class Wrapper2 = Wrapper,
                class Get = utility::storage::get<
                    typename std::decay<Wrapper2>::type::underlying_type,
                    Wrapper &&>>
            typename Get::type operator()(
                typename std::remove_reference<Wrapper>::type & wrapper) const
            {
                return Get()(wrapper.underlying_);
            }
        };

    }  // namespace callable

    /** \brief
    Return the first argument's "underlying_" attribute of type
    \c Wrapper::underlying_type.

    This differentiates between reference and const reference, and rvalues, and
    returns a similarly qualified reference.
    The template argument \a Wrapper must be given, and must have the correct
    qualification (except ).
    This works exactly the same as the template parameter to std::forward.
    99% of the time, this function would be passed
    <c>std::forward\<...>(...)</c> so this makes that unnecessary.

    This allows \c underlying_ and \c ::underlying_type to be private:
    \code
    template <class Wrapper> friend class helper::callable::get_underlying;
    \endcode

    \tparam Wrapper The qualified type of the parameter is passed in.
    */
    template <class Wrapper> inline auto get_underlying(
        typename std::remove_reference<Wrapper>::type & wrapper)
        RETURNS(callable::get_underlying<Wrapper>()(wrapper));

    template <class Wrapper> struct underlying_type
    : utility::storage::get<
          typename std::decay<Wrapper>::type::underlying_type, Wrapper &&> {};

}}  // namespace range::helper

#endif  // RANGE_RANGE_DETAIL_UNDERLYING_HPP_INCLUDED
