/*
Copyright 2011-2015, Rogier van Dalen.

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
Range that internally contains two iterators with the same type to denote its
begin and end.
The type of iterator determines which operations are possible.

There is a sharp difference between an iterator_range based on input iterators
on the one hand, and forward iterators or higher on the other hand.
An iterator_range based on input iterators can not be copied.
Therefore, it does not have a copy constructor; first() and drop() only take
an rvalue, as does chop(), which is the most useful operation.
An iterator_range based on forward iterators can be copied without any problems.

<c>default_direction (range)</c> just returns front.

<c>empty (range)</c> is always defined.

<c>size (range)</c> is defined only for random-access iterators.
The result type of size (range) is the unsigned version of the distance between
two iterators.

<c>first (range, front)</c> is defined for all types of iterators; but
<c>first (range, back)</c> is defined only for bidirectional iterators.

<c>drop (range, front)</c> is defined for input and forward iterators.
<c>drop (range, back)</c> is defined only for bidirectional iterators.
<c>drop (range, n, front)</c> and <c>drop (back, n, range)</c> are defined only
for random-access iterators.

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
        typedef Iterator iterator_type;

        typedef typename std::make_unsigned <typename
            std::iterator_traits <Iterator>::difference_type>::type size_type;

        typedef typename std::iterator_traits <Iterator>::value_type value_type;

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
        friend class helper::member_access;
        bool empty (direction::front) const { return begin_ == end_; }

        Iterator begin_;
        Iterator end_;
    };

} // namespace iterator_range_detail

/// \cond DONT_DOCUMENT
// Implementation for forward and higher iterators: copy is possible.
template <class Iterator>
    class iterator_range <Iterator, typename boost::enable_if <
        std::is_base_of <std::forward_iterator_tag, typename
            std::iterator_traits <Iterator>::iterator_category>
    >::type>
: public iterator_range_detail::iterator_range_base <Iterator>
{
    typedef iterator_range_detail::iterator_range_base <Iterator> base;
    typedef typename base::size_type size_type;
    typedef typename std::iterator_traits <Iterator>::iterator_category
        iterator_tag;

    // Types that can't be used, to effectively disable overloads.
    struct private_type;
    struct private_type_2;

    /// direction::back if the iterator is bidirectional, disabled otherwise.
    typedef typename boost::mpl::if_ <
        std::is_base_of <std::bidirectional_iterator_tag, iterator_tag>,
        direction::back, private_type>::type back_if_bidirectional;

    /// direction::front if the iterator is random access, disabled otherwise.
    typedef typename boost::mpl::if_ <
        std::is_base_of <std::random_access_iterator_tag, iterator_tag>,
        direction::front, private_type>::type front_if_random_access;

    /// direction::back if the iterator is random access, disabled otherwise.
    typedef typename boost::mpl::if_ <
        std::is_base_of <std::random_access_iterator_tag, iterator_tag>,
        direction::back, private_type_2>::type back_if_random_access;

    typedef decltype (*std::declval <Iterator const &>()) dereference_type;

public:
    iterator_range() = default;
    iterator_range (iterator_range const &) = default;
    iterator_range (iterator_range &&) = default;

    iterator_range (Iterator const & begin, Iterator const & end)
    : base (begin, end) {}

    iterator_range & operator = (iterator_range const &) = default;
    iterator_range & operator = (iterator_range &&) = default;

private:
    friend class helper::member_access;

    // first.
    dereference_type first (direction::front) const { return *this->begin(); }

    dereference_type first (back_if_bidirectional) const
    { return *boost::prior (this->end()); }

    // size.
    size_type size (front_if_random_access) const {
        auto distance = this->end() - this->begin();
        assert (distance >= 0);
        return typename base::size_type (distance);
    };

    // drop.
    iterator_range drop_one (direction::front) const
    { return iterator_range (boost::next (this->begin()), this->end()); }

    iterator_range drop_one (back_if_bidirectional) const
    { return iterator_range (this->begin(), boost::prior (this->end())); }

    iterator_range drop (size_type increment, front_if_random_access) const {
        assert (increment <= size (front));
        return iterator_range <Iterator> (
            this->begin() + increment, this->end());
    }

    iterator_range drop (size_type increment, back_if_random_access) const {
        assert (increment <= size (front));
        return iterator_range <Iterator> (
            this->begin(), this->end() - increment);
    }

    // chop_in_place.
    /*
    For forward iterators and up, the return type from dereferencing the
    iterator, possibly a reference type, is the appropriate return type.

    This is in contrast to chop_in_place for input iterators, below, which
    returns the value type.
    */
    dereference_type chop_in_place (direction::front)
    { return * this->begin() ++; }

    dereference_type chop_in_place (back_if_bidirectional)
    { return * -- this->end(); }
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

    // Only "chop" is implemented, in namespace iterator_range_operation.

    /*
    Input iterators need special handling.
    Dereferencing an input iterator returns something that can be
    convertible to the value_type, which may apparently become impossible to
    use after it is incremented.
    Therefore, it should be converted to the value type straight away.
    */
    typename std::iterator_traits <Iterator>::value_type
        chop_in_place (direction::front)
    { return * this->begin() ++; }
};
/// \endcond

