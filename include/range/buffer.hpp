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

#include "core.hpp"

namespace range {

/** \brief
Simple producer of elements for use with \ref buffer.

This must be subclassed to allow other producers than the standard one, which
takes elements from a range.

\sa buffer
*/
template <class Element, std::size_t Number> class element_producer;

namespace buffer_detail {

    template <class Element> class element_producer_base;

} // namespace buffer_detail

} // namespace detail

namespace utility { namespace pointer_policy {

    /*
    Make sure buffers are destructed non-recursively.
    */
    template <class Element>
        struct move_recursive_next <range::buffer_detail::
            element_producer_base <Element>>
    {
        typedef range::buffer_detail::element_producer_base <Element> producer;
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

Not threadsafe.

\tparam Element
    Will be returned by value, so should be small.

\todo Make Number better
*/
template <class Element> class buffer {

    typedef range::buffer_detail::element_producer_base <Element> producer_type;
public:
    // \todo Or use something else for the interface?
    typedef typename producer_type::pointer producer_ptr;

private:
    producer_ptr producer_;
    Element const * first_;

    // \todo Which one?
private:
public:
    explicit buffer (producer_ptr producer)
    : producer_ (std::move (producer)), first_ (producer_->first()) {}

    buffer (producer_ptr producer, Element const * first)
    : producer_ (std::move (producer)), first_ (first) {}

public:
private:
    friend class range::helper::member_access;

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




/**
\todo Must be default direction.
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

namespace buffer_detail {

    template <class Element> class element_producer_base
    : public utility::shared
    {
    public:
        typedef utility::pointer_policy::strict_weak_ordered <
                utility::pointer_policy::pointer_access <
                utility::pointer_policy::reference_count_shared <
                utility::pointer_policy::with_recursive_type <
                utility::pointer_policy::use_new_delete <
                    element_producer_base>>>>>
            pointer_policy;

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

    protected:
        Element * end_;
        pointer next_;

        template <class Producer> friend
            struct utility::pointer_policy::move_recursive_next;

    protected:
        explicit element_producer_base() : next_() {}

        element_producer_base (element_producer_base const &) = delete;
        element_producer_base (element_producer_base &&) = delete;

        /**
        Get a pointer to a newly produced producer.
        */
        virtual pointer get_next() = 0;

    public:
        pointer next() {
            if (!next_)
                next_ = get_next();
            return next_;
        }

        virtual Element const * first() const = 0;
        Element const * end() const { return end_; }

        virtual ~element_producer_base() {}
    };

} // namespace buffer_detail

template <class Element, std::size_t Number> class element_producer
: public buffer_detail::element_producer_base <Element>
{
    typedef buffer_detail::element_producer_base <Element> base_type;
public:
    typedef typename base_type::pointer pointer;

protected:
    Element * memory()
    { return reinterpret_cast <Element *> (memory_); }

    Element const * memory() const
    { return reinterpret_cast <Element const *> (memory_); }

private:
    typename std::aligned_storage <sizeof (Element), alignof (Element)>::type
        memory_ [Number];

    void destruct_elements (rime::true_type trivial) {}

    void destruct_elements (rime::false_type trivial) {
        Element * current = memory();
        while (current != this->end_) {
            current->~Element();
            ++ current;
        }
    }

public:
    explicit element_producer() : base_type() {}

    virtual ~element_producer()
    { destruct_elements (std::has_trivial_destructor <Element>()); }

    virtual Element const * first() const { return memory(); }
};

namespace buffer_detail {

template <class Element, std::size_t NumberOrZero> struct compute_element_num {
    static constexpr std::size_t value =
        NumberOrZero == 0 ?
        // Automatic: max 256 bytes.
        ((sizeof (Element) < 256) ? 256 / sizeof (Element) : 1)
        : NumberOrZero;
};



template <class Range, class Element, std::size_t NumberOrZero>
class range_element_producer
: public element_producer <Element, compute_element_num <Element, NumberOrZero>::value>
{
    static constexpr std::size_t buffer_size = compute_element_num <Element, NumberOrZero>::value;
    typedef element_producer <Element, buffer_size> base_type;
    typedef typename base_type::pointer pointer;

    // Only the last producer needs and has access to the range.
    std::unique_ptr <Range> range_;

protected:
    virtual pointer get_next()
    { return pointer::template construct <range_element_producer> (std::move (range_)); }

    void fill() {
        Element * current = this->memory();

        while (!empty (*range_) && current != this->memory() + buffer_size) {
            // Use placement new.
            new (current) Element (chop_in_place (*range_));
            ++ current;
        }

        this->end_  = current;
    }

public:
    range_element_producer (std::unique_ptr <Range> && r)
    : element_producer <Element, buffer_size>(), range_ (std::move (r))
    { fill(); }

    range_element_producer (Range && range)
    : element_producer <Element, buffer_size>(),
        range_ (utility::make_unique <Range> (std::move (range)))
    {
        fill();
    }

    range_element_producer (Range const & range)
    : element_producer <Element, buffer_size>(),
        range_ (utility::make_unique <Range> (range))
    { fill(); }
};

} // namespace buffer_detail

} // namespace range

#endif // RANGE_BUFFER_HPP_INCLUDED
