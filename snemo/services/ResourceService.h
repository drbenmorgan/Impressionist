#ifndef SNEMO_SERVICES_RESOURCESERVICE_HH
#define SNEMO_SERVICES_RESOURCESERVICE_HH

//! Service to manage Falaise's system services

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "snemo/services/KernelService.h"

#include <memory>

namespace snemo {
  class ResourceService {
   public:
    ResourceService(fhicl::ParameterSet const&, art::ActivityRegistry&);
    ~ResourceService();

   private:
    // Depend on KernelService by depending on it
    art::ServiceHandle<KernelService> kernelSvc_;

    // Need to hold "behind the scenes" Datatools services
    struct URNServices;
    std::unique_ptr<URNServices> urnSvcs_;
  };
} // namespace snemo

// Macro to register with frameworl
DECLARE_ART_SERVICE(snemo::ResourceService, LEGACY)

#endif
