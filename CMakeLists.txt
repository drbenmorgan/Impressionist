cmake_minimum_required(VERSION 3.9)
project(impressionist)

# Standard UPS/Cetbuildtools stanza (cargo-culted from cetlib)
find_package(cetbuildtools 7 REQUIRED)
include(CetCMakeEnv)
cet_cmake_env()
cet_set_compiler_flags(
  DIAGS VIGILANT WERROR NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  )

# Ignore CET_SANITIZE... for now

# Build externals, because we don't have a UPS product for them
# It also helps test against the newer compilers/standards required
# - Mock find_package to allow mutual find/use of externals
macro(find_package)
  if(NOT "${ARGV0}" MATCHES "Bayeux")
    _find_package(${ARGV})
  endif()
endmacro()
include(externals/bayeux.cmake)
include(externals/gitconddb.cmake)

# RDM lib, plus helper variable because rootcling doesn't grok
# CMake genexes
add_subdirectory(externals/SNRawDataProducts)
set(SNRawDataProducts_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/externals/SNRawDataProducts)


# Find directly needed packages
find_package(art 3 REQUIRED)
find_package(canvas_root_io REQUIRED)
find_package(art_root_io REQUIRED)
find_package(ROOT REQUIRED)


# Set them up...
include(ArtDictionary)
include(BuildPlugins)

# SNemo
add_subdirectory(snemo)

# FHiCL
add_subdirectory(fcl)

