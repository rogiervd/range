# SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
#
# SPDX-License-Identifier: Apache-2.0

import tuple_example


def test_double_string():
    t = tuple_example.getDoubleString()
    assert type(t) == tuple
    assert t == (6.5, "Excellent.")


def test_int_float_string():
    t = tuple_example.getIntBoolString(27, 5.25)
    assert type(t) == tuple
    assert t == (27, 5.25, "Wow!")


def test_twice():
    t = tuple_example.getTwice(27, 5.25)
    assert type(t) == tuple
    assert t == (54, 10.5, "Wow!Wow!")
