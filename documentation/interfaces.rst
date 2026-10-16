.. _interfaces:

**********
Interfaces
**********

Linking the lifetime of a container to a range
==============================================

It is often useful, especially when returning ranges, to make a container and
then to return a view of it.
Often the container can be destructed once the last copy of the view goes out
of scope.

.. doxygenvariable:: range::view_shared

Type erasure
============

Sometimes it is irrelevant what the type of a range is, for example at API
boundaries.
:cpp:class:`range::any_range` knows what the type of the elements is, and what
the range can do, but hides what the actual type of the range is.

.. doxygenvariable:: range::make_any_range
.. doxygenclass:: range::any_range
