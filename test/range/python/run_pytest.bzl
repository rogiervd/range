"""
Utility to define a pytest test.
Like a normaly pytest test, the test does not require a main() function, since
pytest_main.py defines it.
"""

load("@rules_python//python:py_test.bzl", "py_test")

def run_pytest(name, size = "small", extra_deps = []):
    """Run a pytest test.

    This is a wrapper around py_test that adds pytest_main.py to the sources
    and sets the main to pytest_main.py, so that the test file does not need
    to have a main() function.
    """
    test_file = name + ".py"
    py_test(
        name = name,
        size = size,
        srcs = ["//test:pytest_main.py", test_file],
        args = ["$(location %s)" % test_file],
        main = "//test:pytest_main.py",
        deps = [
            "//:tooling_demo_nanobind",
            "@tooling_demo_nanobind_pypi//pytest",
        ] + extra_deps,
    )
