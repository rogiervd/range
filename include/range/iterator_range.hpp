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

#ifndef RANGE_ITERATOR_RANGE_HPP_INCLUDED
#define RANGE_ITERATOR_RANGE_HPP_INCLUDED

#include <cassert>
#include <iterator>
#include <type_traits>

#include <boost/utility.hpp>

#include "utility/returns.hpp"

#include "core.hpp"

namespace range {

/**
Range that internally maintains two iterators with the same type to denote its
begin and end.
The type of iterator determines which operations are possible.

default_direction (range) just returns front.

empty (range) is always defined.

size (range) is defined only for random-access iterators.
The result type of size (range) is the unsigned version of the distance between
two iterators.

first (front, range) is always defined.
first (back, range) is defined only for bidirectional iterators.

drop (front, range) is always defined.
drop (back, range) is defined only for bidirectional iterators.
drop (front, n, range) and drop (back, n, range) are defined only for
random-access iterators.

The code for operations on this class is worth looking at, because it
embodies the difference between ranges and iterators.
Ranges only shrink, never grow.
In terms of the code, this means that begin() is only ever incremented, not
decremented, and end() is only ever decremented.
Also, ranges' ends are known, so it can be asserted that they are non-empty or
have at least a certain size for first() and drop().
This can be seen in the assertions.
*/
template <class Iterator> class iterator_range {
public:
    iterator_range (Iterator const & begin_, Iterator const & end_)
    : begin_ (begin_), end_ (end_) {}

    iterator_range() : begin_(), end_() {}

    Iterator begin() const { return begin_; }
    Iterator end() const { return end_; }

private:
    Iterator begin_;
    Iterator end_;
};

template <class IteratorTag> struct iterator_range_tag;

template <class Iterator> struct tag_of_bare <iterator_range <Iterator>> {
    typedef iterator_range_tag <
        typename std::iterator_traits <Iterator>::iterator_category> type;
};

namespace operation {

    // empty
    template <class IteratorTag>
        struct empty <iterator_range_tag <IteratorTag>, direction::front>
    {
        template <class Range> bool operator() (
            direction::front, Range && range) const
        { return range.begin() == range.end(); }
    };

    // size
    template <class IteratorTag>
        struct size <iterator_range_tag <IteratorTag>, direction::front,
            typename boost::enable_if <std::is_base_of <
                std::random_access_iterator_tag, IteratorTag>>::type>
    {
        // The distance type is usually signed.
        // However, we know that the size of the range is non-negative,
        // so we turn this into an unsigned type.
        template <class Iterator> struct unsigned_distance {
            typedef decltype (
                std::declval <Iterator>() - std::declval <Iterator>())
                distance_type;
            typedef typename std::make_unsigned <distance_type>::type type;
        };

        template <class Iterator>
            typename unsigned_distance <Iterator>::type operator() (
                direction::front, iterator_range <Iterator> const & range) const
        {
            auto distance = range.end() - range.begin();
            assert (distance >= 0);
            return typename unsigned_distance <Iterator>::type (distance);
        }
    };

    // first (front, range): always defined.
    template <class IteratorTag>
        struct first <iterator_range_tag <IteratorTag>, direction::front>
    {
        template <class Range> auto operator() (
            direction::front, Range && range) const
        -> decltype (*range.begin())
        {
            assert (!::range::empty (range));
            return *range.begin();
        }
    };

    // first (back, range): only defined for bidirectional iterators.
    template <class IteratorTag>
        struct first <iterator_range_tag <IteratorTag>, direction::back,
            typename boost::enable_if <std::is_base_of <
                std::bidirectional_iterator_tag, IteratorTag>>::type>
    {
        template <class Range> auto operator() (
            direction::back, Range && range) const
        -> decltype (*range.begin())
        {
            assert (!::range::empty (range));
            return *boost::prior (range.end());
        }
    };

    // drop (front, one, range): always defined.
    template <class IteratorTag>
        struct drop_one <iterator_range_tag <IteratorTag>, direction::front>
    {
        template <class Increment, class Iterator>
            iterator_range <Iterator> operator() (
                direction::front, Increment const &,
                iterator_range <Iterator> const & range) const
        {
            assert (!::range::empty (range));
            return iterator_range <Iterator> (
                boost::next (range.begin()), range.end());
        }
    };

    // drop (back, one, range): only defined for bidirectional iterators.
    template <class IteratorTag>
        struct drop_one <iterator_range_tag <IteratorTag>, direction::back,
            typename boost::enable_if <std::is_base_of <
                std::bidirectional_iterator_tag, IteratorTag>>::type>
    {
        template <class Increment, class Iterator>
            iterator_range <Iterator> operator() (
                direction::back, Increment const &,
                iterator_range <Iterator> const & range) const
        {
            assert (!::range::empty (range));
            return iterator_range <Iterator> (
                range.begin(), boost::prior (range.end()));
        }
    };

    // drop (front, n, range): only defined for random-access iterators.
    template <class IteratorTag, class Increment>
        struct drop <iterator_range_tag <IteratorTag>, direction::front,
            Increment, typename boost::enable_if <std::is_base_of <
                std::random_access_iterator_tag, IteratorTag>>::type>
    {
        template <class Iterator> iterator_range <Iterator>
            operator() (direction::front, Increment const & increment,
                iterator_range <Iterator> const & range) const
        {
            assert (increment >= 0);
            typedef decltype (::range::size (range)) size_type;
            assert (size_type (increment) <= ::range::size (range));
            return iterator_range <Iterator> (
                range.begin() + increment, range.end());
        }
    };

    // drop (back, n, range): only defined for random-access iterators.
    template <class IteratorTag, class Increment>
        struct drop <iterator_range_tag <IteratorTag>, direction::back,
            Increment, typename boost::enable_if <std::is_base_of <
                std::random_access_iterator_tag, IteratorTag>>::type>
    {
        template <class Iterator> iterator_range <Iterator>
            operator() (direction::back, Increment const & increment,
                iterator_range <Iterator> const & range) const
        {
            assert (increment >= 0);
            typedef decltype (::range::size (range)) size_type;
            assert (size_type (increment) <= ::range::size (range));
            return iterator_range <Iterator> (
                range.begin(), range.end() - increment);
        }
    };

} // namespace operation
} // namespace range

#endif  // RANGE_ITERATOR_RANGE_HPP_INCLUDED

