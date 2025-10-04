find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_RADIOMON gnuradio-radiomon)

FIND_PATH(
    GR_RADIOMON_INCLUDE_DIRS
    NAMES gnuradio/radiomon/api.h
    HINTS $ENV{RADIOMON_DIR}/include
        ${PC_RADIOMON_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_RADIOMON_LIBRARIES
    NAMES gnuradio-radiomon
    HINTS $ENV{RADIOMON_DIR}/lib
        ${PC_RADIOMON_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-radiomonTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_RADIOMON DEFAULT_MSG GR_RADIOMON_LIBRARIES GR_RADIOMON_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_RADIOMON_LIBRARIES GR_RADIOMON_INCLUDE_DIRS)
