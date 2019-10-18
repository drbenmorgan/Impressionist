#include "snemo/services/VariantService.h"

#include "snemo/datamodel/MultiProperties.h"
#include "bayeux/datatools/configuration/variant_service.h"

#include <iostream>

namespace snemo {
VariantService::VariantService(fhicl::ParameterSet const&, art::ActivityRegistry& aRegistry)
    : resourceSvc_{art::ServiceHandle<ResourceService>()},
      variantSvc_{std::make_unique<datatools::configuration::variant_service>()}
{
  // Register callback...
  aRegistry.sPreBeginRun.watch(this, &VariantService::preBeginRun);
  variantSvc_->set_logging(datatools::logger::PRIO_TRACE);
  std::cout << "VariantService Constructing...\n";
}

VariantService::~VariantService()
{
  std::cout << "VariantService Destructing... \n";
}

void VariantService::preBeginRun(art::Run const& newRun)
{
  std::cout << "VariantService::preBeginRun called\n";
  // Need to consume metadata from Run (this uses a hard coded InputTag
  // for testing
  auto runMetaData = newRun.getValidHandle<MultiProperties>(art::InputTag{"SD"});
  runMetaData->tree_dump(std::cout);

  // In falaise the above is extarct from the input file via
  //
  // falaise::app::metadata_collector mc;
  // mc.set_input_data_file(flRecParameters.inputFile);
  // flRecParameters.inputMetadata = mc.get_metadata_from_data_file();
  //
  // so runMetaData should be equivalent to flRecParameters.inputMetadata
  // if we read things in correctly (the runMetaData multi_properties object
  // constructed in the same way as in input_module::get_metadata_store).
  //
  // What remains is *extremely* overwrought, but for variants the key thing
  // is how the "FLReconstructParameters.variantSubsystemParams" object is
  // filled. It's this we need to configure the variant service:
  // if (flRecParameters.variantSubsystemParams.is_active()) {
  //    variantService.configure(flRecParameters.variantSubsystemParams);
  //    variantService.start();
  // endif()
  //
  // In postprocessing metadata, only thing that's *actually* extracted is the
  // inputMetadata multiprops, and the experimentalSetupURN
  // The remaining postprocessing then does...
  //
  // Looking at actual flsimulate output as read by flreconstruct, with defaults
  // have
  /*
  Extracted metadata
  |-- Description  : Bayeux/dpp library's I/O module metadata store
  |-- Key label    : "name"
  |-- Entries      : [4]
  |   |-- Entry : "flsimulate"
  |   |   |-- Key        : "flsimulate"
  |   |   |-- Meta       : "flsimulate::section"
  |   |   `-- Properties : [12]
  |   |       |-- Description  : 'flsimulate basic system informations'
  |   |       |   |-- Short description : 'flsimulate basic system informations'
  |   |       |   `-- Auxiliary descriptions : <none>
  |   |       |-- Name : 'application'
  |   |       |   |-- Description  : 'The simulation application used to produce Monte Carlo data'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : 'flsimulate'
  |   |       |-- Name : 'application.version'
  |   |       |   |-- Description  : 'The version of the simulation application'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : '3.3.0'
  |   |       |-- Name : 'bayeux.version'
  |   |       |   |-- Description  : 'Bayeux version'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : '3.3.0'
  |   |       |-- Name : 'doDigitization'
  |   |       |   |-- Description  : 'Activate digitization'
  |   |       |   |-- Type  : boolean (scalar)
  |   |       |   `-- Value : 0
  |   |       |-- Name : 'doSimulation'
  |   |       |   |-- Description  : 'Activate simulation'
  |   |       |   |-- Type  : boolean (scalar)
  |   |       |   `-- Value : 1
  |   |       |-- Name : 'embeddedMetadata'
  |   |       |   |-- Description  : 'Metadata embedding flag'
  |   |       |   |-- Type  : boolean (scalar)
  |   |       |   `-- Value : 1
  |   |       |-- Name : 'experimentalSetupUrn'
  |   |       |   |-- Description  : 'Experimental setup URN'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : 'urn:snemo:demonstrator:setup:1.0'
  |   |       |-- Name : 'falaise.version'
  |   |       |   |-- Description  : 'Falaise version'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : '3.3.0'
  |   |       |-- Name : 'falaise.version.commit'
  |   |       |   |-- Description  : 'Falaise commit'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : ''
  |   |       |-- Name : 'falaise.version.state'
  |   |       |   |-- Description  : 'Falaise commit state'
  |   |       |   |-- Type  : boolean (scalar)
  |   |       |   `-- Value : 1
  |   |       |-- Name : 'numberOfEvents'
  |   |       |   |-- Description  : 'Number of simulated events'
  |   |       |   |-- Type  : integer (scalar)
  |   |       |   `-- Value : 1
  |   |       `-- Name : 'userProfile'
  |   |           |-- Description  : 'User profile'
  |   |           |-- Type  : string (scalar)
  |   |           `-- Value : 'normal'
  |   |-- Entry : "flsimulate.simulation"
  |   |   |-- Key        : "flsimulate.simulation"
  |   |   |-- Meta       : "flsimulate::section"
  |   |   `-- Properties : [2]
  |   |       |-- Description  : 'Simulation setup parameters'
  |   |       |   |-- Short description : 'Simulation setup parameters'
  |   |       |   `-- Auxiliary descriptions : <none>
  |   |       |-- Name : 'rngSeeding'
  |   |       |   |-- Description  : 'PRNG initial seeds'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : '{EG=729860916; MGR=193842407; SHPF=654878493; VG=599006936}'
  |   |       `-- Name : 'simulationSetupUrn'
  |   |           |-- Description  : 'Simulation setup URN'
  |   |           |-- Type  : string (scalar)
  |   |           `-- Value : 'urn:snemo:demonstrator:simulation:2.3'
  |   |-- Entry : "flsimulate.variantService"
  |   |   |-- Key        : "flsimulate.variantService"
  |   |   |-- Meta       : "flsimulate::section"
  |   |   `-- Properties : [2]
  |   |       |-- Description  : 'Variant setup'
  |   |       |   |-- Short description : 'Variant setup'
  |   |       |   `-- Auxiliary descriptions : <none>
  |   |       |-- Name : 'configUrn'
  |   |       |   |-- Description  : 'Variants setup configuration URN'
  |   |       |   |-- Type  : string (scalar)
  |   |       |   `-- Value : 'urn:snemo:demonstrator:simulation:2.3:variants'
  |   |       `-- Name : 'profileUrn'
  |   |           |-- Description  : 'Variants profile URN'
  |   |           |-- Type  : string (scalar)
  |   |           `-- Value : 'urn:snemo:demonstrator:simulation:2.3:variants:profiles:default'
  |   `-- Entry : "flsimulate.services"
  |       |-- Key        : "flsimulate.services"
  |       |-- Meta       : "flsimulate::section"
  |       `-- Properties : [1]
  |           |-- Description  : 'Services configuration'
  |           |   |-- Short description : 'Services configuration'
  |           |   `-- Auxiliary descriptions : <none>
  |           `-- Name : 'configUrn'
  |               |-- Description  : 'Services setup configuration URN'
  |               |-- Type  : string (scalar)
  |               `-- Value : 'urn:snemo:demonstrator:simulation:2.3:services'
  `-- Ordered entries      : [4]
      |-- Entry [rank=0] : "flsimulate"
      |-- Entry [rank=1] : "flsimulate.simulation"
      |-- Entry [rank=2] : "flsimulate.variantService"
      `-- Entry [rank=3] : "flsimulate.services"
  */
  // FLReconstruct setup parameters:
  // |-- logLevel                   = fatal
  // |-- userProfile                = normal
  // |-- numberOfEvents               = 0
  // |-- moduloEvents                 = 0
  // |-- experimentalSetupUrn         = urn:snemo:demonstrator:setup:1.0
  // |-- reconstructionPipelineUrn    =
  // |-- reconstructionPipelineConfig =
  // |-- reconstructionPipelineModule = pipeline
  // |-- variantConfigUrn             = urn:snemo:demonstrator:setup:1.0:variants
  // |-- variantProfileUrn            =
  // |-- variantSubsystemParams       = @falaise:config/snemo/demonstrator/setup/1.0/variants/repository.conf
  // |-- servicesSubsystemConfigUrn   = urn:snemo:demonstrator:setup:1.0:services
  // |-- servicesSubsystemConfig      = @falaise:config/snemo/demonstrator/setup/1.0/services.conf
  // |-- inputMetadataFile            =
  // |-- inputFile                    = test.brio
  // |-- outputMetadataFile           =
  // |-- embeddedMetadata             = true
  // `-- outputFile                   =
  // Variant service params:
  // |-- Label : <none>
  // |-- Logging : ''
  // |-- Configuration file : '@falaise:config/snemo/demonstrator/setup/1.0/variants/repository.conf'
  // |-- Additional registries : <none>
  // |-- Load profile : <none>
  // |-- Settings : <none>
  // |-- GUI : no
  // |-- TUI : no
  // |-- Store profile : <none>
  // |-- Reporting file : <none>
  // `-- Active : yes
  //
  // Which is all technically correct, but confusing as we dont appear to end up
  // with the same variant system, at least by URN/config file. Need to see what
  // happens with differing variant settings (because these would affect
  // geometry etc)
  // Certainly, appears from files that in particular, any variant settings are
  // *not* reapplied. As we don't have any large scale changes to geometry and
  // so on, this may not have huge effects, but..., magnetic field is. Would
  // depend on if anything in reco accesses magnetic field from things that
  // would be influenced by this, e.g. Geometry. In any cases, clearly a bug
  // because should be reconstituting exactly the same things as used in
  // a previous processing step.
  //
  // Also, looks like everything is hidden under the experimentalSetup tag
  // which hasn't changed, though things under it have. Need to try:
  //
  // 1. Use Falaise as patched to print out variants/etc to check exactly what
  // is used, and what is not, from different flsimulate runs.
  // 2. Going to need to identify any variants that would affect reconstruction
  // as we'd need to check how these are used in reconstruction
  // 3.

  // So means at least that here, we just need to create a dumb config object.

  // Prepare service
  variantSvc_->stop();
  // variantSvc_.configure(<datafromrun>
  variantSvc_->start();
}

} // namespace snemo

DEFINE_ART_SERVICE(snemo::VariantService)
