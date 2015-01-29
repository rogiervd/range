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

from iterator_example import *

l = list (getDoubles())
print (l)
assert (l == [3.5, 7.25])

l = [None]
setFirstToDoubles (l)
assert (list (l[0]) == [3.5, 7.25])
