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

// Include the Python C API, safely.
#include <boost/python/detail/wrap_python.hpp>

#include <boost/python/object.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/converter/registry.hpp>

#include "range/core.hpp"

namespace range {

/** \brief
Represent a Python iterable as a range.

Python iterables are such things as lists, tuples, and generators.
The range is lazy; the length of the iterable is not checked.
The elements are extracted as the correct type only as they are queried.

If its elements should be returned as \c boost::python::object, then give no
template parameters.
If its elements are all one type, then give that type as the template parameter.
If its elements have different types, then give the sequence of types as
template parameters.

Even if a sequence of types is given, empty() will happily return \c true when
the Python iterable is exhausted.

To use this class, it must be registered with Boost.Python.
For example, your \c BOOST_PYTHON_MODULE could contain:

\code
using namespace range;
python::convert_object_to_range <python_range <double>>();
def <void (python_range <double>)> ("my_function", my_function);
\endcode

\tparam Types
    The types that the iterable contains.
    If none are given, \c first() will return values of
    \c boost::python::object.
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
This uses the APIs at
http://docs.python.org/2/c-api/iter.html: PyIter_Check, PyIter_Next
https://docs.python.org/2/c-api/object.html: PyObject_GetIter

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

struct python_range_tag;

template <class ... Types>
    struct tag_of_qualified <python_range <Types ...>>
{ typedef python_range_tag type; };

namespace python {
namespace detail {

    class python_range_base {
    protected:
        explicit python_range_base (boost::python::object const & iterable)
        // PyObject_GetIter is equivalent to iter(o).
        : iterator_ (PyObject_GetIter (iterable.ptr()))
        {
            if (!iterator_ && PyErr_Occurred())
                boost::python::throw_error_already_set();
        }

        python_range_base (python_range_base const & that) {
            // Steal the iterator from "that".
            using std::swap;
            swap (this->iterator_, that.iterator_);
            swap (this->first_, that.first_);
        }

        python_range_base (boost::python::handle<> && iterator)
        : iterator_ (iterator.release()) {}

    private:
        // Both the iterator and the cached first element are handles, so that
        // they can be null.
        // boost::python::object's can be None, but we want to distinguish
        // between None in a range and a range being invalid.
        mutable boost::python::handle<> iterator_;
        mutable boost::python::handle<> first_;

    protected:
        /**
        Return a boost::python::object that contains the iterator starting at
        the next element.
        */
        boost::python::object next_iterator() {
            fill_first();
            return boost::python::object (iterator_);
        }

        /**
        Retrieve the first element from the Python iterator.
        Do nothing if the first element has already been retrieved.

        \return A reference to the handle holding the first element.
            If the handle is empty, the iterator was empty.

        \throw boost::python::error_already_set iff a Python exception is
            thrown while retrieving the element.
        */
        boost::python::handle<> & fill_first() const {
            if (!first_) {
                assert (iterator_.get() != nullptr
                    && "The iterable should not have been pilfered.");
                // PyIter_Next() is like next() except it does not raise a
                // StopIteration exception if the iterator is exhausted.
                // Instead, PyIter_Next returns NULL with no exception set.
                // If another exception is thrown, PyIter_Next returns NULL and
                // an exception is set.
                first_ = boost::python::handle<> (boost::python::allow_null (
                    PyIter_Next (iterator_.get())));
                if (PyErr_Occurred())
                    boost::python::throw_error_already_set();
            }
            return first_;
        }
    };

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

    \throw boost::python::error_already_set if a Python exception is raised.
        For example, if the object is not an iterable, the call to \c iter
        raises a ValueError.
    */
    explicit python_range (boost::python::object const & iterable)
    : base_type (iterable) {}

    /**
    Construct from another python_range, stealing its state.
    That means that its first element becomes the first element of this.
    This should really be the move constructor, but Boost.Python is not happy
    with that.
    */
    python_range (python_range const & that) = default;

private:
    template <class Tag, class Direction, class Range, class Enable>
        friend struct operation::drop_one;

    /**
    Construct from a Python iterator with the first element it will produce as
    the first element of the range.
    */
    python_range (boost::python::handle<> && iterator)
    : base_type (std::move (iterator)) {}

private:
    friend class operation::member_access;

    /* empty. */
    bool empty (direction::front) const { return !fill_first(); }

    /* first. */
    /// Extract the first type (if any) from a boost::python::object.
    template <class ... Types2> struct extract_first {
        typedef boost::python::object result_type;

        result_type && operator() (result_type && object) const
        { return std::move (object); }
    };

    template <class FirstType, class ... Rest>
        struct extract_first <FirstType, Rest ...>
    {
        typedef FirstType result_type;

        FirstType operator() (boost::python::object && object) const
        { return boost::python::extract <FirstType> (std::move (object)); }
    };

    typename extract_first <Types ...>::result_type
        first (direction::front) const
    {
        boost::python::handle<> & first = fill_first();
        assert (!!first || "This range is empty.");
        return extract_first <Types ...>() (boost::python::object (first));
    }

    /* chop_in_place. */
    class unavailable_type;
    typedef typename std::conditional <(sizeof ... (Types) <= 1),
        direction::front, unavailable_type>::type front_if_homogeneous;

    // This is the natural way of using a Python iterator.
    typename extract_first <Types ...>::result_type
        chop_in_place (front_if_homogeneous)
    {
        boost::python::handle<> & first = fill_first();
        assert (!!first || "This range is empty.");
        // Set first to 0 and return as an object.
        return extract_first <Types ...>() (boost::python::object (
            boost::python::handle<> (first.release())));
    }
};

namespace operation {

    // drop: only for rvalue ranges.
    template <class Range>
        struct drop_one <python_range_tag, direction::front, Range &&>
    {
        /// Evaluate to the result type.
        // For zero or one types, return the same range type.
        template <class ... Types> struct next_range
        { typedef python_range <Types ...> type; };

        // For two or more types, remove the first type.
        template <class FirstType, class SecondType, class ... Types>
            struct next_range <FirstType, SecondType, Types ...>
        { typedef python_range <SecondType, Types ...> type; };

        template <class One, class ... Types>
            typename next_range <Types ...>::type operator() (
                direction::front, One, python_range <Types ...> && range)
            const
        {
            return typename next_range <Types ...>::type (
                std::move (range.next_iterator()));
        }
    };

    // chop is implemented automatically.

} // namespace operation

namespace python {

/**
Construct an object of this class to register a python_range.
This should be done in the BOOST_PYTHON_MODULE function.
\tparam Range
    The python_range to register.
*/
template <class Range> class convert_object_to_range;

template <class ... Types>
    class convert_object_to_range <python_range <Types ...>>
{
public:
    convert_object_to_range() {
        boost::python::converter::registry::push_back (
            &convertible, &construct,
            boost::python::type_id <python_range <Types ...>>());
    }

private:
    // Always convertible.
    static void * convertible (PyObject * pointer)
    { return pointer /*PyIter_Check (pointer) ? pointer : nullptr*/; }

    static void construct (PyObject* python_object,
        boost::python::converter::rvalue_from_python_stage1_data * data)
    {
        typedef python_range <Types ...> range;
        boost::python::object object (boost::python::handle<> (
            boost::python::borrowed (python_object)));
        typedef boost::python::converter::rvalue_from_python_storage <range>
            storage_type;
        void * storage = reinterpret_cast <storage_type*> (data)->storage.bytes;

        new (storage) range (object);

        data->convertible = storage;
    }
};

} // namespace python

} // namespace range

#endif // RANGE_PYTHON_PYTHON_RANGE_HPP_INCLUDED
