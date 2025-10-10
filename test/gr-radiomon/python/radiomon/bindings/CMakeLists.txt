# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT radiomon_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT radiomon_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND radiomon_python_files
    grfirfilt_python.cc
    grfirdecim_python.cc
    grfirinterp_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(radiomon
   ../../..
   gr::radiomon
   "${radiomon_python_files}")

# copy in bindings .so file for use in QA test module
add_custom_target(
  copy_bindings_for_tests ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/*.so"
    ${CMAKE_BINARY_DIR}/test_modules/gnuradio/radiomon/
  DEPENDS radiomon_python)

install(TARGETS radiomon_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/radiomon COMPONENT pythonapi)
