/*
Copyright 2007, 2009, 2014 Rogier van Dalen.

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

#ifndef RANGE_SHARED_LIST_HPP_INCLUDED
#define RANGE_SHARED_LIST_HPP_INCLUDED

#include <utility>

#include "utility/shared.hpp"
#include "utility/small_ptr.hpp"
#include "utility/disable_if_same.hpp"

#include "rime/assert.hpp"
#include "rime/always.hpp"

#include "range/core.hpp"
#include "range/heavyweight.hpp"
#include "range/fold.hpp"

namespace range { namespace detail {
    template <class Type, class Allocator> class shared_node;
    template <class Type, class Allocator> struct shared_node_allocator;
}} // namespace range::detail

namespace utility { namespace pointer_policy {

    template <class Type, class Allocator>
        struct move_recursive_next <
            range::detail::shared_node <Type, Allocator>>
    {
        typedef range::detail::shared_node <Type, Allocator> node_type;
        typedef typename range::detail::shared_node_allocator <Type, Allocator
            >::type allocator_type;
        typedef small_ptr <node_type, allocator_type> pointer_type;
        pointer_type && operator() (node_type * n) const
        { return std::move (n->next()); }
    };

}} // namespace utility::pointer_policy

namespace range {

/**
A singly-linked list that shares tails with other list.

This means that copying a list and editing is cheap.
Tails can safely be accessed from different threads, because they cannot be
changed.

It also means that traversal is in only one direction.
List item are immutable: it is possible to detach from a list, and to prepend
items, but not to mutate contained elements.

\todo A mutable shared_list (mutable_list?) could be built, and traversal would
    imply copying all elements, in case they are edited, or in case their
    successors end up being edited.
    The mutable view must be invalidated when the underlying range is copied.

\todo shared_list_view can be invalidated. When? Is it possible to enforce this?

\todo At which point must the small_ptr allocator be converted to the right
type?
If within this file, as now, then small_ptr should have a static assertion
checking that Allocator::value_type equals Type.

\todo Properties of the allocator?
Must the allocator be the same for the whole chain? Must the view have an allocator too?
*/
template <class Type, class Direction = direction::front,
    class Allocator = std::allocator <Type>>
class shared_list;

template <class Type, class Direction, class Allocator> class shared_list_view;

namespace detail {

    template <class Type, class Allocator> class shared_node;

    template <class Type, class Allocator> struct shared_node_allocator {
        typedef typename Allocator::template
            rebind <shared_node <Type, Allocator>>::other type;
    };

    template <class Type, class Allocator> class shared_node
    : public utility::shared
    {
        typedef Type element_type;
        typedef typename shared_node_allocator <Type, Allocator>::type
            allocator_type;
        typedef utility::small_ptr <shared_node, allocator_type> next_type;

        element_type element_;
        next_type next_;
    public:
        // Constructors for all combinations of const & and &&.
        shared_node (Type const & element, next_type const & next)
        : element_ (element), next_ (next) {}

        shared_node (Type && element, next_type const & next)
        : element_ (std::move (element)), next_ (next) {}

        shared_node (Type const & element, next_type && next)
        : element_ (element), next_ (std::move (next)) {}

        shared_node (Type && element, next_type && next)
        : element_ (std::move (element)), next_ (std::move (next)) {}

        element_type const & element() const { return element_; }
        element_type & element() { return element_; }

        next_type const & next() const { return next_; }
        next_type & next() { return next_; }
    };

} // namespace detail

