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

import pytest

import python_range_example as example


def yield_empty():
    if False:
        yield 3


def yield_6_25_8_5():
    yield 6.25
    yield 8.5


class TestError(Exception):
    pass


def yield_6_25_error():
    yield 6.25
    raise TestError()


def yield_5_hello():
    yield 5
    yield "hello"


def empty_iterables():
    return [
        [],
        (),
        yield_empty(),
        [i for i in [3, 4] if i is None],
        (i for i in [3, 4] if i is None),
    ]


def iterables_6_25_8_5():
    return [
        [6.25, 8.5],
        (6.25, 8.5),
        (i + 1 for i in [5.25, 7.5]),
        yield_6_25_8_5(),
    ]


def iterables_5_hello():
    return [[5, "hello"], (5, "hello"), yield_5_hello()]


def test_empty():
    for iterable in empty_iterables():
        example.check_empty(iterable)


def test_empty_typed():
    for iterable in empty_iterables():
        example.check_empty_2(iterable)


@pytest.mark.parametrize(
    "check",
    [
        "check_6_25_8_5",
        "check_6_25_8_5_chop",
        "check_6_25_8_5_chop_in_place",
    ],
)
def test_6_25_8_5(check):
    for iterable in iterables_6_25_8_5():
        getattr(example, check)(iterable)


@pytest.mark.parametrize(
    "check",
    [
        "check_5_hello_untyped",
        "check_5_hello_typed",
        "check_5_hello_overtyped",
    ],
)
def test_5_hello(check):
    for iterable in iterables_5_hello():
        getattr(example, check)(iterable)


def test_nested():
    example.check_hello_5_bye_27([("hello", 5), ("bye", 27)])


def test_none_element():
    example.check_17_None_hi([17, None, "hi"])


def test_not_iterable():
    with pytest.raises(TypeError):
        example.check_6_25_8_5(5)


def test_exception_in_iterator():
    with pytest.raises(TestError):
        example.check_6_25_8_5(yield_6_25_error())


def test_return_something():
    assert example.test_return_something() == 1
