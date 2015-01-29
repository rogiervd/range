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

from python_range_example import *

def yield_empty():
    if False:
        yield 3

def yield_6_25_8_5():
    yield 6.25
    yield 8.5

class TestError (Exception):
    pass

def yield_6_25_error():
    yield 6.25
    raise TestError()

def yield_5_hello():
    yield 5
    yield "hello"

check_empty ([])
check_empty (())
check_empty (yield_empty())
check_empty ([i for i in [3,4] if i == None])
check_empty (i for i in [3,4] if i == None)

check_empty_2 ([])
check_empty_2 (())
check_empty_2 (yield_empty())
check_empty_2 ([i for i in [3,4] if i == None])
check_empty_2 (i for i in [3,4] if i == None)

check_6_25_8_5 ([6.25, 8.5])
check_6_25_8_5 ((6.25, 8.5))
check_6_25_8_5 (i + 1 for i in [5.25, 7.5])
check_6_25_8_5 (yield_6_25_8_5())

check_6_25_8_5_chop ([6.25, 8.5])
check_6_25_8_5_chop ((6.25, 8.5))
check_6_25_8_5_chop (i + 1 for i in [5.25, 7.5])
check_6_25_8_5_chop (yield_6_25_8_5())

check_6_25_8_5_chop_in_place ([6.25, 8.5])
check_6_25_8_5_chop_in_place ((6.25, 8.5))
check_6_25_8_5_chop_in_place (i + 1 for i in [5.25, 7.5])
check_6_25_8_5_chop_in_place (yield_6_25_8_5())

check_5_hello_untyped ([5, 'hello'])
check_5_hello_untyped ((5, 'hello'))
check_5_hello_untyped (yield_5_hello())

check_5_hello_typed ([5, 'hello'])
check_5_hello_typed ((5, 'hello'))
check_5_hello_typed (yield_5_hello())

check_5_hello_overtyped ([5, 'hello'])
check_5_hello_overtyped ((5, 'hello'))
check_5_hello_overtyped (yield_5_hello())

check_hello_5_bye_27 ([
    ('hello', 5),
    ('bye', 27),
    ])

check_17_None_hi ([17, None, "hi"]);

# Break things.

# Check that iter(5) throws a TypeError.
try:
    iter (5)
except TypeError:
    pass

# This should throw the same.
try:
    check_6_25_8_5 (5)
except TypeError:
    pass

# Throw some random error.
try:
    check_6_25_8_5 (yield_6_25_error())
except TestError:
    pass

v = test_return_something()
assert (v == 1)
