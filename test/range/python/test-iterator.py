# SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
#
# SPDX-License-Identifier: Apache-2.0

import iterator_example


def test_doubles():
    assert list(iterator_example.getDoubles()) == [3.5, 7.25]


def test_set_first_to_doubles():
    l = [None]
    iterator_example.setFirstToDoubles(l)
    assert list(l[0]) == [3.5, 7.25]


def test_tuple():
    assert list(iterator_example.getTuple()) == [6, "hello", 17.5]


def test_optional():
    assert list(iterator_example.getOptional()) == [True]
