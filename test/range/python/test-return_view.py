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

from return_view_example import ContainerContainer, count, count2


def test_count():
    for index1, index2 in enumerate(count(10)):
        assert index1 == index2
        assert index1 < 10


def test_count2():
    for index1, index2 in enumerate(count2()):
        assert index1 == index2
        if index1 == 10:
            break

    # Of course count2 just keeps on counting - it isn't a great implementation.
    n = next(count2())
    assert n == 11


def test_return_internal_reference_1():
    c = ContainerContainer()
    assert list(c.get_17_19()) == [17, 19]
    assert tuple(c.get_tuple()) == (13.5, "Great!", False)


def test_return_internal_reference_2():
    # The iterator should remain alive until after the reference to the object
    # that contains the actual elements has gone.
    c = ContainerContainer()
    iterator = c.get_17_19()
    del c

    # The ContainerContainer should secretly be kept alive so we have access to
    # the content of "iterator".
    assert list(iterator) == [17, 19]
    assert list(iterator) == []
