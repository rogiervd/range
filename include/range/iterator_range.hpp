/*
Copyright 2011-2014 Rogier van Dalen.

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
#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>

#include "utility/returns.hpp"

#include "core.hpp"

namespace range {

/**
Range that internally two iterators with the same type to denote its begin and
end.
The type of iterator determines which operations are possible.

There is a sharp difference between an iterator_range based on input iterators
on the one hand, and forward iterators or higher on the other hand.
An iterator_range based on input iterators can not be copied.
Therefore, it does not have a copy constructor; first() and drop() only take
an rvalue, as does chop(), which is the most useful operation.
An iterator_range based on forward iterators can be copied without any problems.

default_direction (range) just returns front.

empty (range) is always defined.

size (range) is defined only for random-access iterators.
The result type of size (range) is the unsigned version of the distance between
two iterators.

first (front, range) is defined for all types of iterators; but
first (back, range) is defined only for bidirectional iterators.

drop (front, range) is defined for input and forward iterators.
drop (back, range) is defined only for bidirectional iterators.
drop (front, n, range) and drop (back, n, range) are defined only for
random-access iterators.

The code for operations on this class is worth looking at, because it
embodies the difference between ranges and iterators.
Ranges only shrink, never grow.
In terms of the code, this means that \c begin is only ever incremented, not
decremented, and \c end is only ever decremented.
Also, ranges' ends are known, so it can be asserted that they are non-empty or
have at least a certain size for \c first() and \c drop().
This can be seen in the assertions.

\tparam Iterator
    The underlying iterator type.
    This must be an input iterator, or a forward iterator or higher.
    The things called "output iterators" are not supported.
*/

template <class Iterator, class Enable = void>
    class iterator_range;

namespace iterator_range_detail {

    /**
    Base class for both iterator_range's that can be copied, and ones that
    can't.
    */
    template <class Iterator> class iterator_range_base {
        typedef typename std::iterator_traits <Iterator>::iterator_category
            iterator_tag;
        static_assert (
            std::is_base_of <std::input_iterator_tag, iterator_tag>::value ||
            std::is_base_of <std::forward_iterator_tag, iterator_tag>::value,
            "The iterator must be an input iterator or a forward iterator.");
    public:
        iterator_range_base (Iterator const & begin, Iterator const & end)
        : begin_ (begin), end_ (end) {}

        iterator_range_base() = default;

        iterator_range_base (iterator_range_base const &) = default;

        // Use any move or copy constructor available, to make sure that moving
        // is defined.
        iterator_range_base (iterator_range_base && that)
        : begin_ (std::move (that.begin())), end_ (std::move (that.end())) {}

        iterator_range_base & operator = (iterator_range_base const &)
            = default;

        iterator_range_base & operator = (iterator_range_base && that) {
            this->begin_ = std::move (that.begin());
            this->end_ = std::move (that.end());
            return *this;
        }

        Iterator const & begin() const { return begin_; }
        Iterator & begin() { return begin_; }

        Iterator const & end() const { return end_; }
        Iterator & end() { return end_; }

    private:
        Iterator begin_;
        Iterator end_;
    };

} // namespace iterator_range_detail

// Implementation for forward and higher iterators: copy is possible.
template <class Iterator>
    class iterator_range <Iterator, typename boost::enable_if <
        std::is_base_of <std::forward_iterator_tag, typename
            std::iterator_traits <Iterator>::iterator_category>
    >::type>
: public iterator_range_detail::iterator_range_base <Iterator>
{
    typedef iterator_range_detail::iterator_range_base <Iterator> base;
public:
    iterator_range() = default;
    iterator_range (iterator_range const &) = default;
    iterator_range (iterator_range &&) = default;

    iterator_range (Iterator const & begin, Iterator const & end)
    : base (begin, end) {}

    iterator_range & operator = (iterator_range const &) = default;
    iterator_range & operator = (iterator_range &&) = default;
};

// Implementation for input iterators: copy is not possible; move is.
template <class Iterator>
    class iterator_range <Iterator, typename boost::enable_if <
        boost::mpl::and_ <
            std::is_base_of <std::input_iterator_tag, typename
                std::iterator_traits <Iterator>::iterator_category>,
            boost::mpl::not_ <
                std::is_base_of <std::forward_iterator_tag, typename
                    std::iterator_traits <Iterator>::iterator_category>>
    >>::type>
