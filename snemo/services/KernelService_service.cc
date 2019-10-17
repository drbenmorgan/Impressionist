#include "snemo/services/KernelService.h"

#include "bayeux/bayeux.h"
#include "bayeux/datatools/kernel.h"

#include <iostream>

namespace snemo {

KernelService::KernelService(fhicl::ParameterSet const&, art::ActivityRegistry&)
{
  std::cout << "KernelService Constructing...\n";
  // Wrap Bayeux initialization because we need resources from various Bayeux's modules:
  bayeux::initialize(0,0,0);
  datatools::kernel::instance().set_logging(datatools::logger::PRIO_TRACE);
}

KernelService::~KernelService()
{
  bayeux::terminate();
  std::cout << "KernelService Destructing... \n";
}

} // namespace snemo

DEFINE_ART_SERVICE(snemo::KernelService)
