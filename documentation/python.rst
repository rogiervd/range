.. _python:

******
Python
******

The Range library supports lazy ranges, which maps relatively well to Python *iterators*.
The communication layer to allow this uses Boost.Python.

Using a Python iterator as a C++ range
======================================

It is often useful to pass in Python iterators as arguments to a C++ function.

TODO give a complete example.

.. doxygenclass:: range::python_range
.. doxygenclass:: range::python::convert_object_to_range

Exposing a C++ range as a Python iterator
=========================================

It is often useful to traverse a C++ range from Python.
For example, when a C++ function returns a range and is exposed to Python.

TODO give a complete example.

.. doxygenclass:: range::python::python_iterator
.. doxygenfunction:: range::python::initialise_iterator
.. doxygenfunction:: range::python::register_view

.. doxygenstruct:: range::python::return_view
.. doxygenstruct:: range::python::return_view_of_internal_reference

Exposing a C++ range as a Python tuple
======================================

It is often useful to convert a C++ tuple to a Python tuple.
This can be used for any C++ range that has a fixed number of elements.

TODO give a complete example.

.. doxygenfunction:: range::python::register_tuple
