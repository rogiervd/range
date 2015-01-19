/*
Copyright 2011-2013, 2015 Rogier van Dalen.

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

/** \file
Define classes that make it simple to implement range operations for a
container (heavyweight object) that can be converted into a range (a "view" on
the container).
*/

#ifndef RANGE_HEAVYWEIGHT_HPP_INCLUDED
#define RANGE_HEAVYWEIGHT_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include "core.hpp"

namespace range {

/**
Tag for heavyweight objects that can be used as ranges.
Heavyweight ranges are converted to a view, either explicitly, with view() or
make_view() or explicitly, by calling empty(), first(), drop(), et cetera.
The former is a matter of implementing operation::make_view.
The latter is taken care of by using this tag.

To allow a heavy-weight container to be converted to a view, use code like the
following:
\code
template <class ... TemplateArguments>
    struct tag_of_qualified <your_container <TemplateArguments ...>>
{ typedef heavyweight_tag <your_container <TemplateArguments ...>> type; };
\endcode

Then, implement the following operations for
\c heavyweight_tag <your_container <TemplateArguments ...>> :

\li operation::default_direction must be defined if it is not front.

\li operation::make_view must be defined for all combinations of directions that
    the range supports.
    operation::helper::call_with_last may be particularly helpful.

\li All relevant operations for the range.
*/
template <class Container> struct heavyweight_tag;

namespace operation {
    /*
    Short-circuit "view" to "make_view" for heavyweight_tag, to prevent
    circular template instantiations with invalid directions.
    "view" normally provides a default implementation even if make_view is not
    defined, by checking if empty is defined for all elements of Directions.
    The implementation of empty for heavyweight_tag first checks whether
    empty (direction, view (direction, range)) is defined.
    Thus, view would need to be instantiated inside its instantiation.
    */
    template <class Container, class Directions, class Range>
        struct view <heavyweight_tag <Container>, Directions, Range>
    : make_view <false, heavyweight_tag <Container>, Directions, Range> {};
    template <class Container, class Directions, class Range>
        struct view_once <heavyweight_tag <Container>, Directions, Range>
    : make_view <true, heavyweight_tag <Container>, Directions, Range> {};

    /*
    Operations on heavyweight ranges forward to the same operation applied to
    the result of view (direction, range).
    The implementation below is repetitive, but a quick go at a less repetitive
    version was longer than this.
    */

    // empty.
    template <class Container, class Direction, class CVContainer>
        struct empty <heavyweight_tag <Container>, Direction, CVContainer,
            typename boost::enable_if <has <callable::empty (
                Direction, callable::view (Direction, CVContainer))>>::type>
    {
        auto operator() (Direction const & direction, CVContainer && container)
            const
        RETURNS (::range::empty (direction, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

    // size.
    template <class Container, class Direction, class CVContainer>
        struct size <heavyweight_tag <Container>, Direction, CVContainer,
            typename boost::enable_if <has <callable::size (
                Direction, callable::view (Direction, Container))>>::type>
    {
        auto operator() (Direction const & direction, CVContainer && container)
            const
        RETURNS (::range::size (direction, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

    // first.
    template <class Container, class Direction, class CVContainer>
        struct first <heavyweight_tag <Container>, Direction, CVContainer,
            typename boost::enable_if <has <callable::first (
                Direction, callable::view_once (Direction, Container))>>::type>
    {
        auto operator() (Direction const & direction, CVContainer && container)
            const
        // Use view_once in case the container is a temporary.
        RETURNS (::range::first (direction, ::range::view_once (
            direction, std::forward <CVContainer> (container))));
    };

    // at.
    template <class Container, class Direction, class Index, class CVContainer>
        struct at <heavyweight_tag <Container>, Direction, Index, CVContainer,
            typename boost::enable_if <has <callable::at (Direction, Index,
                callable::view_once (Direction, Container))>>::type>
    {
        auto operator() (Direction const & direction, Index const & index,
            CVContainer && container) const
        RETURNS (::range::at (direction, index, ::range::view_once (
            direction, std::forward <CVContainer> (container))));
    };

    // drop.
    template <class Container, class Direction, class Increment,
            class CVContainer>
        struct drop <heavyweight_tag <Container>, Direction, Increment,
            CVContainer,
            typename boost::enable_if <has <callable::drop (
                Direction, Increment, callable::view (Direction, Container))>
            >::type>
    {
        auto operator() (Direction const & direction,
            Increment const & increment, CVContainer && container) const
        RETURNS (::range::drop (direction, increment, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

    // chop.
    template <class Container, class Direction, class CVContainer>
        struct chop <heavyweight_tag <Container>, Direction, CVContainer,
            typename boost::enable_if <has <callable::chop (
                Direction, callable::view (Direction, Container))>
            >::type>
    {
        auto operator() (Direction const & direction, CVContainer && container)
            const
        RETURNS (::range::chop (direction, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

    // chop_in_place is not defined: by definition, the (heavyweight) container
    // is not returned.

} // namespace operation
} // namespace range

#endif  // RANGE_HEAVYWEIGHT_HPP_INCLUDED
