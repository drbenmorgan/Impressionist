//! Convert an RHD Stream to Root

// - Boost:
#include <boost/program_options.hpp>

#include <cstdio>
#include <iostream>

#include "snfee/data/calo_hit_record.h"
#include "snfee/data/tracker_hit_record.h"
#include "snfee/data/trigger_record.h"
#include "snfee/io/multifile_data_reader.h"

// Input is 1-N RTD file to convert
// Output is 1 root file
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TTreeIndex.h"

#include "ROOT/TSeq.hxx"
#include "TRandom.h"
#include "TStopwatch.h"

namespace {
  // Quick and dumb mess-up of trigger ids
  int32_t
  mixTriggerID(int32_t t)
  {
    int32_t nt = t + static_cast<int32_t>(gRandom->Gaus(0, 10));
    return nt < 0 ? 0 : nt;
  }

  // Convert a Boost stream of RHD records to a TTree, with sorting of entries
  // by trigger_id
  template <typename RHDType>
  void
  rhd2root(snfee::io::multifile_data_reader& reader,
           std::string const& ofilename)
  {
    // Parameters (for future factorization)
    const std::string unsortedFilename{ofilename + ".unsorted"};
    // Use gsl finally once C++17...
    const std::string sortedFilename{ofilename};

    const std::string rhdFileTitle{"SuperNEMO RHD File"};
    const std::string rhdTreeName{"RawHitData"};
    const std::string rhdTreeTitle{"SuperNEMO Raw Hit Data"};
    const std::string rhdBranchName{"RHD"};
    const std::string rhdTriggerIDBranchName{"_trigger_id_"};

    // Intermediate direct conversion
    {
      TStopwatch tsp;
      tsp.Start();
      TFile unsortedFile{unsortedFilename.c_str(), "RECREATE"};
      TTree rhdTree{rhdTreeName.c_str(), rhdTreeTitle.c_str()};

      auto rhd = new RHDType{};
      rhdTree.Branch(rhdBranchName.c_str(), &rhd);

      std::string msg{"Converting RHD stream to ROOT Tree, done: "};

      size_t counter{0};
      while (reader.has_record_tag()) {
        reader.load(*rhd);

        // Mix up to model non-time ordering
        rhd->set_trigger_id(mixTriggerID(rhd->get_trigger_id()));

        rhdTree.Fill();

        if (!(counter % 1000)) {
          std::clog << msg << counter << "\r";
        }
        counter++;
      }
      std::clog << msg << counter << "\n";

      unsortedFile.Write();
      tsp.Stop();
      tsp.Print();
    }

    // Sort entries by trigger_id
    {
      TStopwatch tsp;
      tsp.Start();

      ROOT::EnableImplicitMT();

      TFile unsortedFile{unsortedFilename.c_str()};
      TTree* unsortedTree;
      unsortedFile.GetObject(rhdTreeName.c_str(), unsortedTree);
      // Help random access performance, can expect same trigger_ids to
      // be close to each other
      // To be adjusted based on typical RHD file size (or from GetNbytes)
      unsortedTree->SetMaxVirtualSize(1e9);

      const auto nentries = unsortedTree->BuildIndex(rhdTriggerIDBranchName.c_str());
      auto index = dynamic_cast<TTreeIndex*>(unsortedTree->GetTreeIndex());

      RHDType* hit{nullptr};
      unsortedTree->SetBranchAddress(rhdBranchName.c_str(), &hit);

      TFile sortedFile{sortedFilename.c_str(), "RECREATE", rhdFileTitle.c_str()};
      auto sortedTree = unsortedTree->CloneTree(0);

      std::string msg{"Sorting RHD Tree by Trigger ID: "};

      for (auto i : ROOT::TSeqI(nentries)) {
        unsortedTree->GetEntry(index->GetIndex()[i]);
        sortedTree->Fill();
        if (!(i % 1000)) {
          std::clog << msg << 100 * i / nentries << "%\r";
        }
      }
      std::clog << msg << "100%\n";

      sortedFile.Write();
      tsp.Stop();
      tsp.Print();
    }

    // Remove intermediate file (eventually handled automatically
    // by gsl finally)
    int rmStatus = std::remove(unsortedFilename.c_str());
    if (rmStatus) {
      std::perror(("Failed to remove intermediate ROOT file '" +
                  unsortedFilename + "'").c_str());
    }
  }
} // namespace

int
main(int argc, char* argv[])
{
  // - Command Line
  snfee::io::multifile_data_reader::config_type inputConfig;
  std::string outputFile{};

  namespace po = boost::program_options;
  po::options_description opts("Allowed options");
  opts.add_options()("help,h", "produce help message")(
    "input-file,i",
    po::value<std::vector<std::string>>(&inputConfig.filenames)
      ->value_name("<PATH>")
      ->required()
      ->multitoken(),
    "path to RHD input file")(
    "output-file,o",
    po::value<std::string>(&outputFile)->value_name("<PATH>")->required(),
    "path to ROOT output file");

  // Describe command line arguments
  try {
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(opts).run(), vm);
    if (vm.count("help")) {
      std::cout << opts << "\n";
      return 0;
    }
    po::notify(vm);
  }
  catch (std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  // - Processing
  // 1. Determine type of input RHD stream (calo, tracker, trigger)
  try {
    snfee::io::multifile_data_reader reader{inputConfig};
    std::string rhdType{reader.get_record_tag()};

    // 2. Process according to stream type
    if (rhdType == snfee::data::calo_hit_record::SERIAL_TAG) {
      rhd2root<snfee::data::calo_hit_record>(reader, outputFile);
    } else if (rhdType == snfee::data::tracker_hit_record::SERIAL_TAG) {
      rhd2root<snfee::data::tracker_hit_record>(reader, outputFile);
    } else if (rhdType == snfee::data::trigger_record::SERIAL_TAG) {
      rhd2root<snfee::data::tracker_hit_record>(reader, outputFile);
    } else {
      std::cerr << "Unknown RHD type '" << rhdType << "' in input stream "
          << std::endl;
      return EXIT_FAILURE;
    }
  }
  catch(std::exception& e) {
    std::cerr << "RHD2ROOT Conversion threw an exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}
