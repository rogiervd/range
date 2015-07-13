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

BRANCH=${1}
shift

# If the branch is "master", check out the master branch of range-test.
# Otherwise, check out the develop branch of range-test.
if [ x${BRANCH} != xmaster ]
then
    BRANCH=develop
fi

(
    # Check out the "master" branch of "range-test" from GitHub, in the parent
    # directory.

    cd ../
    git clone git://github.com/rogiervd/range-test.git
    cd range-test
    echo Checking out branch ${BRANCH} of range-test...
    git checkout ${BRANCH}
    git submodule init
    git submodule update
    # Then replace the "range" submodule with the one in ../range.
    rm -r range
    ln -s ../range

    # Run the tests.
    bjam "$@"

)
