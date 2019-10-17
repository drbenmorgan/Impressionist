#ifndef SNEMO_SERVICES_KERNELSERVICE_HH
#define SNEMO_SERVICES_KERNELSERVICE_HH

//! This service's sole purpose is to guarantee construction
//! of the Falaise/Bayeux singleton "kernel"
//! This is used as "backdoor" between variant/urn services and
//! several datatools classe that access these (properties/utils)
//! It's thus just for compatibility with that implementation

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace snemo {
  class KernelService {
   public:
    KernelService(fhicl::ParameterSet const&, art::ActivityRegistry&);
    ~KernelService();
  };
} // namespace snemo

// Macro to register with frameworl
DECLARE_ART_SERVICE(snemo::KernelService, LEGACY)

#endif
