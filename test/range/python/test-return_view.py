# Copyright 2014, 2015 Rogier van Dalen.

# This file is part of Rogier van Dalen's Range library for C++.

# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This test should run under Python 2 and 3 without modification.

from return_view_example import *

def test_count():
    for index1, index2 in enumerate (count (10)):
        assert (index1 == index2)
        if index1 == 10:
            assert (False)

def test_count2():
    for index1, index2 in enumerate (count2()):
        assert (index1 == index2)
        if index1 == 10:
            break

    # Of course count2 just keeps on counting - it isn't a great implementation.
    n = next (count2())
    assert (n == 11)

def test_return_internal_reference_1():
    iterator = []
    c = ContainerContainer()
    l = list (c.get_17_19())
    print (l)
    assert (l == [17, 19])

    t = tuple (c.get_tuple())
    print (t)
    assert (t == (13.5, "Great!", False))

    # The iterator should remain alive until after the reference to the object
    # that contains the actual elements has gone.
    iterator = c.get_17_19()
    del c
    return iterator

def test_return_internal_reference_2():
    iterator = test_return_internal_reference_1()

    # The ContainerContainer should secretly be kept alive so we have access to
    # the content of "iterator".
    l = list (iterator)
    print (l)
    assert (l == [17, 19])

    l = list (iterator)
    assert (l == [])

test_count()
test_count2()

test_return_internal_reference_1()
test_return_internal_reference_2()
