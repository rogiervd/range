# SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
#
# SPDX-License-Identifier: Apache-2.0

"""
Make the Python library files available, for linking on Windows.
"""

def _python_libs_impl(ctx):
    py_cc_toolchain = ctx.toolchains["@rules_python//python/cc:toolchain_type"].py_cc_toolchain
    files = []
    if py_cc_toolchain.libs:
        cc_info = py_cc_toolchain.libs.providers_map["CcInfo"]
        for linker_input in cc_info.linking_context.linker_inputs.to_list():
            for library in linker_input.libraries:
                file = (library.interface_library or library.static_library or
                        library.dynamic_library)
                if file and not file.path.endswith(".dll"):
                    files.append(file)
    return [DefaultInfo(files = depset(files))]

python_libs = rule(
    implementation = _python_libs_impl,
    toolchains = ["@rules_python//python/cc:toolchain_type"],
    doc = """
The library files (e.g. python312.lib) of the current Python toolchain.

On Windows, Python extensions must link with these.
rules_python declares them with system_provided = True, so linking with
@rules_python//python/cc:current_py_cc_libs does not pass them to the linker.
This is how rules_python's own py_extension deals with this.
""",
)