template <class Type, class Direction, class Allocator> class shared_list {
public:
    typedef Type element_type;
    typedef Direction direction_type;
    typedef typename detail::shared_node_allocator <Type, Allocator>::type
        allocator_type;

    static_assert (direction::is_direction <Direction>::value,
        "Direction template argument must be a direction.");

private:
    // Use the original allocator for the node type.
    typedef detail::shared_node <Type, Allocator> node_type;
    // The pointer needs the actual allocator, though.
    typedef utility::small_ptr <node_type, allocator_type> node_ptr;
    typedef shared_list_view <Type, Direction, Allocator> view_type;

private:
    explicit shared_list (node_ptr const & first_node)
    : first_node (first_node) {}

    /**
    Append one element to the node_ptr indicated by "last", and return a
    reference to the new last pointer.
    This mutates the linked list, and should therefore only be used for
    construction, when it is known that the list is not shared.

    \pre last is empty, so that the element is appended to the end of the
    sequence.
    */
    struct append_element {
        Allocator const & allocator;

        append_element (Allocator const & allocator) : allocator (allocator) {}

        template <class CVType> node_ptr & operator() (
            node_ptr & last, CVType && element) const
        {
            assert (last.empty());

            last = node_ptr (
                utility::pointer_policy::construct_as <node_type>(),
                allocator,
                std::forward <CVType> (element), node_ptr (allocator));
            return last->next();
        }
    };

    /**
    Append all elements in the range to the list.
    This mutates the linked list, and should therefore only be used for
    construction, when it is known that the list is not shared.

    \pre last is empty, so that the element is appended to the end of the
    sequence.
    */
    template <class Range> node_ptr & append_range (
        Allocator const & allocator, node_ptr & last, Range && range)
    {
        Direction direction;
        return fold (direction, append_element (allocator), last,
            std::forward <Range> (range));
    }

    template <class Element> void do_push (Element && element) {
        // Replace this->first_node by a new node pointing at it.
        this->first_node = node_ptr (
            utility::pointer_policy::construct_as <node_type>(),
            this->first_node.allocator(),
            std::forward <Element> (element), std::move (this->first_node));
    }

public:
    shared_list() : first_node (allocator_type()) {}

    explicit shared_list (Allocator const & allocator)
    : first_node (allocator) {}

    /* Construction from range. */
    // Construct from shared_list.
    shared_list (shared_list const & l)
    : first_node (l.first_node) {}

    shared_list (shared_list && l)
    : first_node (std::move (l.first_node)) {}

    // Construct from view.
    shared_list (view_type const & view)
    : first_node (view.node(), allocator_type()) {}

    /**
    Construct from another range, copying the elements from the range in
    direction Direction.
    \todo And enable only if the elements can be copied.
    */
    template <class Range,
        class Enable1 = typename boost::enable_if <is_range <Range>>::type,
        class Enable2 = typename utility::disable_if_same_or_derived <
            shared_list, Range>::type,
        class Enable3 = typename utility::disable_if_same_or_derived <
            view_type, Range>::type>
    explicit shared_list (Range && range)
    : first_node (Allocator())
    {
        append_range (first_node.allocator(),
            this->first_node, std::forward <Range> (range));
    }

    /**
    Construct from another range, copying the elements from the range in
    direction Direction.
    If the range is a shared_list or a view of a shared_list, it will be copied
    to new nodes allocated with the allocator passed in.
    \todo And enable only if the elements can be copied.
    */
    template <class Range,
        class Enable1 = typename boost::enable_if <is_range <Range>>::type>
    explicit shared_list (Range && range, Allocator const & allocator)
    : first_node (allocator)
    {
        append_range (allocator,
            this->first_node, std::forward <Range> (range));
    }

    /* Assignment. */

    shared_list & operator= (shared_list const &) = default;
    shared_list & operator= (shared_list &&) = default;

    /**
    Assignment always possible.
    \todo Test with view.
    \todo Test with take (3, shared_list)
    \todo Test with function_range.
    \todo Check convertibility of value type?
    */
    template <class Range, class Enable = typename
        utility::disable_if_same_or_derived <shared_list, Range>::type>
    shared_list & operator= (Range && range) {
        // Start a new node; only rewire the current range once there has been
        // no exception.
        node_ptr new_first_node (this->first_node.allocator());
        append_range (this->first_node.allocator(), new_first_node,
            std::forward <Range> (range));
        // Use the new list.
        this->first_node = std::move (new_first_node);

        return *this;
    }

    // \todo Doc.
    bool empty() const { return !first_node; }

    // \todo Doc.
    Type const & first() const { return first_node->element(); }

    // \todo  Doc
    shared_list tail() const { return shared_list (first_node->next()); }

    /**
    Add an element so that <c>first (*this)</c> returns the element.

    \param element
        The element to add.
    */
    void push (Type const & element) { do_push (element); }
    void push (Type && element) { do_push (std::move (element)); }

    /**
    Add an element so that <c>first (direction, *this)</c> returns the element.

    \param direction
        The direction to use.
        This must equal the default-constructed <c>Direction()</c>.
    \param element
        The element to add.
    */
    void push (Direction const & direction, Type const & element) {
        // direction must be equal to the default-constructed type.
        rime::assert_ (direction == Direction());
        this->do_push (element);
    }
    void push (Direction const & direction, Type && element) {
        // direction must be equal to the default-constructed type.
        rime::assert_ (direction == Direction());
        this->do_push (std::move (element));
    }

    /**
    \todo Doc
    \internal
    Even if range is a shared_list, it needs to be added to the front element
    by element.
    */
    template <class Range> void push_range (Range && range) {
        // Start a new node; only rewire the current range once there has been
        // no exception.
        node_ptr new_first_node (this->first_node.allocator());
        node_ptr & last =
            append_range (this->first_node.allocator(), new_first_node,
                std::forward <Range> (range));
        // Append current list to new list.
        last = std::move (this->first_node);
        // Use the new list.
        this->first_node = std::move (new_first_node);
    }

    /**
    \todo Doc
    */
    template <class Range> void push_range (
        Direction const & direction, Range && range)
    {
        // direction must be equal to the default-constructed type.
        rime::assert_ (direction == Direction());
        return this->push_range (std::forward <Range> (range));
    }

    /**
    \todo Doc
    */
    Type pop() {
        if (first_node.unique()) {
            Type result = std::move (first_node->element());
            first_node = std::move (first_node->next());
            return result;
        } else {
            Type result = first_node->element();
            first_node = first_node->next();
            return result;
        }
    }

    /**
    \todo Doc, with copydoc?
    */
    Type pop (Direction const & direction) {
        // direction must be equal to the default-constructed type.
        rime::assert_ (direction == Direction());
        return this->pop();
    }

private:
    node_ptr first_node;

    template <bool Move, class Tag, class Direction2, class Enable>
        friend struct operation::make_view;
};

