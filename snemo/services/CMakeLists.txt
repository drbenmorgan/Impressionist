simple_plugin(KernelService "service"
  Bayeux::Bayeux
  )

generate_binreloc(ResourceService)
simple_plugin(ResourceService "service"
  snemo_services_KernelService_service
  ResourceService_binreloc
  Bayeux::Bayeux
  )
