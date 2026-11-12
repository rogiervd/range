# SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
#
# SPDX-License-Identifier: Apache-2.0

import os
import sys

import pytest

if __name__ == "__main__":
    # The Python extensions are built into this directory.
    # Bazel makes it a package (with an implicit __init__.py), which stops
    # pytest from adding it to the path, so add it explicitly.
    here = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, here)
    # Keep pytest within this directory. Otherwise, on Windows, it finds
    # pyproject.toml in Bazel's execution root, and collects that directory
    # and every directory in between, while other build actions create and
    # delete files there. Also do not write a cache.
    test_files = [os.path.abspath(argument) for argument in sys.argv[1:]]
    sys.exit(
        pytest.main(
            [
                "--import-mode=importlib",
                "--rootdir=" + here,
                "--confcutdir=" + here,
                "-p",
                "no:cacheprovider",
            ]
            + test_files
        )
    )
