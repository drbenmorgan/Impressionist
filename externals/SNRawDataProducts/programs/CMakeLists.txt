# Build programs
# - Helper function to uniformly set RPATH so that the programs
#   can find the main library
# - Don't seem to be able to use genexs in properties...
function(_snrtd_install_rpath target)
  if(APPLE)
    set_target_properties(${target} PROPERTIES
      INSTALL_RPATH "@executable_path/${CMAKE_INSTALL_BINDIR_TO_LIBDIR}"
    )
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set_target_properties(${target} PROPERTIES
      INSTALL_RPATH "$ORIGIN/${CMAKE_INSTALL_BINDIR_TO_LIBDIR}"
    )
  endif()
endfunction()

add_subdirectory(crd2rhd)
add_subdirectory(rhd2rtd)
add_subdirectory(rhd2root)
add_subdirectory(rtd2root)
