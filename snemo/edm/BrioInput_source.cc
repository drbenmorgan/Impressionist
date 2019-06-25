/// Example input source, see:
/// https://cdcvs.fnal.gov/redmine/projects/art/wiki/Writing_your_own_input_sources
/// for further details

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"

// Falaise/Bayeux
#include "bayeux/brio/reader.h"
#include "bayeux/datatools/things.h"
#include "bayeux/mctools/simulated_data.h"

#include "snemo/edm/GenBBPrimaryEvent.h"
#include "snemo/edm/MultiProperties.h"
#include "snemo/edm/StepHitCollection.h"
#include "snemo/edm/event_header.h"

// Forward declare implementation of input source
namespace snemo {
  class BrioInputSourceDriver;
}

class snemo::BrioInputSourceDriver {
public:
  // Required constructor
  BrioInputSourceDriver(fhicl::ParameterSet const&,
                        art::ProductRegistryHelper&,
                        art::SourceHelper const&);

  // Required member function
  void readFile(std::string const& filename, art::FileBlock*& fb);

  // Required member function
  bool readNext(art::RunPrincipal const* const inR,
                art::SubRunPrincipal const* const inSR,
                art::RunPrincipal*& outR,
                art::SubRunPrincipal*& outSR,
                art::EventPrincipal*& outE);

  // Required member function
  void closeCurrentFile();

private:
  // Return true if the supplied reader has required stores
  bool validateSchema(const brio::reader& r) const;

private:
  art::SourceHelper const&
    srcHelper_;         ///< {Run,SubRun,Event}Principal construction helper
  brio::reader bInput_; ///< Reader for BRIO files
  std::string const GI_STORE{"GI"};
  std::string const ER_STORE{"ER"};
  std::set<std::string> const stepHitCategories{"gg", "calo", "xcalo", "gveto"};
  std::string const outputLabel{"SD"}; ///< Matches expectation
};

// Implementation of the driver
snemo::BrioInputSourceDriver::BrioInputSourceDriver(
  fhicl::ParameterSet const& /*ps*/,
  art::ProductRegistryHelper& helper,
  art::SourceHelper const& aSH)
  : srcHelper_{aSH}, bInput_{}
{
  // Products this source will reconstitute into Principals
  // string is the module label.
  // At the Run level, we add:
  // 1. metadata on...
  // At the Event level, we add:
  helper.reconstitutes<snemo::MultiProperties, art::InRun>(outputLabel);
  // 1. Primary vertex:
  helper.reconstitutes<geomtools::vector_3d, art::InEvent>(outputLabel);
  // 2. Time (always null in SD)
  helper.reconstitutes<double, art::InEvent>(outputLabel);
  // 3. Primary event
  helper.reconstitutes<snemo::GenBBPrimaryEvent, art::InEvent>(outputLabel);
  // each expected bank of hits, even if empty
  for (auto hitCat : stepHitCategories) {
    helper.reconstitutes<snemo::StepHitCollection, art::InEvent>(outputLabel,
                                                                 hitCat);
  }
}

void
snemo::BrioInputSourceDriver::readFile(std::string const& filename,
                                       art::FileBlock*& fb)
{
  bInput_.open(filename);
  std::cout << "Opening file " << filename << std::endl;
  if (!this->validateSchema(bInput_)) {
    // Need to throw if we get here
    // "throw cet::CodedException("BrioInput")"?
  };

  // Create the file block for the new file
  fb =
    new art::FileBlock{art::FileFormatVersion{1, "FLSIMULATE 3.3"}, filename};
}

