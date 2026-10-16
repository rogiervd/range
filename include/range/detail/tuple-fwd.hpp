// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define optimised versions of "equal" and "less_lexicographical" for tuples.
*/

#ifndef RANGE_DETAIL_TUPLE_FWD_HPP_INCLUDED
#define RANGE_DETAIL_TUPLE_FWD_HPP_INCLUDED

namespace range {

template <class Tuple> struct tuple_size;

namespace tuple_detail {

    template <std::size_t Index> class extract;
    template <std::size_t Size> struct tuple_view_tag;
    template <std::size_t Begin, std::size_t End, class TupleReference>
    class tuple_view;

}  // namespace tuple_detail

}  // namespace range

#endif  // RANGE_DETAIL_TUPLE_FWD_HPP_INCLUDED
