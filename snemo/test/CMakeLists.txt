# CET infrastructure
cet_enable_asserts()
include(CetTest)

# Sanity compilation-time tests
add_executable(ext-sanity sanity.cc)
add_test(NAME ext-sanity COMMAND ext-sanity)

add_executable(ext-bayeux externals.cc)
target_compile_definitions(ext-bayeux PRIVATE IMP_TEST_BAYEUX)
target_link_libraries(ext-bayeux Bayeux::Bayeux)
add_test(NAME ext-bayeux COMMAND ext-bayeux)

add_executable(ext-gitconddb externals.cc)
target_compile_definitions(ext-gitconddb PRIVATE IMP_TEST_GITCONDDB IMP_TEST_REPO=${PROJECT_SOURCE_DIR})
target_link_libraries(ext-gitconddb GitCondDB::GitCondDB)
add_test(NAME ext-gitcondb COMMAND ext-gitconddb)

add_executable(ext-nlohmann externals.cc)
target_compile_definitions(ext-nlohmann PRIVATE IMP_TEST_NLOHMANN)
target_link_libraries(ext-nlohmann nlohmann_json::nlohmann_json)
add_test(NAME ext-nlohmann COMMAND ext-nlohmann)

