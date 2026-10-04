/*
Copyright 2014, 2015 Rogier van Dalen.

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
Allow interaction with Python iterables.
*/

#ifndef RANGE_PYTHON_PYTHON_RANGE_HPP_INCLUDED
#define RANGE_PYTHON_PYTHON_RANGE_HPP_INCLUDED

#include <cassert>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <utility>

#include <nanobind/nanobind.h>

#include "range/core.hpp"

namespace range {

/** \brief
Represent a Python iterable as a range.

Python iterables are such things as lists, tuples, and generators.
The range is lazy; the length of the iterable is not checked.
The elements are extracted as the correct type only as they are queried.

If its elements should be returned as \c nanobind::object, then give no
template parameters.
If its elements are all one type, then give that type as the template parameter.
If its elements have different types, then give the sequence of types as
template parameters.

Even if a sequence of types is given, empty() will happily return \c true when
the Python iterable is exhausted.

A python_range can be used as an argument of a function exposed with Nanobind;
a Nanobind type caster is defined for it below.
For example, your \c NB_MODULE could contain:

\code
module.def ("my_function", my_function); // void my_function (python_range<double>)
\endcode

\tparam Types
    The types that the iterable contains.
    If none are given, \c first() will return values of
    \c nanobind::object.
    If one or more are given, those types will be extracted from the Python
    iterable in order.
    The last type will be repeated indefinitely.

Python exceptions are propagated.

This class is defined to call next() on the Python iterator lazily.
However, it is not defined when it is called exactly; it may be called earlier
than expected.
This may yield unexpected results if the Python iterator has side effects or
raises exceptions.

\internal

This class has two attributes: iterator_ and first_.
iterator_ holds the current iterator; if it has been moved or copied out from,
it is null (not None); otherwise it is a Python iterator.
first_ caches the first object.
It is either null, or holds the first object, if next() has been called on the
Python iterator already.
first_ is necessary because all operations can need to call next() on the
Python iterator, because of how the Python iterator protocol works: next() tells
whether the range is empty and if so returns the first element.
*/
template <class ... Types> class python_range;

namespace python_range_operation {
    struct python_range_tag {};
} // namespace python_range_operation

template <class ... Types>
    struct tag_of_qualified <python_range <Types ...>>
{ typedef python_range_operation::python_range_tag type; };

namespace python {
namespace detail {

    class python_range_base {
    protected:
        explicit python_range_base (nanobind::object const & iterable)
        // nanobind::iter is equivalent to iter(o).
        // It throws nanobind::python_error if the object is not iterable.
        : iterator_ (nanobind::iter (iterable)) {}

        python_range_base (python_range_base const & that) {
            // Steal the iterator from "that".
            using std::swap;
            swap (this->iterator_, that.iterator_);
            swap (this->first_, that.first_);
        }

    private:
        // Both the iterator and the cached first element can be null (invalid),
        // which is different from being None.
        // nanobind::object's can be None, but we want to distinguish
        // between None in a range and a range being invalid.
        mutable nanobind::object iterator_;
        mutable nanobind::object first_;

    protected:
        /**
        Return a nanobind::object that contains the iterator starting at
        the next element.
        */
        nanobind::object next_iterator() {
            fill_first();
            return iterator_;
        }

        /**
        Retrieve the first element from the Python iterator.
        Do nothing if the first element has already been retrieved.

        \return A reference to the object holding the first element.
            If the object is invalid, the iterator was empty.

        \throw nanobind::python_error iff a Python exception is thrown while
            retrieving the element.
        */
        nanobind::object & fill_first() const {
            if (!first_.is_valid()) {
                assert (iterator_.is_valid()
                    && "The iterable should not have been pilfered.");
                // obj_iter_next() is like next() except it does not raise a
                // StopIteration exception if the iterator is exhausted.
                // Instead, it returns NULL, which makes first_ invalid.
                // If another exception is thrown, it throws
                // nanobind::python_error.
                first_ = nanobind::steal (
                    nanobind::detail::obj_iter_next (iterator_.ptr()));
            }
            return first_;
        }
    };

