# Template for the configuration file for the Sphinx documentation generator.

# Change these to the correct values for your project.

root_doc = "index"
project = "Range library for C++"
copyright = "2021-2026 Rogier van Dalen"
version = "1.0.0"
release = "1.0.0"

# Add any Sphinx extension module names here, as strings.
# They also need to be added in ./BUILD.bazel to sphinx_build_binary(deps=...).

extensions = ["sphinx.ext.imgmath", "sphinx.ext.todo", "breathe"
# TODO "intersphinx"
]

pygments_style = "sphinx"
html_theme = "nature"
templates_path = ["_templates"]


source_suffix = ".rst"

# Pick up the Doxygen XML output from the build directory.
# Options for Breathe.
breathe_projects = {"range": "xml"}
breathe_default_project = "range"