bool
snemo::BrioInputSourceDriver::readNext(art::RunPrincipal const* const inR,
                                       art::SubRunPrincipal const* const inSR,
                                       art::RunPrincipal*& outR,
                                       art::SubRunPrincipal*& outSR,
                                       art::EventPrincipal*& outE)
{
  // BRIO is based on while/next iteration...
  // However, we seem to lose one event per file currently...
  if (!bInput_.has_next(ER_STORE)) {
    return false;
  }

  // If input RunPrincipal is null, we need to create one (we are not yet
  // checking for boundaries, don't expect them in Brio files)
  if (inR == nullptr) {
    std::cout << "New run\n";
    outR = srcHelper_.makeRunPrincipal(1, art::Timestamp{});
    // GI_STORE likely has run level info, maybe even global (where it would be
    // handled in open/close file and Service attachment)
    // From Simulation, each element in the GI store
    // has keys that tell us about it
    // __dpp.io.metadata.key: the eventual key we need to look up
    // __dpp.io.metadata.meta: the multiproperties "meta" key
    // __dpp.io.metadata.rank: seems to just be the store index?
    // These are used to reconstitute the overall multiproperties object
    // Should be put in Run, then services can register to be notified
    // of new Runs (hence new data)
    datatools::properties p;
    size_t entry{0};
    auto inputMP = std::make_unique<snemo::MultiProperties>();

    while (bInput_.has_next(GI_STORE)) {
      bInput_.load(p, GI_STORE, entry);
      std::string key{p.fetch_string("__dpp.io.metadata.key")};
      std::string meta{p.fetch_string("__dpp.io.metadata.meta")};
      p.clean("__dpp.io.metadata.key");
      p.clean("__dpp.io.metadata.meta");
      p.clean("__dpp.io.metadata.rank");
      inputMP->add(key, meta, p);
      ++entry;
    }

    art::put_product_in_principal(std::move(inputMP), *outR, outputLabel);
  }
  // Same for input SubRunPrincipal, need to create, but BRIO files have no
  // concept of subrun so we simply match Run ID
  if (inSR == nullptr) {
    art::SubRunID srID{outR ? outR->run() : inR->run(), 0};
    outSR = srcHelper_.makeSubRunPrincipal(
      srID.run(), srID.subRun(), art::Timestamp{});
  }

  // ALWAYS create outE...
  datatools::things e;
  bInput_.load_next(e, ER_STORE);
  // ER_STORE expected to hold two banks
  // "EH" is a simple header. We only really need the event number for
  // now.
  auto EH = e.get<snemo::datamodel::event_header>("EH");
  auto eID = EH.get_id().get_event_number();
  outE = srcHelper_.makeEventPrincipal(outR ? outR->run() : inR->run(),
                                       outSR ? outSR->subRun() : inSR->subRun(),
                                       eID,
                                       art::Timestamp{});

  // "SD" is the simulated data itself, and has substructure that we convert:
  auto SD = e.get<mctools::simulated_data>("SD");
  // 1. Vertex (CLHEP 3vector)
  auto vertex = std::make_unique<geomtools::vector_3d>(SD.get_vertex());
  art::put_product_in_principal(std::move(vertex), *outE, outputLabel);

  // 2. Time (double, though generally always zero for sim)
  art::put_product_in_principal(
    std::make_unique<double>(SD.get_time()), *outE, outputLabel);

  // 3. Primary Event (genbb_primary event)
  auto primary =
    std::make_unique<snemo::GenBBPrimaryEvent>(SD.get_primary_event());
  art::put_product_in_principal(std::move(primary), *outE, outputLabel);

  // 4. Properties (but always empty)
  // 5. Step hits (generally always collections of "handles":
  //    - In this case, banks may be:
  //      - calo
  //      - xcalo
  //      - gveto
  //      - gg
  for (auto hitCat : stepHitCategories) {
    auto HC = std::make_unique<snemo::StepHitCollection>();

    if (SD.has_step_hits(hitCat)) {
      auto stepHits = SD.get_step_hits(hitCat);
      HC->reserve(stepHits.size());
      for (const auto& hitHandle : stepHits) {
        HC->push_back(snemo::StepHit(hitHandle.get()));
      }
    }
    art::put_product_in_principal(std::move(HC), *outE, outputLabel, hitCat);
  }

  return true;
}

void
snemo::BrioInputSourceDriver::closeCurrentFile()
{
  bInput_.close();
}

bool
snemo::BrioInputSourceDriver::validateSchema(const brio::reader& /*r*/) const
{
  return true;
}

// Make the implementation usable in the framework
namespace snemo {
  using BrioInputSource = art::Source<BrioInputSourceDriver>;
}

DEFINE_ART_INPUT_SOURCE(snemo::BrioInputSource)
