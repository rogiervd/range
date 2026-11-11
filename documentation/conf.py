# Template for the configuration file for the Sphinx documentation generator.

# Change these to the correct values for your project.

root_doc = "index"
project = "Range library for C++"
copyright = "2012-2026 Rogier van Dalen"
# The following variables are defined externally so they can be specified in
# a single location.
# Pass them to Sphinx with the -D option, e.g.:
#   sphinx-build -D version=1.2.3 -D release=1.2.3
# version = ...
# release = ...

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
