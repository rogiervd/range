# SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
#
# SPDX-License-Identifier: Apache-2.0

"""Make the module version from MODULE.bazel available to BUILD files."""

# This only returns a value and does not instantiate any rules, so it does not
# need the "name" argument that buildifier expects of macros.
# buildifier: disable=unnamed-macro
def module_version():
    """Returns the version declared in module() in MODULE.bazel.

    native.module_version() can only be called from a .bzl file, so BUILD
    files call this function instead.
    """
    return native.module_version()
