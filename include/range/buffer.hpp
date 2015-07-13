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

#ifndef RANGE_BUFFER_HPP_INCLUDED
#define RANGE_BUFFER_HPP_INCLUDED

#include <type_traits>
#include <memory>
#include <new>

#include "utility/returns.hpp"
#include "utility/pointer_policy.hpp"
#include "utility/unique_ptr.hpp"
#include "utility/disable_if_same.hpp"
#include "utility/is_trivially_destructible.hpp"

#include "core.hpp"

namespace range {

/** \brief
Simple producer of elements for use with \ref buffer.

This must be subclassed to allow \ref buffer to provide elements from this
source.
The derived type must implement get_next() and first(), and its constructor
must set end_.
*/
template <class Element> class element_producer;

/** \brief
Element producer for \ref buffer that holds its elements inside the object
itself.

\sa element_producer

\tparam Element The type of the elements
\tparam NumberOrZero (optional) Number of elements to hold in one producer.
*/
template <class Element, std::size_t NumberOrZero>
    class internal_element_producer;

} // namespace detail

namespace utility { namespace pointer_policy {

    /*
    Make sure buffers are destructed non-recursively.
    */
    template <class Element>
        struct move_recursive_next <range::element_producer <Element>>
    {
        typedef range::element_producer <Element> producer;
        typedef typename producer::pointer pointer;

        pointer && operator() (producer * producer) const
        { return std::move (producer->next_); }
    };

}} // namespace utility::pointer_policy


namespace range {

namespace buffer_detail {

