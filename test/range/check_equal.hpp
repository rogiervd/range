/*
Copyright 2011, 2012 Rogier van Dalen.

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

#ifndef RIME_CHECK_EQUAL_HPP_INCLUDED
#define RIME_CHECK_EQUAL_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "rime/core.hpp"

/*** Basic checks ***/

// Same type.
template <class A>
    void rime_check_equal_implementation (A const & a, A const & b)
{
    BOOST_CHECK_EQUAL (a, b);
}

/*
Different types: must be two constants with the same value type and the
same value.
*/
template <class A, class B>
    void rime_check_equal_implementation (A const &, B const &)
{
    BOOST_MPL_ASSERT ((std::is_same <
            typename rime::value <A>::type,
            typename rime::value <B>::type
        >));
    BOOST_MPL_ASSERT ((rime::is_constant <A>));
    BOOST_MPL_ASSERT ((rime::is_constant <B>));
    BOOST_MPL_ASSERT ((rime::equal_constant <A, B>));
}

/**
Check whether a and b are either of the same type and the same value,
or constants with the same value type and same constant value.
*/
#define RIME_CHECK_EQUAL(a, b) \
    BOOST_CHECK_EQUAL (a, b); \
    rime_check_equal_implementation (a, b)


#endif  // RIME_CHECK_EQUAL_HPP_INCLUDED

