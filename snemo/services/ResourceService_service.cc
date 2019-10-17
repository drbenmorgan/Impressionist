#include "snemo/services/ResourceService.h"

#include "cetlib_except/exception.h"

#include "bayeux/bayeux.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/library_info.h"
#include "bayeux/datatools/utils.h"
#include "bayeux/datatools/urn_query_service.h"
#include "bayeux/datatools/urn_db_service.h"
#include "bayeux/datatools/urn_to_path_resolver_service.h"

#include "ResourceService_binreloc.h"

#include <iostream>

namespace  {
  const std::string resourceName {"falaise"};

  std::string getSelfDir() {
    BrInitError bErr;
    int err = br_init_lib(&bErr);
    char* selfDirCharStar = br_find_exe_dir(0);

    if (!err || selfDirCharStar == 0) {
      throw cet::exception("ResourceService", "unable to locate self via binreloc");
    }

    std::string selfDir {selfDirCharStar};
    free(selfDirCharStar);
    return selfDir;
  }

  // Helper function to
  datatools::properties propertiesFromPath(std::string const& filepath)
  {
    std::string resolvedPath = filepath;
    datatools::fetch_path_with_env(resolvedPath);
    datatools::properties props;
    props.read_configuration(resolvedPath);
    return props;
  }
} // namespace

namespace snemo {
struct ResourceService::URNServices {
  const std::string dbSvcConfig_ {"@falaise:urn_db.conf"};
  datatools::urn_db_service dbSvc_;
  const std::string resolverSvcConfig_ {"@falaise:urn_resolver.conf"};
  datatools::urn_to_path_resolver_service resolverSvc_;

  void start()
  {
    // DB
    dbSvc_.set_name("flSetupDb");
    dbSvc_.initialize_standalone(propertiesFromPath(dbSvcConfig_));
    dbSvc_.kernel_push();

    // Resolver
    resolverSvc_.set_name("flResourceResolver");
    resolverSvc_.initialize_standalone(propertiesFromPath(resolverSvcConfig_));
    resolverSvc_.kernel_push();
  }

  void stop()
  {
    // DB
    dbSvc_.kernel_pop();
    dbSvc_.reset();

    // Resolver
    resolverSvc_.kernel_pop();
    resolverSvc_.reset();
  }

  void restart()
  {
    this->stop();
    this->start();
  }
};

ResourceService::ResourceService(fhicl::ParameterSet const&, art::ActivityRegistry&)
    : kernelSvc_{art::ServiceHandle<KernelService>()},
      urnSvcs_{std::make_unique<URNServices>()}
{
  std::cout << "ResourceService Constructing...\n";

  std::string resourceRoot = getSelfDir();
  std::cout << "Self-location: " << resourceRoot << std::endl;

  // Mount resources
  auto& kernel = datatools::kernel::instance();
  auto& resourceDB = kernel.grab_library_info_register();
  resourceDB.registration(::resourceName, ::resourceName, "version")
      .store_string(datatools::library_info::keys::install_resource_dir(), resourceRoot + "/resources");

  // Set the URN DB/Resolvers
  // MUST be done after resources mounted because config comes from resource
  urnSvcs_->start();
}

ResourceService::~ResourceService()
{
  // Remove URN DB/Resolver
  urnSvcs_->stop();

  // Unmount resources
  auto& kernel = datatools::kernel::instance();
  auto& resourceDB = kernel.grab_library_info_register();
  resourceDB.unregistration(::resourceName);
  std::cout << "ResourceService Destructing... \n";
}

} // namespace snemo

DEFINE_ART_SERVICE(snemo::ResourceService)