namespace iterator_range_operation {
    template <class IteratorTag> struct iterator_range_tag {};
} // namespace iterator_range_operation

template <class Iterator>
    struct tag_of_qualified <iterator_range <Iterator>>
{
    typedef iterator_range_operation::iterator_range_tag <typename
        std::iterator_traits <Iterator>::iterator_category> type;
};

/* make_iterator_range */

namespace callable {

    /// Start a new namespace so "using std::begin" does not pollute the
    /// namespace.
    namespace make_iterator_range_detail {

        using std::begin;
        using std::end;

        // From two iterators or a container.
        struct make_iterator_range {
            // IteratorCategory is merely to check that Iterator is an iterator.
            template <class Iterator, class IteratorCategory = typename
                std::iterator_traits <Iterator>::iterator_category>
            iterator_range <Iterator> operator() (
                Iterator const & begin, Iterator const & end) const
            { return iterator_range <Iterator> (begin, end); }

            template <class Container, class Iterator
                = decltype (begin (std::declval <Container &&>()))>
            iterator_range <Iterator> operator() (Container && container)
                const
            {
                return iterator_range <Iterator> (
                    begin (container), end (container));
            }
        };

        struct make_move_iterator_range {
            template <class Container,
                class RawIterator = typename std::decay <
                    decltype (begin (std::declval <Container &>()))>::type,
                class Iterator
                = typename std::conditional <
                    std::is_same <Container,
                        typename std::decay <Container>::type>::value,
                    std::move_iterator <RawIterator>, RawIterator
                >::type>
            iterator_range <Iterator> operator() (Container && container) const
            {
                return iterator_range <Iterator> (
                    Iterator (begin (container)), Iterator (end (container)));
            }
        };

    } // namespace make_iterator_range_detail

    using make_iterator_range_detail::make_iterator_range;
    using make_iterator_range_detail::make_move_iterator_range;

} // namespace callable

/**
Make an iterator_range from begin and end iterators or from a container.
Give either \a begin and \a end, or \a container.

\param begin The begin iterator.
\param end The past-the-end iterator.

\param container
    The container to iterate.
    Begin and end iterators are found with unqualified calls to \c begin
    and \c end, after <c>using std::begin</c> and <c>using std::end</c>.
*/
static constexpr auto make_iterator_range = callable::make_iterator_range();

/**
Make an iterator_range from a container that will be read only once.

\param container
    The container to iterate.
    Begin and end iterators are found with unqualified calls to \c begin
    and \c end, after <c>using std::begin</c> and <c>using std::end</c>.
    If this is an rvalue reference, then the iterator_range will use
    std::move_iterator.
*/
static constexpr auto make_move_iterator_range
    = callable::make_move_iterator_range();

namespace iterator_range_operation {

    template <class IteratorTag, class Range> inline
        auto implement_chop (iterator_range_tag <IteratorTag> const & tag,
            Range && range, direction::front const & direction)
    RETURNS (helper::chop_by_chop_in_place (
        std::forward <Range> (range), direction));

} // namespace iterator_range_operation

} // namespace range

#endif  // RANGE_ITERATOR_RANGE_HPP_INCLUDED
