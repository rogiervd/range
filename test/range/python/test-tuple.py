# Copyright 2015 Rogier van Dalen.

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
