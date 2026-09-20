import os
import sys

import pytest

if __name__ == "__main__":
    # The Python extensions are built into this directory.
    # Bazel makes it a package (with an implicit __init__.py), which stops
    # pytest from adding it to the path, so add it explicitly.
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    sys.exit(pytest.main(["--import-mode=importlib"] + sys.argv[1:]))
