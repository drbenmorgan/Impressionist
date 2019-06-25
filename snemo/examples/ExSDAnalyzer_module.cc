//! \file SDAnalyzer_module.cc
//! \brief Demo analyzer module with use of TFileService
//!
//! Largely adapted from art_root_io/test/TestTFileService_module.cc
//! It demonstrates the minimum needed to extract a data product and
//! use TFileService to product plots for per-Event data with recommended
//! multithreading setup (see also "Multithreaded processing" on the art wiki:
//! https://cdcvs.fnal.gov/redmine/projects/art/wiki
//!

#include "TH1.h"
#include "snemo/edm/StepHitCollection.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedAnalyzer.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Utilities/SharedResource.h"
#include "art_root_io/TFileService.h"
#include "fhiclcpp/ParameterSet.h"
#include <string>

namespace snemo {

  class SDAnalyzer : public art::SharedAnalyzer {
  public:
    explicit SDAnalyzer(fhicl::ParameterSet const& p,
                        art::ProcessingFrame const& frame);

  private:
    // Main method we must implement
    void analyze(art::Event const& e, art::ProcessingFrame const& frame);

    // Where the Root objects will be created from the TFileService,
    // also used as callback when TFileService switches files
    void initRootObjects();

    // Primary Input Tag roughly equivalent to "bank label" in dpp
    art::InputTag const sdBankLabel;

    // Token for data product we'll use. Helps art to validate that
    // we don't use data that hasn't been produced yet
    art::ProductToken<StepHitCollection> const ggHitsToken;

    // Can hold normal Root pointers. Will be created/owned by the
    // TFileService
    TH1F* geigerHitCounter{nullptr};
  };

  SDAnalyzer::SDAnalyzer(fhicl::ParameterSet const& p,
                         art::ProcessingFrame const& frame)
    : art::SharedAnalyzer{p}
      // Label hard coded, but can be parameter
    , sdBankLabel{"SD"}
    ,
    // Use consumes to mark what we consume
    ggHitsToken{consumes<StepHitCollection>({sdBankLabel.label(), "gg"})}
  {
    // Must serialize access to the TFileService
    serialize(art::SharedResource<art::TFileService>);

    // Get the service from the frame, and register a callback
    // for the file switch transition
    auto const fs = frame.serviceHandle<art::TFileService>();
    fs->registerFileSwitchCallback(this, &SDAnalyzer::initRootObjects);

    // Create our Root Objects
    initRootObjects();
  }

  void
  SDAnalyzer::initRootObjects()
  {
    // get the service
    art::ServiceHandle<art::TFileService> fs;

    // All Root objects for *this* module are created
    // by default in a TDirectory whose name matches
    // the module label in the FHiCL script.

    // Through we only have one product, show we can create
    // additional subdirectories if required:
    art::TFileDirectory geigerDir = fs->mkdir("geiger");

    // Create histo in that directory
    geigerHitCounter = geigerDir.make<TH1F>(
      "geiger_step_counter", "Number of Geiger Steps per Event", 100, 0., 100.);
  }

  void
  SDAnalyzer::analyze(art::Event const& e, art::ProcessingFrame const&)
  {
    // Extract the GeigerHC via its token
    auto const& ghc = e.getValidHandle(ggHitsToken);

    // Fill the histo
    geigerHitCounter->Fill(ghc->size());
  }

} // namespace snemo

DEFINE_ART_MODULE(snemo::SDAnalyzer)
