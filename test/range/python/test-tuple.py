# Copyright 2015 Rogier van Dalen.

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
