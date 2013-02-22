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

#ifndef RANGE_RANGE_DETAIL_META_SPLIT_HPP_INCLUDED
#define RANGE_RANGE_DETAIL_META_SPLIT_HPP_INCLUDED

#include <boost/mpl/apply.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/pair.hpp>

#include "meta/range.hpp"

namespace range { namespace detail {

namespace split_detail {

    template <class Direction> struct reverse;
    template <> struct reverse <meta::front> { typedef meta::back type; };
    template <> struct reverse <meta::back> { typedef meta::front type; };

    template <class Direction, class Predicate, class True, class Rest>
        struct split;

    template <class Direction, class Predicate, class True, class Rest>
        struct split_not_empty
    {
        typedef typename reverse <Direction>::type reverse;
        typedef typename meta::first <Direction, Rest>::type first;

        typedef split <Direction, Predicate,
            typename meta::push <reverse, first, True>::type,
            typename meta::drop <Direction, Rest>::type> recurse;
        typedef typename boost::mpl::eval_if <
            typename boost::mpl::apply <Predicate, first>::type,
            recurse, boost::mpl::pair <True, Rest>>::type type;
    };

    template <class Direction, class Predicate, class True, class Rest>
        struct split
    : boost::mpl::eval_if <meta::empty <Direction, Rest>,
        boost::mpl::pair <True, Rest>,
        split_not_empty <Direction, Predicate, True, Rest>> {};

} // namespace split_detail

/**
Split range into the first part as long as Predicate is true, and the rest.
Only front and back can be used as directions.
*/
template <class Direction, class Predicate, class Range>
    struct split
: split_detail::split <Direction, Predicate, meta::vector<>, Range> {};

}} // namespace range::detail

#endif  // RANGE_RANGE_DETAIL_META_SPLIT_HPP_INCLUDED

