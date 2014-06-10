/*
Copyright 2011, 2012, 2013 Rogier van Dalen.

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

operation::default_direction must be defined if it is not front.
operation::make_view must be defined for all combinations of directions that the
range supports.
operation::helper::call_with_last may be particularly helpful in this regard.
*/
template <class Container> struct heavyweight_tag;

template <class Container> struct tag_of_bare <heavyweight_tag <Container>>
{ typedef heavyweight_tag <Container> type; };

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
    template <class Container, class Directions>
        struct view <heavyweight_tag <Container>, Directions>
    : make_view <heavyweight_tag <Container>, Directions> {};

    /*
    Operations on heavyweight ranges forward to the same operation applied to
    the result of view (direction, range).
    The implementation below is repetitive, but a quick go at a less repetitive
    version was longer than this.
    */

    // empty
    template <class Container, class Direction>
        struct empty <heavyweight_tag <Container>, Direction,
            typename boost::enable_if <has <callable::empty (
                Direction, callable::view (Direction, Container))>>::type>
    {
        template <class CVContainer> auto
        operator() (Direction const & direction, CVContainer && container) const
        RETURNS (::range::empty (direction, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

    // size
    template <class Container, class Direction>
        struct size <heavyweight_tag <Container>, Direction,
            typename boost::enable_if <has <callable::size (
                Direction, callable::view (Direction, Container))>>::type>
    {
        template <class CVContainer> auto
        operator() (Direction const & direction, CVContainer && container) const
        RETURNS (::range::size (direction, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

    // first
    template <class Container, class Direction>
        struct first <heavyweight_tag <Container>, Direction,
            typename boost::enable_if <has <callable::first (
                Direction, callable::view (Direction, Container))>>::type>
    {
        template <class CVContainer> auto
        operator() (Direction const & direction, CVContainer && container) const
        RETURNS (::range::first (direction, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

    // drop
    template <class Container, class Direction, class Increment>
        struct drop <heavyweight_tag <Container>, Direction, Increment,
            typename boost::enable_if <has <callable::drop (
                Direction, Increment, callable::view (Direction, Container))>
            >::type>
    {
        template <class CVContainer> auto
        operator() (Direction const & direction, Increment const & increment,
            CVContainer && container) const
        RETURNS (::range::drop (direction, increment, ::range::view (
            direction, std::forward <CVContainer> (container))));
    };

} // namespace operation
} // namespace range

#endif  // RANGE_HEAVYWEIGHT_HPP_INCLUDED

