// Standard library:
#include <cstdio>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/io_factory.h>
// - Boost:
#include <boost/program_options.hpp>

// This project:
#include <snfee/utils.h>
#include <snfee/io/rtd2root_converter.h>

struct app_params_type
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  snfee::io::rtd2root_converter::config_type converter_cfg;
};

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    app_params_type app_params;
    
    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")

      ("logging,L",
       po::value<std::string>()->value_name("level"),
       "logging priority")

      ("input-list,l",
       po::value<std::string>(&app_params.converter_cfg.input_rtd_listname)
       ->value_name("path"),
       "set a list of RTD input filenames")

      ("input-file,i",
       po::value<std::vector<std::string>>(&app_params.converter_cfg.input_rtd_filenames)
       ->multitoken()
       ->value_name("path"),
       "add a RTD input filename")

      ("output-file,o",
       po::value<std::string>(&app_params.converter_cfg.output_root_filename)
       ->value_name("path"),
       "set the Root output filename")

      ("max-total-records,M",
       po::value<std::size_t>(&app_params.converter_cfg.max_total_records)
       ->value_name("number")->default_value(0),
       "set the maximum number of RTD records to be converted (default: 0, unused")
      
      ("calo-select-crate,C",
       po::value<int16_t>(&app_params.converter_cfg.calo_sel_config.crate_num)
       ->value_name("id"),
       "set a specific crate number (0-2) for calorimeter hit record selection")

      ("calo-select-board,B",
       po::value<int16_t>(&app_params.converter_cfg.calo_sel_config.board_num)
       ->value_name("id"),
       "set a specific board number (0-9,11-20) for calorimeter hit record selection")

      ("calo-select-chip,H",
       po::value<int16_t>(&app_params.converter_cfg.calo_sel_config.chip_num)
       ->value_name("id"),
       "set a specific chip number (0-7) for calorimeter hit record selection")

      ("calo-select-reverse,V",
       po::value<bool>(&app_params.converter_cfg.calo_sel_config.reverse)
       ->zero_tokens()->default_value(false),
       "reverse the calorimeter hit selection")
      
    ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "snfee-rtd2root : "
                << "Convert raw trigger data file (RTD) to a Root file"
                << std::endl << std::endl;
      std::cout << "Usage : " << std::endl << std::endl;
      std::cout << "  snfee-rtd2root [OPTIONS]" << std::endl << std::endl;  
      std::cout << opts << std::endl;
      std::cout << "Example : " << std::endl << std::endl;
      std::cout << "  snfee-rtd2root \\\n";
      std::cout << "    --input-file \"snemo_run-8_rtd_part-0.data.gz\" \\\n";
      std::cout << "    --input-file \"snemo_run-8_rtd_part-1.data.gz\" \\\n";
      std::cout << "    --input-file \"snemo_run-8_rtd_part-2.data.gz\" \\\n";
      std::cout << "    --output-file \"snemo_run-8_rtd.root\"";
      std::cout << std::endl << std::endl;  
      std::cout << "  snfee-rtd2root \\\n";
      std::cout << "    --input-list \"snemo_run-8_rtd.lis\" \\\n";
      std::cout << "    --output-file \"snemo_run-8_rtd.root\"";
      std::cout << std::endl << std::endl;  
      return (-1);
    }

    // Use command line arguments :
    if (vm.count("logging")) {
      std::string logging_repr = vm["logging"].as<std::string>();
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Logging repr. = '" << logging_repr << "'");
      app_params.logging = datatools::logger::get_priority(logging_repr);
      DT_THROW_IF(app_params.logging == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority '" << vm["logging"].as<std::string>() << "'!");
    }

    // Checks:
    DT_THROW_IF(app_params.converter_cfg.input_rtd_listname.empty() and
                app_params.converter_cfg.input_rtd_filenames.size() == 0,
                std::logic_error,
                "Missing input RTD filenames!");
    DT_THROW_IF(app_params.converter_cfg.output_root_filename.empty(),
                std::logic_error,
                "Missing output Root filename!");

    // The RTD builder:
    snfee::io::rtd2root_converter rtd2rootConverter;
    rtd2rootConverter.set_logging(app_params.logging);
    rtd2rootConverter.set_config(app_params.converter_cfg);

    // Run:
    rtd2rootConverter.initialize();
    rtd2rootConverter.run();
    rtd2rootConverter.terminate();
    
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
