/*
Copyright 2011-2013, 2015 Rogier van Dalen.

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
Define classes that make it simple to implement range operations for a
container (heavyweight object) that can be converted into a range (a "view" on
the container).
*/

#ifndef RANGE_HEAVYWEIGHT_HPP_INCLUDED
#define RANGE_HEAVYWEIGHT_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include "core.hpp"

namespace range { namespace operation {

/** \brief
Tag for heavyweight objects that can be used as ranges.

Heavyweight ranges are converted to a view, either explicitly, with view() or
make_view() or explicitly, by calling empty(), first(), drop(), et cetera.

To allow a heavyweight container to be converted to a view, give the
heavyweight container a range tag that derives from \ref heavyweight_tag.
first(), for example, will then automatically be implemented for heavyweight
\c h as <c>first (view (h))</c>.

Then, implement the following operations:

\li implement_default_direction, if it is not \ref direction::front.

\li implement_make_view for all combinations of directions that the range
    supports.

\li All relevant operations for the view.
*/
struct heavyweight_tag {};

/*
Operations on heavyweight ranges forward to the same operation applied to
the result of implement_make_view.

It would not work to use "view" here, because it uses "empty" to determine
whether something is actually a view in a specific direction.
This would then cause recursive instantiations, and, in the case of compilers
from 2011 or so, compiler crashes.
*/

using helper::implement_make_view;

// The tag is derived from heavyweight_tag, but it has to be found again to
// call \c implement_make_view.

// empty.
template <class Container, class Direction>
    inline auto implement_empty (heavyweight_tag const &,
        Container && container, Direction const & direction)
RETURNS (range::empty (
    implement_make_view (typename tag_of <Container>::type(), rime::true_,
        std::forward <Container> (container), direction), direction));

template <class Container, class Direction>
    inline auto implement_size (heavyweight_tag const &,
        Container && container, Direction const & direction)
RETURNS (range::size (
    implement_make_view (typename tag_of <Container>::type(), rime::true_,
        std::forward <Container> (container), direction), direction));

template <class Container, class Direction>
    inline auto implement_first (heavyweight_tag const &,
        Container && container, Direction const & direction)
RETURNS (range::first (
    implement_make_view (typename tag_of <Container>::type(), rime::true_,
        std::forward <Container> (container), direction), direction));

// at: once == true.
template <class Container, class Index, class Direction>
    inline auto implement_at (heavyweight_tag const &,
        Container && container,
        Index const & index, Direction const & direction)
RETURNS (range::at (
    implement_make_view (typename tag_of <Container>::type(), rime::true_,
        std::forward <Container> (container), direction), index, direction));

template <class Container, class Increment, class Direction>
    inline auto implement_drop (heavyweight_tag const &,
        Container && container,
        Increment const & increment, Direction const & direction)
RETURNS (range::drop (
    implement_make_view (typename tag_of <Container>::type(), rime::false_,
        std::forward <Container> (container), direction),
    increment, direction));

template <class Container, class Direction>
    inline auto implement_chop (heavyweight_tag const &,
        Container && container, Direction const & direction)
RETURNS (range::chop (
    implement_make_view (typename tag_of <Container>::type(), rime::false_,
        std::forward <Container> (container), direction), direction));

// chop_in_place is not defined: by definition, the (heavyweight) container
// type cannot be returned.

}} // namespace range::operation

#endif  // RANGE_HEAVYWEIGHT_HPP_INCLUDED
