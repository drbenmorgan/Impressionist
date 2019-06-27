add_executable(crd2rhd crd2rhd.cxx
  calo_hit_parser.cc
  calo_hit_parser.h
  raw_hit_reader.cc
  raw_hit_reader.h
  raw_record_parser.cc
  raw_record_parser.h
  raw_run_header.cc
  raw_run_header.h
  tracker_hit_parser.cc
  tracker_hit_parser.h
  )
target_link_libraries(crd2rhd PUBLIC SNRawDataProducts)
_snrtd_install_rpath(crd2rhd)

install(TARGETS crd2rhd EXPORT SNRawDataProductsTargets DESTINATION ${CMAKE_INSTALL_BINDIR})