: public iterator_range_detail::iterator_range_base <Iterator>
{
    typedef iterator_range_detail::iterator_range_base <Iterator> base;
public:
    iterator_range() = default;
    // For input iterators, delete the copy constructor and provide only a
    // move constructor.
    iterator_range (iterator_range const &) = delete;
    iterator_range (iterator_range &&) = default;

    iterator_range (Iterator const & begin, Iterator const & end)
    : base (begin, end) {}

    iterator_range & operator = (iterator_range const &) = delete;
    iterator_range & operator = (iterator_range &&) = default;
};

template <class IteratorTag, qualification qualifier>
    struct iterator_range_tag;

template <class Iterator, qualification qualifier>
    struct tag_of_qualified <iterator_range <Iterator>, qualifier>
{
    typedef iterator_range_tag <typename
        std::iterator_traits <Iterator>::iterator_category, qualifier> type;
};

namespace operation {

    // empty
    template <class IteratorTag, qualification qualifier>
        struct empty <iterator_range_tag <IteratorTag, qualifier>,
            direction::front>
    {
        template <class Range> bool operator() (
            direction::front, Range && range) const
        { return range.begin() == range.end(); }
    };

    // size
    template <class IteratorTag, qualification qualifier>
        struct size <iterator_range_tag <IteratorTag, qualifier>,
            direction::front,
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

    /*
    first (front, range).
    This is defined explicitly for forward iterators.
    (Explain why that makes sense and chop is defined for input iterators.)
    */
    template <class IteratorTag, qualification qualifier>
        struct first <iterator_range_tag <IteratorTag, qualifier>,
            direction::front,
            typename boost::enable_if <std::is_base_of <
                std::forward_iterator_tag, IteratorTag>>::type>
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
    template <class IteratorTag, qualification qualifier>
        struct first <iterator_range_tag <IteratorTag, qualifier>,
            direction::back,
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

    // drop (front, one, range): defined for forward iterators.
    template <class IteratorTag, qualification qualifier>
        struct drop_one <iterator_range_tag <IteratorTag, qualifier>,
            direction::front,
            typename boost::enable_if <std::is_base_of <
                std::forward_iterator_tag, IteratorTag>>::type>
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
    template <class IteratorTag, qualification qualifier>
        struct drop_one <iterator_range_tag <IteratorTag, qualifier>,
            direction::back,
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
    template <class IteratorTag, class Increment, qualification qualifier>
        struct drop <iterator_range_tag <IteratorTag, qualifier>,
            direction::front, Increment,
            typename boost::enable_if <std::is_base_of <
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
    template <class IteratorTag, class Increment, qualification qualifier>
        struct drop <iterator_range_tag <IteratorTag, qualifier>,
            direction::back, Increment,
            typename boost::enable_if <std::is_base_of <
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

    namespace iterator_range_detail {

        template <class Iterator> struct chop_element
        : boost::mpl::if_ <
            std::is_base_of <std::forward_iterator_tag,
                typename std::iterator_traits <Iterator>::iterator_category>,
            decltype (* std::declval <Iterator>() ++),
            typename std::iterator_traits <Iterator>::value_type> {};

    } // namespace iterator_range_detail

    /*
    Input iterators need special handling.
    Dereferencing an input iterator returns something that can be convertible to
    the value_type, which may apparently become impossible to use after it is
    incremented.
    Therefore, it should be converted to the value type straight away.

    For forward iterators and up, this should not be a problem.
    The default implementation of \a chop then works.

    This is only defined for rvalue ranges.
    */
    template <class IteratorTag>
        struct chop <iterator_range_tag <IteratorTag, temporary>,
            direction::front, typename boost::disable_if <std::is_base_of <
                std::forward_iterator_tag, IteratorTag>>::type>
    {
        template <class Iterator> struct result {
            typedef chopped <
                typename std::iterator_traits <Iterator>::value_type,
                iterator_range <Iterator>> type;
        };

        // rvalue range only.
        template <class Iterator>
            typename result <Iterator>::type
            operator() (direction::front, iterator_range <Iterator> && range)
            const
        {
            typedef typename std::iterator_traits <Iterator>::value_type
                value_type;
            value_type first = *range.begin() ++;
            return typename result <Iterator>::type (
                static_cast <value_type &&> (first), std::move (range));
        }
    };

    /*
    The equivalent of chop_in_place is supplied automatically, but this is a
    more direct implementation.
    (Note that no performance improvements have been observed.)
    */
    template <class IteratorTag>
        struct chop_in_place <iterator_range_tag <IteratorTag, reference>,
            direction::front>
    {
        template <class Iterator>
            typename iterator_range_detail::chop_element <Iterator>::type
            operator() (direction::front const &,
                iterator_range <Iterator> & range) const
        { return * range.begin() ++; }
    };

}} // namespace range::operation

#endif  // RANGE_ITERATOR_RANGE_HPP_INCLUDED
