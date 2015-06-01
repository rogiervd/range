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

from iterator_example import *

l = list (getDoubles())
print (l)
assert (l == [3.5, 7.25])

l = [None]
setFirstToDoubles (l)
assert (list (l[0]) == [3.5, 7.25])

l = list (getTuple())
print (l)
assert (l == [6, "hello", 17.5])

l = list (getOptional())
print (l)
assert (l == [True])
