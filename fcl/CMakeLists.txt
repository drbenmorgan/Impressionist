# Configure/install FHiCL files
# NB: does *not* install a C-headers like hierarchy....
#install_fhicl(SUBDIRS snemo)

# Try and make C-Header like hierarchy work for builddir
add_custom_target(link-fcl ALL
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/snemo snemo
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/examples examples
  )
