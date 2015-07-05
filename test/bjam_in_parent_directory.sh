#!/bin/bash

# Run this from the root directory of the "range" repository.

# This script will generate a directory ../range-test and not remove it!
# So watch out where you run it.

# Run the tests by using this repository as submodule of the "range-test"
# repository.

# This is necessary because this repository cannot be tested by itself.


# Travis CI looks for this line.
set -ev

set -o nounset
set -o errexit

(
    # Check out the "master" branch of "range-test" from GitHub, in the parent
    # directory.

    cd ../
    git clone git://github.com/rogiervd/range-test.git
    cd range-test
    git checkout master
    git submodule init
    git submodule update
    # Then replace the "range" submodule with the one in ../range.
    rm -r range
    ln -s ../range

    # Run the tests.
    bjam "$@"

)
