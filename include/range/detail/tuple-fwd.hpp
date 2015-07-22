/*
Copyright 2015 Rogier van Dalen.

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

} // namespace tuple_detail

} // namespace range

#endif // RANGE_DETAIL_TUPLE_FWD_HPP_INCLUDED
