*************
Range library
*************

The Range library deals with ranges, sequences of objects.

.. rubric:: License

The Range library is licensed under the Apache License, Version 2.0.

.. rubric:: Contents

.. toctree::
    :maxdepth: 2

    intrinsic.rst
    utility.rst
    query.rst
    compile_time.rst
    existing.rst
    predefined.rst
    iteration.rst
    adaptors.rst
    interfaces.rst
    python.rst
    function.rst
    hash.rst
    implement.rst

.. highlight:: cpp

.. rubric:: Design

A range is an object that represents sequential data. One example is std::vector, which contains a variable number of elements of the same type. A more surprising example is std::tuple.
It contains a fixed number of elements with fixed types, which can be different from element to element.

The range library allows you to treat different types of ranges in the same way.
So, for example, the following code ::

    std::vector <int> integers {5, 7, 9};
    std::tuple <int, std::string, float> various {6, "Hello", 7.5};

    // Print the first and second element.
    std::cout << first (integers) << ' ' << at_c <1> (integers) << std::endl;
    std::cout << first (various) << ' ' << at_c <1> (various) << std::endl;

prints::

    5 7
    6 Hello

A ``vector`` and a
Traditionally, iterating over a container has been done with iterators: a start iterator, and a past-the-end iterator.
It is tedious and error-prone to always move two iterators around, and composition of operations becomes hard.
Even when a function returns a pair of iterators, they cannot normally be passed straight into another function.
It is therefore useful to package two iterators and call the package a "range".
This is what the Boost.Range library does, for example.

However, this library generalises the concept of a range from a pair of iterators.
A range is not necessarily composed of two iterators any more.
This approach was proposed forcefully by `Andrei Alexandrescu's keynote speech at BoostCon 2009`_.
Doing away with iterators makes it possible to use the same algorithms over sequences of infinite length, or, as in the example, of different element types.
It also makes it easier to write lazy ranges, which can improve performance compared to the traditional implementation.

To allow ranges with different types, the types of the ranges themselves must change.
The whole infrastructure therefore uses free functions.
A range has a few intrinsic operations:

-   ``empty``.
    Returns true if the range is empty, i.e. if it has no elements, and false if it does have elements.
    For example::

        assert (! empty (integers));

-   ``size``.
    Returns the size of the range.
    For example::

        assert (size (various) == 3);

-   ``first``.
    Returns (a reference to) the first element of a range.
    For example::

        assert (first (integers) == 5);

    An interesting feature is that a "direction" can be used.
    For example, to get the first element from the back as opposed to the front::

        assert (first (various, back) == 7.5);

-   ``drop (r)``.
    Returns a *view* of the range minus its first element.
    For example, ``drop (integers)`` returns a view without element ``5``::

        assert (first (drop (integers)) == 7);

Operations
==========

Other operations are defined on ranges.
They are documented, but there is no high-level overview yet.
Sorry.

.. _Andrei Alexandrescu's keynote speech at BoostCon 2009: http://erdani.com/index.php/media/
