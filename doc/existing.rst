.. _existing:

***************
Existing ranges
***************

Existing sequence can be used as ranges.
Any pair of iterators can be converted into a range.
But also, standard containers can be used straight away.

.. highlight:: cpp

Standard containers
===================

To use standard container types as ranges, say::

    #include "range/std.hpp"

and then, for example::

    using namespace range;

    // Initialise vector and tuple.
    int elements[] {5, 7, 9};
    std::vector <int> v (&elements [0], &elements [3]);
    std::tuple <int, std::string, float> t (6, "Hello", 7.5);

    // Is the range empty?
    assert (!empty (v));
    assert (!empty (t));

    // Find the size.
    assert (size (v) == 3);
    assert (size (t) == 3);

    // Retrieve the first element.
    assert (first (v) == 5);
    assert (first (t) == 6);

    // Remove one element from the back and then retrieve the next element.
    assert (first (drop (v, back), back) == 7);
    assert (first (drop (t, back), back) == "Hello");

    // Retrieve the element at position 2.
    assert (at_c <2> (v) == 9);
    assert (at (v, 2) == 9);
    assert (at_c <2> (t) == 7.5);

Optionals
=========

Another type of container is ``boost::optional``, or ``std::optional``, when that comes through.
To use this as a range, of length 0 or 1, ``range::view_optional`` must be used.
For example::

    using boost::optional;
    using namespace range;

    optional <int> none;
    optional <int> three (3);

    assert (empty (view_optional (none)));

    auto three_view = view_optional (three);
    assert (!empty (three_view));
    assert (size (three_view) == 1);
    assert (first (three_view) == 3);

.. doxygenvariable:: range::view_optional


Pairs of iterators
==================

.. doxygenvariable:: range::make_iterator_range
.. doxygenclass:: range::iterator_range
