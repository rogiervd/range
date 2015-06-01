# Copyright 2014, 2015 Rogier van Dalen.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
