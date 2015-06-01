/*
Copyright 2013 Rogier van Dalen.

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

