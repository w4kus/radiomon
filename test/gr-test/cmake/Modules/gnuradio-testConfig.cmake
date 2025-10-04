find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_TEST gnuradio-test)

FIND_PATH(
    GR_TEST_INCLUDE_DIRS
    NAMES gnuradio/test/api.h
    HINTS $ENV{TEST_DIR}/include
        ${PC_TEST_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_TEST_LIBRARIES
    NAMES gnuradio-test
    HINTS $ENV{TEST_DIR}/lib
        ${PC_TEST_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-testTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_TEST DEFAULT_MSG GR_TEST_LIBRARIES GR_TEST_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_TEST_LIBRARIES GR_TEST_INCLUDE_DIRS)
