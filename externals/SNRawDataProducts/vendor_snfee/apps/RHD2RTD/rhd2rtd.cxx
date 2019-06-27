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
#include <snfee/rtdb/builder.h>

struct app_params_type
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  std::string config_filename;
  std::string report_filename;
  int32_t     run_id = snfee::data::INVALID_RUN_ID;
  bool        force_complete_rtd = false;
  std::size_t max_total_records = 0;
  uint32_t    calo_rhd_buffer_capacity    = 0;
  uint32_t    tracker_rhd_buffer_capacity = 0;
  bool        accept_unsorted_rhd = false;
  std::size_t unsorted_records_min_popping_safety_depth = 3;
  uint32_t    skel_run_id = 100;
  uint32_t    skel_nb_crates = 2;
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
      
      ("help-config,G", "produce documentation about configuration or the merge")
      
      ("skel-config,K", "produce a skeleton configuration file")
      
      ("skel-run-id",
       po::value<uint32_t>(&app_params.skel_run_id)
       ->value_name("number"),
       "set the run ID (>=100) for the production of the skeleton configuration file")
      
      ("skel-nb-crates",
       po::value<uint32_t>(&app_params.skel_nb_crates)
       ->value_name("number"),
       "set the number of crates (1 <= N <= 6) for the production of the skeleton configuration file")
      
      ("logging,L",
       po::value<std::string>()->value_name("level"),
       "logging priority")
      
      ("config,c",
       po::value<std::string>(&app_params.config_filename)
       ->value_name("path"),
       "set the configuration filename")
      
      ("report,r",
       po::value<std::string>(&app_params.report_filename)
       ->value_name("path"),
       "set the report filename")
      
      ("run-id,R",
       po::value<int32_t>(&app_params.run_id)
       ->value_name("number"),
       "set the run ID (override value from the config file)")
      
      ("force-complete-rtd,C",
        po::value<bool>(&app_params.force_complete_rtd)
       ->default_value(false)
       ->zero_tokens(),      
       "force complete RTD on output (override value from the config file, expert)")
      
      ("max-total-records,M",
       po::value<std::size_t>(&app_params.max_total_records)
       ->value_name("number"),
       "set the maximum number of RTD records to be generated (expert)")
      
      ("calo-rhd-buffer-capacity",
       po::value<uint32_t>(&app_params.calo_rhd_buffer_capacity)
       ->value_name("number"),
       "set the capacity of calorimeter RHD input buffers (expert)")
      
      ("tracker-rhd-buffer-capacity",
       po::value<uint32_t>(&app_params.tracker_rhd_buffer_capacity)
       ->value_name("number"),
       "set the capacity of tracker RHD input buffers (expert)")
      
      ("accept-unsorted-rhd,U",
       po::value<bool>(&app_params.accept_unsorted_rhd)
       ->zero_tokens()
       ->default_value(false),
       "accept unsorted records from RHD input buffers (expert)")
      
    ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "snfee-rhd2rtd : "
                << "Merge raw hit data file (RHD) into raw trigger data file (RTD)"
                << std::endl << std::endl;
      std::cout << 
        "\n"
        "  +----------------------------------+         +----------+\n"
        "  |   Calo crate #0 RHD hit records  |-------->|          |\n"
        "  +----------------------------------+         |          |\n"
        "                                               |          |\n"
        "  +----------------------------------+         |          |       +-----------+\n"
        "  |   Calo crate #1 RHD hit records  |-------->|  merger  |------>|    RTD    |\n"
        "  +----------------------------------+         |          |       +-----------+\n"
        "                    :                          |          |\n"
        "  +----------------------------------+         |          |\n"
        "  | Tracker crate #2 RHD hit records |-------->|          |\n"
        "  +----------------------------------+         +----------+\n";
      std::cout << std::endl;
      std::cout << "Usage : " << std::endl << std::endl;
      std::cout << "  snfee-rhd2rtd [OPTIONS]" << std::endl << std::endl;  
      std::cout << opts << std::endl;
      std::cout << "Example : " << std::endl << std::endl;
      std::cout << "  snfee-rhd2rtd \\\n";
      std::cout << "    --run-id 8 \\\n";
      std::cout << "    --config \"rhd2rtd_run-8.conf\" \\\n";
      std::cout << "    --report \"rhd2rtd_run-8.log\"";
      std::cout << std::endl << std::endl;  
      std::cout << "  snfee-rhd2rtd --help-config > snfee-rhd2rtd_config.rst\n";
      std::cout << std::endl << std::endl;  
      std::cout << "  snfee-rhd2rtd \\\n";
      std::cout << "    --skel-config  \\\n";
      std::cout << "    --skel-run-id 100 \\\n";
      std::cout << "    --skel-nb-crates 2 > snfee-rhd2rtd.conf \\\n";
      std::cout << std::endl << std::endl;  
      return (-1);
    }

    // Use command line arguments :
    if (vm.count("help-config")) {
      snfee::rtdb::builder_config::print_documentation(std::cout);  
      return (-1);
    }
 
    // Use command line arguments :
    if (vm.count("skel-config")) {
      snfee::rtdb::builder_config::print_skel_config(std::cout,
                                                     app_params.skel_run_id,
                                                     app_params.skel_nb_crates);  
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
    DT_THROW_IF(app_params.config_filename.empty(),
                std::logic_error,
                "Missing configuration filename!");

    snfee::rtdb::builder_config rtdBuilderCfg;
    std::string rtdBuilderCfgFilename = app_params.config_filename;
    snfee::rtdb::builder_config::load(rtdBuilderCfgFilename, rtdBuilderCfg);
    if (app_params.run_id != snfee::data::INVALID_RUN_ID) {
      if (rtdBuilderCfg.run_id != snfee::data::INVALID_RUN_ID) {
        DT_LOG_WARNING(app_params.logging, "Override run ID " << rtdBuilderCfg.run_id << " from the configuration file.");
        DT_LOG_WARNING(app_params.logging, "Run ID is now : " << app_params.run_id);
      }
      rtdBuilderCfg.run_id = app_params.run_id;
    }

    if (app_params.force_complete_rtd) {
      rtdBuilderCfg.force_complete_rtd = true;
    }
  
    if (app_params.accept_unsorted_rhd) {
      rtdBuilderCfg.accept_unsorted_records = true;
    }
   
    if (app_params.max_total_records != 0) {
      if (rtdBuilderCfg.output_config.max_total_records != 0) {
        DT_LOG_WARNING(app_params.logging, "Override max number of RTD records "
                       << rtdBuilderCfg.output_config.max_total_records << " from the configuration file.");
        DT_LOG_WARNING(app_params.logging, "Max number of RTD records : " << app_params.max_total_records);
     }
      rtdBuilderCfg.output_config.max_total_records = app_params.max_total_records;
    }

    if (app_params.calo_rhd_buffer_capacity != 0) {
      if (rtdBuilderCfg.calo_rhd_buffer_capacity != 0) {
        DT_LOG_WARNING(app_params.logging, "Override calorimeter RHD input buffer capacity "
                       << rtdBuilderCfg.calo_rhd_buffer_capacity << " from the configuration file.");
        DT_LOG_WARNING(app_params.logging, "Calorimeter RHD input buffer capacity is now : "
                       << app_params.calo_rhd_buffer_capacity);
      }
      rtdBuilderCfg.calo_rhd_buffer_capacity = app_params.calo_rhd_buffer_capacity;
    }

    if (app_params.tracker_rhd_buffer_capacity != 0) {
      if (rtdBuilderCfg.tracker_rhd_buffer_capacity != 0) {
        DT_LOG_WARNING(app_params.logging, "Override tracker RHD input buffer capacity "
                       << rtdBuilderCfg.tracker_rhd_buffer_capacity << " from the configuration file.");
        DT_LOG_WARNING(app_params.logging, "Tracker RHD input buffer capacity is now : "
                       << app_params.tracker_rhd_buffer_capacity);
      }
      rtdBuilderCfg.tracker_rhd_buffer_capacity = app_params.tracker_rhd_buffer_capacity;
    }

    // Check the configuration:
    snfee::rtdb::builder_config::check(rtdBuilderCfg);
    {
      // Print:
      boost::property_tree::ptree options;
      options.put("title", "RTD Builder Configuration: ");
      rtdBuilderCfg.print_tree(std::clog, options);
    }

    // The RTD builder:
    snfee::rtdb::builder rtdBuilder;
    rtdBuilder.set_logging(app_params.logging);
    rtdBuilder.set_config(rtdBuilderCfg);
    {
      // Print:
      boost::property_tree::ptree poptions;
      poptions.put("title", "RTD builder:");
      rtdBuilder.print_tree(std::clog, poptions);
    }

    // Run:
    rtdBuilder.initialize();
    rtdBuilder.run();
    rtdBuilder.terminate();
    
    {
      // Results:
      std::ostream * rtdBuilderResultsOut = &std::cout;
      std::unique_ptr<std::ofstream> rtdBuilderResultsOutfile;
      if (!app_params.report_filename.empty()) {
        std::string rtdBuilderResultsFilename = app_params.report_filename;
        datatools::fetch_path_with_env(rtdBuilderResultsFilename);
        rtdBuilderResultsOutfile.reset(new std::ofstream(rtdBuilderResultsFilename.c_str()));
        DT_THROW_IF(!rtdBuilderResultsOutfile, std::logic_error,
                    "Cannot open result file '" << rtdBuilderResultsFilename << "'!");
        rtdBuilderResultsOut = rtdBuilderResultsOutfile.get();
      }
      const auto & rtdBuilderResults = rtdBuilder.get_results();
      int i = 0;
      *rtdBuilderResultsOut << "Results :" << std::endl;
      for (const auto & res : rtdBuilderResults) {
        *rtdBuilderResultsOut << "- Worker #" << i;
        if (res.category == snfee::rtdb::builder::WORKER_INPUT_RHD) {
          *rtdBuilderResultsOut << " (input RHD";
          *rtdBuilderResultsOut << ",crate=" << snfee::model::crate_model_label(res.crate_model);
          *rtdBuilderResultsOut << ")"; 
        } else if (res.category == snfee::rtdb::builder::WORKER_OUTPUT_RTD) {
          *rtdBuilderResultsOut << " (output RTD)";
        } 
        *rtdBuilderResultsOut << " : " << std::endl;
        *rtdBuilderResultsOut << "   - Processed records : " << res.processed_records_counter1;
        *rtdBuilderResultsOut << std::endl;
        if (res.category == snfee::rtdb::builder::WORKER_OUTPUT_RTD) {
          *rtdBuilderResultsOut << "   - Stored records    : " << res.processed_records_counter2;
          *rtdBuilderResultsOut << std::endl;
        }
        i++;
      }
    }
     
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
