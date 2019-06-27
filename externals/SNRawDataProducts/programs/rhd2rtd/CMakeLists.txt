add_executable(rhd2rtd rhd2rtd.cxx
  rhd_record.cc
  rhd_record.h
  rhd_sorter.cc
  rhd_sorter.h
  rtd_record.cc
  rtd_record.h
  builder.cc
  builder.h
  builder_config.cc
  builder_config.h
  )
target_link_libraries(rhd2rtd PRIVATE SNRawDataProducts Threads::Threads)
_snrtd_install_rpath(rhd2rtd)

install(TARGETS rhd2rtd EXPORT SNRawDataProductsTargets DESTINATION ${CMAKE_INSTALL_BINDIR})
