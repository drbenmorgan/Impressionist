//! Try and serialize RRawTriggerData to brio/things

// - Boost:
#include <boost/program_options.hpp>

#include <iostream>

#include "snfee/data/RRawTriggerData.h"
#include "snfee/data/raw_trigger_data.h"
#include "snfee/data/rtdReformater.h"
#include "snfee/io/multifile_data_reader.h"

#include <bayeux/brio/writer.h>
#include <bayeux/datatools/things.h>

// Input is 1-N RTD file to convert
// Output is 1 brio file

int
main(int argc, char* argv[])
{
  // - Command Line
  snfee::io::multifile_data_reader::config_type inputConfig;
  std::string outputFile{};

  // clang-format off
  namespace po = boost::program_options;
  po::options_description opts("Allowed options");
  opts.add_options()
    ("help,h", "produce help message")
    ("input-file,i",
      po::value<std::vector<std::string>>(&inputConfig.filenames)
      ->value_name("<PATH>")
      ->required()
      ->multitoken(),
      "path to RTD input file")
    ("output-file,o",
      po::value<std::string>(&outputFile)
      ->value_name("<PATH>")
      ->required(),
      "path to ROOT output file");

  // Describe command line arguments
  try {
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
              .options(opts)
              .run(), vm);
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
  // clang-format on

  // - Processing
  // Input
  snfee::data::raw_trigger_data rtdRaw;
  snfee::io::multifile_data_reader reader{inputConfig};

  // Output
  // Create Brio writer
  // Only "ER" store, No Metadata ("GI") store for now...
  brio::writer writer{outputFile};
  std::string erStore{"ER"};
  writer.add_store(erStore, datatools::things{}.get_serial_tag());
  writer.add_store("GI", datatools::properties{}.get_serial_tag());
  datatools::things workItem;

  // Test handful of records only
  size_t counter{0};
  size_t maxRecords{5};

  while (reader.has_record_tag() &&
         reader.record_tag_is(snfee::data::raw_trigger_data::SERIAL_TAG)) {
    reader.load(rtdRaw);

    workItem.clear();
    auto& rtdBrio = workItem.add<snfee::data::RRawTriggerData>("RTD");
    rtdBrio = snfee::data::rtdOnlineToOffline(rtdRaw);
    writer.store(workItem, erStore);

    if (!(counter % 1000))
      std::clog << "Processed record: " << counter << "\n";
    counter++;
  }

  std::clog << "Total records processed: " << counter << "\n";

  return 0;
}
