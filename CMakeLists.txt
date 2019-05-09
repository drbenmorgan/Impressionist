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
include(externals/bayeux.cmake)

# Sanity checks
add_executable(cbt-sanity smoketest/sanity.cc)
add_executable(cbt-externals smoketest/externals.cc)
target_link_libraries(cbt-externals Bayeux::Bayeux)

# FHiCL
add_subdirectory(fcl)

