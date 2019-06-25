add_library(snemo_edm SHARED
  event_header.h
  event_header.ipp
  event_header.cc
  timestamp.h
  timestamp.ipp
  timestamp.cc
  bayeux_dict.cc
  )
target_include_directories(snemo_edm
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  )
target_link_libraries(snemo_edm
  PUBLIC
    Bayeux::Bayeux
    Boost::serialization
    ROOT::Core
  )

# Create the Art dictionary so we can store Bayeux types directly
# in ROOT (likely also need ROOT_INCLUDE_DIRS at runtime to avoid
# "Cling autoload" issues.
include_directories(${Bayeux_INCLUDE_DIRS})
art_dictionary(DICTIONARY_LIBRARIES Bayeux::Bayeux)


# Now the module
simple_plugin(BrioInput "source" Bayeux::Bayeux snemo_edm)
