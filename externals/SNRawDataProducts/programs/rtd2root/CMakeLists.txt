add_executable(rtd2root rtd2root.cxx
  rtd2root_data.cc
  rtd2root_data.h
  rtd2root_converter.cc
  rtd2root_converter.h
  )
target_link_libraries(rtd2root PRIVATE SNRawDataProducts)
_snrtd_install_rpath(rtd2root)

install(TARGETS rtd2root EXPORT SNRawDataProductsTargets DESTINATION ${CMAKE_INSTALL_BINDIR})

