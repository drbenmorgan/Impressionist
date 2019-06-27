
add_executable(snfee-rhd2rtd rhd2rtd.cxx)
target_link_libraries(snfee-rhd2rtd
  snfee
  Bayeux::Bayeux
  )
set_target_properties(snfee-rhd2rtd
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  )
install(FILES
    ${CMAKE_BINARY_DIR}/snfee-rhd2rtd 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    )