    template <class Range> inline
        nanobind::object next_iterator (Range & range)
    { return range.next_iterator(); }

} // namespace detail

} // namespace python

template <class ... Types> class python_range
: python::detail::python_range_base
{
    typedef python::detail::python_range_base base_type;
public:
    /**
    Construct from a Python iterable.
    Note that this does not have to be an iterator: the equivalent of
    <c>iter (iterable)</c> is called.

    \throw nanobind::python_error if a Python exception is raised.
        For example, if the object is not an iterable, the call to \c iter
        raises a TypeError.
    */
    explicit python_range (nanobind::object const & iterable)
    : base_type (iterable) {}

    /**
    Construct from another python_range, stealing its state.
    That means that its first element becomes the first element of this.
    This should really be the move constructor, but it is a copy constructor
    so that python_range can be copied where a copy is syntactically needed.
    */
    python_range (python_range const & that) = default;

private:
    friend nanobind::object
        python::detail::next_iterator <python_range> (python_range &);

private:
    friend class helper::member_access;

    /* empty. */
    bool empty (direction::front) const { return !fill_first().is_valid(); }

    /* first. */
    /// Extract the first type (if any) from a nanobind::object.
    template <class ... Types2> struct extract_first {
        typedef nanobind::object result_type;

        result_type && operator() (result_type && object) const
        { return std::move (object); }
    };

    template <class FirstType, class ... Rest>
        struct extract_first <FirstType, Rest ...>
    {
        typedef FirstType result_type;

        FirstType operator() (nanobind::object && object) const
        { return nanobind::cast <FirstType> (std::move (object)); }
    };

    typename extract_first <Types ...>::result_type
        first (direction::front) const
    {
        nanobind::object & first = fill_first();
        assert (first.is_valid() && "This range is empty.");
        return extract_first <Types ...>() (nanobind::object (first));
    }

    /* chop_in_place. */
    class unavailable_type;
    typedef typename std::conditional <(sizeof ... (Types) <= 1),
        direction::front, unavailable_type>::type front_if_homogeneous;

    // This is the natural way of using a Python iterator.
    typename extract_first <Types ...>::result_type
        chop_in_place (front_if_homogeneous)
    {
        nanobind::object & first = fill_first();
        assert (first.is_valid() && "This range is empty.");
        // Set first to null and return it.
        return extract_first <Types ...>() (
            nanobind::object (std::move (first)));
    }
};

namespace python_range_operation {

    // drop_one: only defined for rvalue references.
    // For zero or one types, return the same range type.
    template <class ... Types>
        inline python_range <Types ...> implement_drop_one (
            python_range_tag const &, python_range <Types ...> && range,
            direction::front)
    {
        return python_range <Types ...> (
            ::range::python::detail::next_iterator (range));
    }

    // For two or more types, remove the first type.
    template <class FirstType, class SecondType, class ... Types>
        inline python_range <SecondType, Types ...> implement_drop_one (
            python_range_tag const &,
            python_range <FirstType, SecondType, Types ...> && range,
            direction::front)
    {
        return python_range <SecondType, Types ...> (
            ::range::python::detail::next_iterator (range));
    }

    // chop is implemented automatically.

} // namespace python_range_operation

} // namespace range

namespace nanobind { namespace detail {

    /**
    Nanobind type caster that converts any Python iterable to a python_range.

    \tparam Range
        The python_range to convert to.
    */
    template <class Range> struct python_range_caster {
        using Value = Range;
        static constexpr auto Name = const_name ("Iterable");

        template <class T> using Cast = movable_cast_t <T>;

        // Python_range is not default-constructible.
        std::optional <Range> value;

        bool from_python (nanobind::handle source, std::uint8_t,
            cleanup_list *) noexcept
        {
            try {
                value.emplace (nanobind::borrow (source));
                return true;
            } catch (nanobind::python_error &) {
                // The object is not iterable (iter() raised a TypeError).
                return false;
            }
        }

        operator Range * () { return &*value; }
        operator Range & () { return *value; }
        operator Range && () { return std::move (*value); }
    };

    template <class ... Types>
        struct type_caster <::range::python_range <Types ...>>
    : python_range_caster <::range::python_range <Types ...>> {};

}} // namespace nanobind::detail

#endif // RANGE_PYTHON_PYTHON_RANGE_HPP_INCLUDED