/**
View of the elements of shared_list_view.
The elements are always returned by const reference, since they may be shared
by other shared_list's.
Viewing a non-const reference or rvalue reference to a shared_list is therefore
no different from any other qualification.

\todo Test

\todo Test assignment of shared_list from shared_list_view.
*/
template <class Type, class Direction, class Allocator> class shared_list_view {
private:
    typedef detail::shared_node <Type, Allocator> node_type;

public:
    shared_list_view (node_type * node)
    : node_ (node) {}

    bool empty() const { return !node_; }

    Type const & first() const { return node_->element(); }

    node_type * node() const { return node_; }

private:
    node_type * node_;
};

// template <class Direction> struct shared_list_tag;
template <class Direction> struct shared_list_view_tag;

template <class Type, class Direction, class Allocator>
    struct tag_of_qualified <shared_list <Type, Direction, Allocator>>
{ typedef heavyweight_tag <shared_list <Type, Direction, Allocator>> type; };

template <class Type, class Direction, class Allocator>
    struct tag_of_qualified <shared_list_view <Type, Direction, Allocator>>
{ typedef shared_list_view_tag <Direction> type; };

namespace operation {

    // shared_list: convert to shared_list_view.

    template <class Type, class Direction, class Allocator>
        struct default_direction <
            heavyweight_tag <shared_list <Type, Direction, Allocator>>>
    : rime::callable::always_default <Direction> {};

    /*
    Since the elements in the shared_list can be shared, they can't be moved
    out.
    There is therefore no difference between Move=true and Move=false.
    */
    template <bool Move, class Type, class Direction, class Allocator>
        struct make_view <Move,
            heavyweight_tag <shared_list <Type, Direction, Allocator>>,
            meta::vector <Direction>>
    {
        typedef shared_list_view <Type, Direction, Allocator> view_type;
            view_type operator() (Direction const & direction,
                shared_list <Type, Direction, Allocator> const & l) const
        {
            rime::assert_ (direction == Direction());
            return view_type (l.first_node.get());
        }
    };

    /* shared_list_view. */

    template <class Direction>
        struct default_direction <shared_list_view_tag <Direction>>
    : rime::callable::always_default <Direction> {};

    template <class Direction>
        struct empty <shared_list_view_tag <Direction>, Direction>
    {
        template <class Type, class Allocator>
            bool operator() (Direction const & direction,
                shared_list_view <Type, Direction, Allocator> const & v) const
        {
            rime::assert_ (direction == Direction());
            return v.empty();
        }
    };

    template <class Direction>
        struct first <shared_list_view_tag <Direction>, Direction>
    {
        template <class Type, class Allocator>
            Type const & operator() (Direction const & direction,
                shared_list_view <Type, Direction, Allocator> const & view)
            const
        {
            rime::assert_ (direction == Direction());
            assert (!view.empty());
            return view.first();
        }
    };

    template <class Direction>
        struct drop_one <shared_list_view_tag <Direction>, Direction>
    {
        template <class One, class Type, class Allocator>
            shared_list_view <Type, Direction, Allocator>
            operator() (Direction const & direction, One const &,
                shared_list_view <Type, Direction, Allocator> const & view)
            const
        {
            rime::assert_ (direction == Direction());
            assert (!view.empty());
            return shared_list_view <Type, Direction, Allocator> (
                view.node()->next().get());
        }
    };

} // namespace operation

} // namespace range

#endif // RANGE_SHARED_LIST_HPP_INCLUDED
