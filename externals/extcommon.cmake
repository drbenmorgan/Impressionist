# - Shared variables/structure for externals
if(NOT __EXTERNALS_COMMON_INCLUDED)
  set(__EXTERNALS_COMMON_INCLUDED TRUE)
else()
  return()
endif()

include(ExternalProject)
include(ProcessorCount)

ProcessorCount(N)
if(NOT N EQUAL 0)
  set(PARALLEL_ARG "-j${N}")
endif()

set_property(DIRECTORY PROPERTY EP_PREFIX "${PROJECT_BINARY_DIR}/IMPExternals-BUILD")

set(EXTERNALS_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/IMPExternals)
set(EXTERNALS_INSTALL_LIBDIR ${EXTERNALS_INSTALL_PREFIX}/lib)
set(EXTERNALS_INSTALL_INCLUDEDIR ${EXTERNALS_INSTALL_PREFIX}/include)

foreach(_extdir PREFIX LIBDIR INCLUDEDIR)
  if(NOT EXISTS ${EXTERNALS_INSTALL_${_extdir}})
    file(MAKE_DIRECTORY ${EXTERNALS_INSTALL_${_extdir}})
  endif()
endforeach()

