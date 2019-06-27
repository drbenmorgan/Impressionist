cmake_minimum_required(VERSION 3.3)
project(SNRawDataProducts VERSION 0.1.0)

# Settings and Modules
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)
set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

include(GNUInstallDirs)
file(RELATIVE_PATH CMAKE_INSTALL_BINDIR_TO_LIBDIR
  "${CMAKE_INSTALL_FULL_BINDIR}"
  "${CMAKE_INSTALL_FULL_LIBDIR}"
  )

# Direct Dependencies
find_package(Bayeux 3.3.0 REQUIRED)
# A hack until
# - root_generate_dictionary support usage requirements/genexs
# - Bayeux sets a variable for its include path
if(NOT Bayeux_INCLUDE_DIRS)
  # NB assumes a normal install
  find_path(Bayeux_INCLUDE_DIRS bayeux/bayeux.h
    PATHS "${Bayeux_DIR}/../../../"
    PATH_SUFFIXES include
    NO_DEFAULT_PATH)
endif()
if(Bayeux_INCLUDE_DIRS)
    list(APPEND Bayeux_INCLUDE_DIRS "${Bayeux_INCLUDE_DIRS}/bayeux")
endif()

# Bayeux will refind Boost, but we need an extra component
find_package(Boost REQUIRED date_time)

# Need ROOT for dictionary generation
find_package(ROOT 6.12 REQUIRED)

# Need threads for the rhd2rtd program
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# - Library build
# Add the Boost/Root dictionaries into the library for now
include(${ROOT_DIR}/modules/RootNewMacros.cmake)
# Need incdirs until root_generate_dictionary support usage requirements/genexs
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Bayeux_INCLUDE_DIRS})
root_generate_dictionary(SNRawDataProducts_dict
  # Don't add headers explicitly or their buildtime paths end up in the dict
  # Use #include in the linkdef to bring in what's needed.
  MODULE SNRawDataProducts
  LINKDEF ${PROJECT_SOURCE_DIR}/snfee/root_dict_linkdef.h
  OPTIONS "-noIncludePaths")

# The library itself
add_library(SNRawDataProducts SHARED
  # Data Model
  snfee/data/calo_hit_record.cc
  snfee/data/calo_hit_record.h
  snfee/data/channel_id.cc
  snfee/data/channel_id.h
  snfee/data/has_trigger_id_interface.h
  snfee/data/raw_trigger_data.cc
  snfee/data/raw_trigger_data.h
  snfee/data/run_info-serial.h
  snfee/data/run_info.cc
  snfee/data/run_info.h
  snfee/data/tracker_hit_record.cc
  snfee/data/tracker_hit_record.h
  snfee/data/trigger_record.cc
  snfee/data/trigger_record.h
  snfee/data/utils.h
  snfee/data/RRawTriggerData.cc
  snfee/data/RRawTriggerData.h
  snfee/data/rtdReformater.cc
  snfee/data/rtdReformater.h
  snfee/geometry.h
  snfee/model/feb_constants.cc
  snfee/model/feb_constants.h
  snfee/model/utils.cc
  snfee/model/utils.h
  snfee/utils.cc
  snfee/utils.h
  # Boost.Serialization File Reader/Writers
  snfee/io/multifile_data_reader.cc
  snfee/io/multifile_data_reader.h
  snfee/io/multifile_data_writer.cc
  snfee/io/multifile_data_writer.h
  # Boost.Serialization, Root dictionaries
  snfee/boost_dict.cc
  ${CMAKE_CURRENT_BINARY_DIR}/SNRawDataProducts_dict.cxx
  )

target_include_directories(SNRawDataProducts PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(SNRawDataProducts PUBLIC Bayeux::Bayeux Boost::date_time ROOT::RIO)

# Configure build time ROOT setup script
configure_file("setupSNRawDataProducts.C.in" "setupSNRawDataProducts.C" @ONLY)

# Programs
add_subdirectory(programs)

# Examples
add_subdirectory(examples)

# Installation
# - Interface
install(TARGETS SNRawDataProducts
  EXPORT SNRawDataProductsTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

# PCM/rootmap, little awkward because of deriving name:
#  - <libname>.ext
#    - <libname>.rootmap
#    - <libname>_rdict.pcm
# - For now, we *know* that prefix is "lib" so use directly
set(ROOTMAP_FILE "${CMAKE_CURRENT_BINARY_DIR}/libSNRawDataProducts.rootmap")
set(PCM_FILE "${CMAKE_CURRENT_BINARY_DIR}/libSNRawDataProducts_rdict.pcm")
install(FILES ${ROOTMAP_FILE} ${PCM_FILE}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

install(DIRECTORY snfee
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h"
  PATTERN "elsewhere" EXCLUDE
  PATTERN "*-serial.h" EXCLUDE
  PATTERN "root_dict_linkdef.h" EXCLUDE
  )

# - Support files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
  )
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )
install(EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

