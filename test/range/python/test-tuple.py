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

# This test should run under Python 2 and 3 without modification.

from tuple_example import *

t = getDoubleString()
assert (type (t) == type ((4,5)))
print (t)
assert (t == (6.5, "Excellent."))

t = getIntBoolString (27, 5.25)
assert (type (t) == type ((4,5)))
print (t)
assert (t == (27, 5.25, "Wow!"))

t = getTwice (27, 5.25)
assert (type (t) == type ((4,5)))
print (t)
assert (t == (54, 10.5, "Wow!Wow!"))
