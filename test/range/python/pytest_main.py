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
    # Use this directory as pytest's root directory, and pass the test files
    # as absolute paths. Otherwise pytest may choose a root directory higher
    # up (on Windows, Bazel's execution root, where it finds pyproject.toml)
    # and look at files there that other build actions create and delete
    # concurrently. Also do not write a cache there.
    test_files = [os.path.abspath(argument) for argument in sys.argv[1:]]
    sys.exit(
        pytest.main(
            [
                "--import-mode=importlib",
                "--rootdir=" + here,
                "-p",
                "no:cacheprovider",
            ]
            + test_files
        )
    )
