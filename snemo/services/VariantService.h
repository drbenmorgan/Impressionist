#ifndef SNEMO_SERVICES_VARIANTSERVICE_HH
#define SNEMO_SERVICES_VARIANTSERVICE_HH

//! Service to manage Bayeux's "Variant" system

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "snemo/services/ResourceService.h"

#include <memory>

// Forward declare hidden service
namespace datatools {
namespace configuration {
class variant_service;
}
}

namespace snemo {
  class VariantService {
   public:
    VariantService(fhicl::ParameterSet const&, art::ActivityRegistry&);
    ~VariantService();

    //! Called by Art on each new Run
    void preBeginRun(art::Run const&);

   private:
    // Depend on ResourceService by holding an
    art::ServiceHandle<ResourceService> resourceSvc_;

    // Must manage the underlying Bayeux variant service
    std::unique_ptr<datatools::configuration::variant_service> variantSvc_;
  };
} // namespace snemo

// Macro to register with frameworl
DECLARE_ART_SERVICE(snemo::VariantService, LEGACY)

#endif
