
add_executable(snfee-crd2rhd crd2rhd.cxx)
target_link_libraries(snfee-crd2rhd
  snfee
  Bayeux::Bayeux
  )
set_target_properties(snfee-crd2rhd
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  )
install(FILES
  ${CMAKE_BINARY_DIR}/snfee-crd2rhd 
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end