    template <class Range, class Element, std::size_t NumberOrZero>
        class range_element_producer;

} // namespace buffer_detail

/** \brief
Range that keeps a read-only buffer of elements from a producer.

The producer could be another range, for example if that range provides only
\c chop, or if computing elements from it is slow.
Another useful option is a buffer acquired with \ref read_file, which gives
sequential access to the bytes in a file.

This class is copyable, and many buffers can use the same underlying producer.
However, this class is not thread-safe: multiple calls to buffers of the same
producer cannot be made at the same time.

The underlying elements are kept in chunks.
As all buffers go out of scope or are moved forward, memory for earlier chunks
gets deallocated.

The buffers must be derived from type \ref element_producer\<Element>.

\tparam Element
    The type of the elements that the range contains.
    Will be returned by value, so should be small.
*/
template <class Element> class buffer {
    typedef range::element_producer <Element> producer_type;
public:
    /// Type of pointer to the underlying producer.
    typedef typename producer_type::pointer producer_ptr;

private:
    producer_ptr producer_;
    Element const * first_;

public:
    /// Construct a buffer that starts with the elements in \a producer.
    explicit buffer (producer_ptr producer)
    : producer_ (std::move (producer)), first_ (producer_->first()) {}

private:
    /// Construct a buffer with \a producer, starting not necessarily at its
    /// first element.
    buffer (producer_ptr producer, Element const * first)
    : producer_ (std::move (producer)), first_ (first) {}

private:
    friend class range::helper::member_access;
    /* Range interface. */

    bool empty (direction::front) const { return first_ == producer_->end(); }

    Element first (direction::front) const { return *first_; }

    buffer drop_one (direction::front) const {
        assert (!empty (front));

        Element const * new_first = first_ + 1;
        if (new_first == producer_->end()) {
            producer_ptr next_producer = producer_->next();
            if (next_producer)
                return buffer (std::move (next_producer));
            // If there is no next producer, the resulting range is empty.
            // Leave things as they are.
        }
        return buffer (producer_, new_first);
    }

    Element chop_in_place (direction::front) {
        assert (!empty (front));
        Element result = *first_;

        ++ first_;
        if (first_ == producer_->end()) {
            // Go to next producer.
            producer_ptr next_producer = producer_->next();
            if (next_producer) {
                producer_ = std::move (next_producer);
                first_ = producer_->first();
            } else {
                // It should be clear that this is empty from now on.
                assert (empty (front));
            }
        }

        return result;
    }
};

namespace operation {
    struct buffer_tag {};
} // namespace operation

template <class Element> struct tag_of_qualified <buffer <Element>>
{ typedef operation::buffer_tag type; };

namespace buffer_detail {

template <class Element, std::size_t NumberOrZero> struct compute_element_num {
    static constexpr std::size_t value =
        NumberOrZero == 0 ?
        // Automatic: max 256 bytes.
        ((sizeof (Element) < 256) ? 256 / sizeof (Element) : 1)
        : NumberOrZero;
};

} // namespace buffer_detail

/** \brief
Make a \ref buffer object from a range.

This erases the type of the range, and allows copying, first(), and drop() with
an increment of one.
If the underlying range is an input range, this is an upgrade.

The range is traversed along its default direction, though the buffer always
uses \ref front.

\tparam Element The element type of the buffer.
\tparam NumberOrZero The number of elements kept in one chunk.
*/
template <class Element, std::size_t NumberOrZero, class Range,
    class Enable1 = typename
        std::enable_if <is_range <Range>::value>::type,
    class View = typename decayed_result_of <callable::view (Range)>::type>
buffer <Element> make_buffer (Range && range)
{
    typedef typename buffer <Element>::producer_ptr producer_ptr;
    return buffer <Element> (producer_ptr::template construct <
        buffer_detail::range_element_producer <View, Element, NumberOrZero>> (
            view (std::forward <Range> (range))));
}

template <class Element> class element_producer
: public utility::shared
{
public:
    typedef utility::pointer_policy::strict_weak_ordered <
            utility::pointer_policy::pointer_access <
            utility::pointer_policy::reference_count_shared <
            utility::pointer_policy::with_recursive_type <
            utility::pointer_policy::use_new_delete <
                element_producer>>>>>
        pointer_policy;

    /// Type of pointer to objects of class element_producer.
    class pointer
    : public utility::pointer_policy::pointer <pointer_policy, pointer>
    {
        typedef utility::pointer_policy::pointer <pointer_policy, pointer>
            base_type;
    public:
        template <class ... Arguments>
            explicit pointer (Arguments && ... arguments)
        : base_type (std::forward <Arguments> (arguments) ...) {}

    };

private:
    pointer next_;

    template <class Producer> friend
        struct utility::pointer_policy::move_recursive_next;

protected:
    /// Past-the-end element pointer.
    /// Set this in the derived type's constructor.
    Element const * end_;

    /**
    Construct the element_producer.
    \ref end_ is unset, so the constructor of the derived type should set it.
    */
    explicit element_producer() : next_() {}

    element_producer (element_producer const &) = delete;
    element_producer (element_producer &&) = delete;

    /** \brief
    Get a pointer to the next producer.

    If there is no such producer, then return an empty pointer.
    This will be called only once if a non-empty pointer is returned.
    It is therefore possible for the final element_producer to hold a unique
    object or std::unique_ptr.

    However, if an empty pointer is returned, it can be called multiple times
    (but should return an empty pointer each time).
    */
    virtual pointer get_next() = 0;

public:
    /// Return a pointer to the next producer.
    pointer next() {
        if (!next_)
            next_ = get_next();
        return next_;
    }

    /// Return a raw pointer to the first element.
    /// This must be implemented in the derived class.
    virtual Element const * first() const = 0;

    // Return the past-the-end pointer for this producer.
    Element const * end() const { return end_; }

    virtual ~element_producer() {}
};

template <class Element, std::size_t NumberOrZero>
    class internal_element_producer
: public element_producer <Element>
{
    typedef element_producer <Element> base_type;
public:
    typedef typename base_type::pointer pointer;

private:
    static constexpr std::size_t buffer_size
        = buffer_detail::compute_element_num <Element, NumberOrZero>::value;
    typename std::aligned_storage <sizeof (Element), alignof (Element)>::type
        memory_ [buffer_size];

    void destruct_elements (rime::true_type trivial) {}

    void destruct_elements (rime::false_type trivial) {
        Element * current = memory();
        while (current != this->end_) {
            current->~Element();
            ++ current;
        }
    }

protected:
    /// Return a pointer to the first element in the internal array of elements.
    Element * memory()
    { return reinterpret_cast <Element *> (memory_); }

    Element const * memory() const
    { return reinterpret_cast <Element const *> (memory_); }

    Element const * memory_end() const
    { return memory() + buffer_size; }

public:
    /** \brief
    Construct a new internal_element_producer, with next_ unset.

    It is the task of the derived constructor to fill the array with at most
    \a Number elements, and set end_.
    */
    explicit internal_element_producer() : base_type() {}

    /** \brief
    Destruct the array up to end_.
    */
    virtual ~internal_element_producer()
    { destruct_elements (utility::is_trivially_destructible <Element>()); }

    virtual Element const * first() const { return memory(); }
};

namespace buffer_detail {

template <class Range, class Element, std::size_t NumberOrZero>
class range_element_producer
: public internal_element_producer <Element, NumberOrZero>
{
    typedef internal_element_producer <Element, NumberOrZero> base_type;
    typedef typename base_type::pointer pointer;

    // Only the last producer needs and has access to the range.
    std::unique_ptr <Range> range_;

protected:
    virtual pointer get_next() {
        return pointer::template construct <range_element_producer> (
            std::move (range_));
    }

    void fill() {
        Element * current = this->memory();

        while (!empty (*range_) && current != this->memory_end()) {
            // Use placement new.
            new (current) Element (chop_in_place (*range_));
            ++ current;
        }

        this->end_  = current;
    }

public:
    range_element_producer (std::unique_ptr <Range> && r)
    : internal_element_producer <Element, NumberOrZero>(),
        range_ (std::move (r))
    { fill(); }

    range_element_producer (Range && range)
    : internal_element_producer <Element, NumberOrZero>(),
        range_ (utility::make_unique <Range> (std::move (range)))
    {
        fill();
    }

    range_element_producer (Range const & range)
    : internal_element_producer <Element, NumberOrZero>(),
        range_ (utility::make_unique <Range> (range))
    { fill(); }
};

} // namespace buffer_detail

} // namespace range

#endif // RANGE_BUFFER_HPP_INCLUDED
