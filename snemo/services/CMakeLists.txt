simple_plugin(KernelService "service"
  Bayeux::Bayeux
  )

generate_binreloc(ResourceService)
simple_plugin(ResourceService "service"
  snemo_services_KernelService_service
  ResourceService_binreloc
  Bayeux::Bayeux
  )

# Add a post build to ResourceService target to
# place softlink next to ResourceService library
add_custom_command(TARGET snemo_services_ResourceService_service
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink "resources" "${CMAKE_CURRENT_SOURCE_DIR}/resources"
  WORKING_DIRECTORY $<TARGET_FILE_DIR:snemo_services_ResourceService_service>
  COMMENT "linking ResourceService resource tree"
  )
