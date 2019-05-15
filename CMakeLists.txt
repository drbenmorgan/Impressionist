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
include(externals/gitconddb.cmake)

# Sanity compilation-time tests
add_executable(cbt-sanity smoketest/sanity.cc)

add_executable(cbt-externals-bayeux smoketest/externals.cc)
target_compile_definitions(cbt-externals-bayeux PRIVATE IMP_TEST_BAYEUX)
target_link_libraries(cbt-externals-bayeux Bayeux::Bayeux)

add_executable(cbt-externals-gitconddb smoketest/externals.cc)
target_compile_definitions(cbt-externals-gitconddb PRIVATE IMP_TEST_GITCONDDB IMP_TEST_REPO=${PROJECT_SOURCE_DIR})
target_link_libraries(cbt-externals-gitconddb GitCondDB::GitCondDB)

add_executable(cbt-externals-nlohmann smoketest/externals.cc)
target_compile_definitions(cbt-externals-nlohmann PRIVATE IMP_TEST_NLOHMANN)
target_link_libraries(cbt-externals-nlohmann nlohmann_json::nlohmann_json)

# FHiCL
add_subdirectory(fcl)

