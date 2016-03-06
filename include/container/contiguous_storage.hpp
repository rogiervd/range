/*
Copyright 2016 Rogier van Dalen.

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

#ifndef CONTAINER_CONTIGUOUS_STORAGE_HPP_INCLUDED
#define CONTAINER_CONTIGUOUS_STORAGE_HPP_INCLUDED

#include <cassert>
#include <stdlib.h> // for aligned_alloc
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <new>

#include "utility/with_allocator.hpp"

// For range::front and range::back.
#include "range/core.hpp"

namespace container {

    /* Contiguous storage policies. */

    /*
    Storage policies encapsulate methods of keeping contiguous memory for
    multiple objects.
    They deal with allocating and deallocating this memory.
    All objects allocates a block of memory on construction and deallocates it
    on destruction.

    Some types of storage can be resized, iff static bool resizable is then true.
    This works by constructing a new storage, and using move-assign or swap.
    Move-assignment and swap are noexcept.
    */

    /**
    Storage policy that has memory for the objects inside itself.
    */
    template <class Type, std::size_t Number> class internal_storage {
    public:
        typedef Type value_type;
        typedef Type * pointer;
        typedef Type const * const_pointer;

        internal_storage() {}

        internal_storage (internal_storage const &) = delete;
        internal_storage (internal_storage &&) = delete;

        internal_storage & operator= (internal_storage const &) = delete;
        internal_storage & operator= (internal_storage &&) = delete;

        const_pointer memory_begin() const {
            return static_cast <const_pointer> (
                static_cast <void const *> (&memory[0]));
        }
        
        pointer memory_begin()
        { return static_cast <pointer> (static_cast <void *> (&memory[0])); }

        const_pointer memory_end() const { return memory_begin() + Number; }
        pointer memory_end() { return memory_begin() + Number; }

        std::size_t memory_size() const { return Number; }

        // Does not have "resized".
        // Even declaring this would mean declaring a move constructor, so let's
        // not.

        static const bool resizable = false;

    private:
        typename std::aligned_storage
            <sizeof(Type), alignof (Type)>::type memory [Number];
    };

    template <class Type> class dynamic_storage {
    public:
        typedef Type value_type;
        typedef Type * pointer;
        typedef Type const * const_pointer;

        /// \todo Document what "size" means here!
        dynamic_storage (std::size_t size) {
            if (size == 0) {
                begin_ = nullptr;
                end_ = nullptr;
            } else {
                begin_ = static_cast <Type *> (
                    aligned_alloc (alignof (Type), sizeof (Type) * size));
                end_ = begin_ + size;
            }
        }

        ~dynamic_storage() { free (begin_); }

        dynamic_storage (dynamic_storage && other) noexcept
        { pilfer (std::move (other)); }

        dynamic_storage & operator= (dynamic_storage && other) noexcept {
            // Move construction is impossible from this itself.
            assert (this != &other);
            free (begin_);
            pilfer (std::move (other));
            return *this;
        }

        const_pointer memory_begin() const { return begin_; }
        pointer memory_begin() { return begin_; }

        const_pointer memory_end() const { return end_; }
        pointer memory_end() { return end_; }

        std::size_t memory_size() const { return end_ - begin_; }

        static const bool resizable = true;

        dynamic_storage resized (std::size_t new_size) const
        { return dynamic_storage (new_size); }

        friend void swap (dynamic_storage & left, dynamic_storage & right) {
            swap (left.begin_, right.begin_);
            swap (left.end_, right.end_);
        }

    private:
        void pilfer (dynamic_storage && other) noexcept {
            this->begin_ = other.begin_;
            this->end_ = other.end_;
            other.begin_ = nullptr;
            other.end_ = nullptr;
        }

        pointer begin_;
        pointer end_;
    };

    template <class Allocator> class allocator_storage
    : utility::with_allocator <Allocator> {
        typedef utility::with_allocator <Allocator> with_allocator;
    public:
        typedef typename Allocator::value_type value_type;
        typedef typename Allocator::pointer pointer;
        typedef typename Allocator::const_pointer const_pointer;

        allocator_storage (Allocator const & allocator, std::size_t size)
        : with_allocator (allocator)
        {
            if (size == 0) {
                begin_ = nullptr;
                end_ = nullptr;
            } else {
                begin_ = with_allocator::allocator().allocate (size);
                end_ = begin_ + size;
            }
        }

        ~allocator_storage() { deallocate(); }

        allocator_storage (allocator_storage && other) noexcept
        : with_allocator (other.allocator())
        { pilfer (std::move (other)); }

        allocator_storage & operator= (allocator_storage && other) noexcept {
            // Move construction is impossible from this itself.
            assert (this != &other);
            deallocate();
            pilfer (std::move (other));
            return *this;
        }

        Allocator const & allocator() const
        { return with_allocator::allocator(); }

        const_pointer memory_begin() const { return begin_; }
        pointer memory_begin() { return begin_; }

        const_pointer memory_end() const { return end_; }
        pointer memory_end() { return end_; }

        std::size_t memory_size() const { return end_ - begin_; }

        static const bool resizable = true;

        allocator_storage resized (std::size_t new_size) const
        { return allocator_storage (with_allocator::allocator(), new_size); }

        friend void swap (allocator_storage & left, allocator_storage & right) noexcept {
            using std::swap;
            swap (left.with_allocator::allocator(),
                right.with_allocator::allocator());
            swap (left.begin_, right.begin_);
            swap (left.end_, right.end_);
        }


    private:
        void deallocate() {
            if (begin_ != nullptr)
                with_allocator::allocator().deallocate (begin_, end_ - begin_);
        }

        void pilfer (allocator_storage && other) noexcept {
            begin_ = other.begin_;
            end_ = other.end_;
            other.begin_ = nullptr;
            other.end_ = nullptr;
        }

        pointer begin_;
        pointer end_;
    };

    /* Resize policies. */

    struct exponential_2 {
        static std::size_t grow (std::size_t current, std::size_t minimum) {
            assert (minimum > current);
            if (current == 0)
                current = 1;
            while (current < minimum)
                current *= 2;
            return current;
        }

        static std::size_t shrink (std::size_t current, std::size_t minimum) {
            assert (minimum < current);
            if (minimum == 0)
                return 0;
            while (current/2 >= minimum)
                current /= 2;
            return current;
        }
    };

    /* Padding policies. */

    template <class Storage, class SizePolicy>
        class flush_left
    {
    public:
        typedef typename Storage::value_type value_type;
        typedef typename Storage::pointer pointer;
        typedef typename Storage::const_pointer const_pointer;

        /**
        Initialise empty, but with storage 
        */
        template <class ... StorageArguments>
            flush_left (StorageArguments && ... storage_arguments)
        : storage_(std::forward <StorageArguments> (storage_arguments) ...),
            end_ (storage_.memory_begin()) {}

        ~flush_left() { destruct_elements (storage_.memory_begin(), end_); }

        // \todo Copy/move  constructor.
        // \todo Copy/move  assignment.

        const_pointer begin() const { return storage_.memory_begin(); }
        pointer begin() { return storage_.memory_begin(); }

        const_pointer end() const { return end_; }
        pointer end() { return end_; }

        std::size_t size() const { return end() - begin(); }

        std::size_t space() const { return storage_.memory_end() - end_; }

        void extend (direction::back direction, value_type const & value) {
            if (space() == 0) {
                assert (size() == storage_.memory_size());
                std::size_t new_memory_size =
                    SizePolicy::grow (size(), size() + 1);
                grow_storage (direction, new_memory_size);
            }
            ::new (static_cast <void*> (std::addressof (*end_)))
                value_type (value);
            ++ end_;
        }

        void shorten (direction::back direction) {
            if (Storage::resizable) {
                std::size_t memory_size = storage_.memory_size();
                std::size_t new_memory_size =
                    SizePolicy::shrink (memory_size, size() - 1);
                if (new_memory_size != memory_size) {
                    // If resizing is necessary, then do so.
                    // This also destructs the last element.
                    shrink_storage (direction, end_ - 1, new_memory_size);
                    return;
                }
            }
            pointer new_end = end_ - 1;
            new_end->~value_type();
            end_ = new_end;
        }

    private:
        struct dummy;

        static void destruct_elements (pointer begin, pointer end) {
            for (pointer current = begin; current != end; ++ current)
                current->~value_type();
        }

        /**
        Resize the storage.
        If this is not possible (at compile time), then an exception is thrown
        instead.
        */
        void grow_storage (direction::back,
            typename std::conditional <
                Storage::resizable, std::size_t, dummy>::type new_memory_size)
        {
            using std::swap;
            static_assert (noexcept (swap (
                std::declval <Storage &>(), std::declval <Storage &>())),
                "swap() on Storage should be noexcept.");

            Storage new_storage = storage_.resized (new_memory_size);
            // \todo Use http://en.cppreference.com/w/cpp/utility/move_if_noexcept
            pointer new_end = std::uninitialized_copy (begin(), end_,
                new_storage.memory_begin());
            swap (this->storage_, new_storage);
            swap (this->end_, new_end);
            destruct_elements (new_storage.memory_begin(), new_end);
            assert (space() >= 1);
        }

        void grow_storage (direction::back,
            typename std::conditional <
                Storage::resizable, dummy, std::size_t>::type)
        {
            // \todo Or something else.
            throw std::overflow_error ("Overflow of fixed-size storage");
        }

        /**
        Resize the storage.
        If this is not possible (at compile time), then an exception is thrown
        instead.
        */
        void shrink_storage (direction::back, pointer new_last,
            typename std::conditional <
                Storage::resizable, std::size_t, dummy>::type new_memory_size)
        {
            using std::swap;
            static_assert (noexcept (swap (
                std::declval <Storage &>(), std::declval <Storage &>())),
                "swap() on Storage should be noexcept.");

            Storage new_storage = storage_.resized (new_memory_size);
            // \todo Use http://en.cppreference.com/w/cpp/utility/move_if_noexcept
            pointer new_end = std::uninitialized_copy (begin(), new_last,
                new_storage.memory_begin());
            swap (this->storage_, new_storage);
            swap (this->end_, new_end);
            destruct_elements (new_storage.memory_begin(), new_end);
            assert (size() <= new_memory_size);
        }

        void shrink_storage (direction::back, pointer,
            typename std::conditional <
                Storage::resizable, dummy, std::size_t>::type)
        {
            // \todo Or something else.
            throw std::logic_error ("Cannot resize fixed-size storage");
        }

        Storage storage_;
        pointer end_;
    };

} // namespace container

#endif // CONTAINER_CONTIGUOUS_STORAGE_HPP_INCLUDED
