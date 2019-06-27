// Standard library:
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <memory>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/io_factory.h>
// - Boost:
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

// This project:
#include <snfee/utils.h>
#include <snfee/data/trigger_record.h>
#include <snfee/data/calo_hit_record.h>
#include <snfee/data/tracker_hit_record.h>
#include <snfee/io/raw_hit_reader.h>
#include <snfee/io/multifile_data_writer.h>

// \brief Application configuration parameters
struct app_params_type
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  datatools::logger::priority reader_logging = datatools::logger::PRIO_FATAL;
  snfee::io::raw_hit_reader::config_type reader_config;
  std::string input_listname;
  std::vector<std::string> input_filenames;
  // std::vector<std::string> output_filenames;
  std::string output_filename;
  bool        dynamic_output_files = false;
  std::size_t max_rhd_files = 1;
  snfee::io::multifile_data_writer::config_type writer_config;
  std::size_t crd_counter_period = 1000;
  bool        print_records = false;
  std::string work_dir = "/tmp";
  bool        force_fake_trigger_ids = false;
  int32_t     session_id = 0;
  std::size_t max_crd_per_input_file = 0;
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
       po::value<std::string>()
       ->value_name("level"),
       "set logging priority")

      ("reader-logging,R",
       po::value<std::string>()
       ->value_name("level"),
       "set reader logging priority")

      ("input-list,I",
       po::value<std::string>(&app_params.input_listname)
       ->value_name("path"),
       "set the filename of the CRD input files list")
 
      ("input-file,i",
       po::value<std::vector<std::string>>(&app_params.input_filenames)
       ->value_name("path"),
       "add an RHD input filename")

      ("output-file,o",
       po::value<std::string>(& app_params.output_filename)
       ->value_name("path"),
       "set the RHD output filename or filename pattern")

      ("crate-number,c",
       po::value<int16_t>(& app_params.reader_config.crate_num)
       ->value_name("number"),
       "set the crate number (0, 1 or 2)")

      ("max-records,m",
       po::value<std::size_t>(& app_params.writer_config.max_total_records)
       ->value_name("number")
       ->default_value(0),
       "set the maximum total number of collected RHD records")

      ("max-records-per-file,f",
       po::value<std::size_t>(& app_params.writer_config.max_records_per_file)
       ->value_name("number")
       ->default_value(0),
       "set the maximum number of collected RHD records per file")

      ("max-output-files,M",
       po::value<std::size_t>(& app_params.max_rhd_files)
       ->value_name("number")
       ->default_value(1),
       "set the maximum number of RHD output files")

      ("dynamic-output-files,Y",
       po::value<bool>(& app_params.dynamic_output_files)
       ->zero_tokens()
       ->default_value(false),
       "allow to dynamically add more RHD output files as needed")

      ("allow-overrun,O",
       po::value<bool>(& app_params.writer_config.terminate_on_overrun)
       ->zero_tokens()
       ->default_value(false),
       "allow data writer overrun (expert)")

      ("print-records,P",
       po::value<bool>(& app_params.print_records)
       ->zero_tokens()
       ->default_value(false),
       "print records (debug only)")

      ("force-fake-trigger-ids,F",
       po::value<bool>(& app_params.force_fake_trigger_ids)
       ->zero_tokens()
       ->default_value(false),
       "force trigger IDs and ignore those from CRD input files (expert)")

      ("session-id,S",
       po::value<int32_t>(& app_params.session_id)
       ->default_value(0),
        "set the session ID (expert)")

      ("work-dir,W",
       po::value<std::string>(& app_params.work_dir)
       ->value_name("path")
       ->default_value("/tmp"),
       "set the working directory (expert)")

      ("max-crd-per-input-file,C",
       po::value<std::size_t>(& app_params.max_crd_per_input_file)
       ->value_name("number")
       ->default_value(0),
       "set the maximum number of records collected per CRD input file (expert)")

      ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "snfee-crd2rhd : "
                << "Convert commissioning raw data file (CRD) to official raw hit data file (RHD)"
                << std::endl << std::endl;
      std::cout << "Usage : " << std::endl << std::endl;
      std::cout << "  snfee-crd2rhd [OPTIONS]" << std::endl << std::endl;  
      std::cout << opts << std::endl;
      std::cout << "Example : " << std::endl << std::endl;
      std::cout << " 1) Convert an unique CRD file to an unique RHD file : " << std::endl << std::endl;
      std::cout << "    snfee-crd2rhd \\\n";
      std::cout << "      --crate-number 0 \\\n";
      std::cout << "      --input-file \"/data/SuperNEMO/ManCom2108/Run_8/calo/RunCalo_8.dat\" \\\n";
      std::cout << "      --output-file \"snemo_run-8_rhd_crate-0_part-0.xml\"";
      std::cout << std::endl << std::endl;  
      std::cout << " 2) Convert a list of CRD files to a list of RHD files: " << std::endl << std::endl;
      std::cout << "    snfee-crd2rhd \\\n";
      std::cout << "      --crate-number 0 \\\n";
      std::cout << "      --force-fake-trigger-ids \\\n";
      std::cout << "      --max-records-per-file 500000 \\\n";
      std::cout << "      --dynamic-output-files \\\n";
      std::cout << "      --input-list \"snemo_run-8_crd_files.lis\" \\\n";
      std::cout << "      --output-file \"snemo_run-8_rhd_crate-0.xml\" \n";
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
    if (vm.count("reader-logging")) {
      std::string logging_repr = vm["reader-logging"].as<std::string>();
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Logging repr. = '" << logging_repr << "'");
      app_params.reader_logging = datatools::logger::get_priority(logging_repr);
      DT_THROW_IF(app_params.reader_logging == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid reader logging priority '" << vm["reader-logging"].as<std::string>() << "'!");
    }

    // Checks:
    DT_THROW_IF(app_params.reader_config.crate_num < 0, std::logic_error, "Missing crate number!");
    // DT_THROW_IF(app_params.output_filename.empty(), std::logic_error, "Missing output production raw hit data file!");

    DT_LOG_DEBUG(app_params.logging, "Input filename  = '" << app_params.reader_config.input_filename << "'");
    DT_LOG_DEBUG(app_params.logging, "Crate number    = [" << app_params.reader_config.crate_num << "]");
    DT_LOG_DEBUG(app_params.logging, "With calo       = " << std::boolalpha << app_params.reader_config.with_calo);
    DT_LOG_DEBUG(app_params.logging, "With tracker    = " << std::boolalpha << app_params.reader_config.with_tracker);
    if (app_params.reader_config.with_calo_waveforms) {
      DT_LOG_DEBUG(app_params.logging, "With calo waveforms = " << std::boolalpha << app_params.reader_config.with_calo_waveforms);
    }
    DT_LOG_DEBUG(app_params.logging, "Output filename   = '" << app_params.output_filename << "'");
    DT_LOG_DEBUG(app_params.logging, "Max total records = " << app_params.writer_config.max_total_records);
    DT_LOG_DEBUG(app_params.logging, "Max records/file  = " << app_params.writer_config.max_records_per_file);
    DT_LOG_DEBUG(app_params.logging, "Terminate/overrun = " << std::boolalpha << app_params.writer_config.terminate_on_overrun);

    // Writer:
    std::unique_ptr<snfee::io::multifile_data_writer> pWriter;
    std::string output_file_dirname;
    std::string output_file_basename;
    std::string output_file_extension;
    int         output_file_part_num = -1;
    if (!app_params.output_filename.empty()) {
      std::string dirpath;
      std::string basename;
      // Extract output file:
      {
        std::size_t basename_pos = app_params.output_filename.find_last_of("/");
        basename = app_params.output_filename;
        if (basename_pos != std::string::npos) {
          dirpath = app_params.output_filename.substr(0, basename_pos);
          basename = app_params.output_filename.substr(basename_pos + 1);
        }
        std::vector<std::string> strs;
        boost::split(strs, basename, boost::is_any_of("."));
        DT_THROW_IF(strs.size() < 2, std::logic_error, "Missing output filename extension!");
        std::ostringstream basename_s;
        basename_s << strs[0];
        output_file_basename = basename_s.str();
        std::ostringstream extension_s;
        for (int i = 1; i < (int) strs.size(); i++) {
          extension_s << '.' << strs[i];
        }
        output_file_extension = extension_s.str();
        output_file_part_num = 0;
      }
      bool unique_output_file = true;
      if (app_params.writer_config.max_records_per_file != 0) {
        unique_output_file = false;
      }
      if (app_params.max_rhd_files > 1) {
        unique_output_file = false;
      }
      if (app_params.dynamic_output_files) {
        unique_output_file = false;
      }
      snfee::io::multifile_data_writer::config_type writerCfg = app_params.writer_config;
      // Build the initial list of output files:
      if (unique_output_file) {
        // Unique output file:
        DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,
                           "Set output file : '" << app_params.output_filename << "'");
        writerCfg.filenames.push_back(app_params.output_filename);
      } else {
        // Multiple output files (with automated postfix index):
        for (int ipart = 0; ipart < (int) app_params.max_rhd_files; ipart++) {
          int part_index = 0;
          // if (output_file_part_num >= 0) {
          //   part_index = output_file_part_num;
          // }
          part_index += ipart;
          std::ostringstream filename_s;
          if (!dirpath.empty()) {
            filename_s << dirpath << '/';
          }
          filename_s << output_file_basename << "_part-" << part_index << output_file_extension;
          std::string filename = filename_s.str();
          DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,
                             "Add output file : '" << filename << "'");
          writerCfg.filenames.push_back(filename);
          output_file_part_num = ipart;
        }
      }
      output_file_dirname = dirpath;
      pWriter.reset(new snfee::io::multifile_data_writer(writerCfg, app_params.logging));
    }

    // Hack for messy trigger IDs:
    int32_t     fake_trigger_id = 0;
    std::string fake_trigger_id_save
      = app_params.work_dir + "/snfee-crd2rhd_fake_trigger_ids-" + std::to_string(app_params.session_id) + ".save";
    if (app_params.force_fake_trigger_ids) {
      datatools::fetch_path_with_env(fake_trigger_id_save);
      if (boost::filesystem::exists(fake_trigger_id_save)) {
        std::ifstream load_fake_trigger_id(fake_trigger_id_save);
        DT_THROW_IF(!load_fake_trigger_id, std::logic_error,
                    "Cannot open '" << fake_trigger_id_save << "'!");
        load_fake_trigger_id >> fake_trigger_id;
        DT_THROW_IF(!load_fake_trigger_id, std::logic_error,
                    "Cannot read next fake trigger ID from file '" << fake_trigger_id_save << "'!");
      }
      DT_LOG_NOTICE(app_params.logging, "Forcing starting trigger ID : " << fake_trigger_id);
    }

    if (!app_params.input_listname.empty()) {
      // Read a file containing a list of input filenames:
      std::string listname = app_params.input_listname;
      datatools::fetch_path_with_env(listname);
      std::ifstream finput_list(listname);
      DT_THROW_IF(!finput_list, std::logic_error, "Cannot open input files list filename!");
      while (finput_list and ! finput_list.eof()) {
        std::string line;
        std::getline(finput_list, line);
        boost::trim_copy(line);
        if (line.empty()) {
          continue;
        }
        std::istringstream ins(line);
        std::string filename;
        ins >> filename >> std::ws;
        if (!filename.empty() and filename[0] != '#') {
          app_params.input_filenames.push_back(filename);
        }
      }
    }
    DT_THROW_IF(app_params.input_filenames.empty(),
                std::logic_error, "Missing CRD input files!");
    std::size_t stored_rhd_counter = 0;
    std::size_t crd_counter = 0;
    // Input file loop:
    bool end_of_input = false;
    for (int i_input_filename = 0;
         i_input_filename < (int) app_params.input_filenames.size();
         i_input_filename++) {
      app_params.reader_config.input_filename = app_params.input_filenames[i_input_filename];
      DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,
                         "Reading CRD input file : '" + app_params.reader_config.input_filename + "'");
      // Reader for RHD data file (for the commissioning phase using the SN CRATE SOFTWARE acquisition program):
      snfee::io::raw_hit_reader reader;
      reader.set_logging(app_params.reader_logging);
      reader.set_config(app_params.reader_config);
      reader.initialize();
      reader.print(std::cerr);
      std::size_t crd_counter_for_this_file = 0;
      bool end_of_input_for_this_file = false;
      // Reader loop:
      while (reader.has_next_hit()) {
        DT_LOG_DEBUG(app_params.logging, "Loading next record...");
        snfee::data::calo_hit_record    myCaloRec;
        snfee::data::tracker_hit_record myTrackerRec;
        snfee::io::raw_record_parser::record_type ret
          = reader.load_next_hit(myCaloRec, myTrackerRec);
        if (ret == snfee::io::raw_record_parser::RECORD_CALO) {
          DT_LOG_DEBUG(app_params.logging, "Found a calo hit record.");
          if (app_params.print_records) {
            boost::property_tree::ptree options;
            options.put("title", "Loaded calorimeter raw hit data record: ");
            // options.put("with_waveform_samples", true);
            myCaloRec.print_tree(std::clog, options);
          }
          if (app_params.force_fake_trigger_ids) {
            myCaloRec.set_trigger_id(fake_trigger_id);
            fake_trigger_id++;
          }
          if (pWriter) {
            pWriter->store(myCaloRec);
            stored_rhd_counter++;
          }
        } else if (ret == snfee::io::raw_record_parser::RECORD_TRACKER) {
          DT_LOG_DEBUG(app_params.logging, "Found a tracker hit record.");
          if (app_params.print_records) {
            boost::property_tree::ptree options;
            options.put("title", "Loaded tracker raw hit data record: ");
            myTrackerRec.print_tree(std::clog, options);
          }
          if (pWriter and !pWriter->is_terminated()) {
            pWriter->store(myTrackerRec);
            stored_rhd_counter++;
          }
        } else if (ret == snfee::io::raw_record_parser::RECORD_TRIGGER) {
          DT_LOG_WARNING(app_params.logging, "Found a trigger record.");
          DT_THROW(std::logic_error, "Trigger records are not supported!");
        } else {
          DT_THROW(std::logic_error, "Parsing failed!");
        }
        crd_counter++;
        crd_counter_for_this_file++;
        // End of loop:
        if (crd_counter % app_params.crd_counter_period == 0) {
          DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION, "Loaded CRD records: " << crd_counter);
        }
        if (app_params.writer_config.max_total_records
            and crd_counter == app_params.writer_config.max_total_records) {
          DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION, "Max CRD records reached!");
          end_of_input_for_this_file = true;
          end_of_input = true;
        }
        if (app_params.max_crd_per_input_file
            and crd_counter_for_this_file == app_params.max_crd_per_input_file) {
          DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION, "Max CRD records per input file reached!");
          end_of_input_for_this_file = true;
        }
        if (end_of_input) break;
        if (end_of_input_for_this_file) break;
        if (pWriter) {
          if (app_params.dynamic_output_files && pWriter->is_last_file()) {
            output_file_part_num++;
            std::ostringstream filename_s;
            if (!output_file_dirname.empty()) {
              filename_s << output_file_dirname << '/';
            }
            filename_s << output_file_basename << "_part-" << output_file_part_num << output_file_extension;
            std::string rhd_filename = filename_s.str();
            DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,
                               "Additional output file : '" << rhd_filename << "'");
            pWriter->add_filename(rhd_filename);
          }
        }
      } // end of reader loop:
      reader.reset();  
      if (end_of_input) break;
    } // end of input file loop.
    
    if (app_params.force_fake_trigger_ids) {
      DT_LOG_NOTICE(datatools::logger::PRIO_INFORMATION, "Saving next starting trigger ID : " << fake_trigger_id);
      std::ofstream save_fake_trigger_id(fake_trigger_id_save);
      save_fake_trigger_id << fake_trigger_id << std::endl;
     }    

    if (pWriter) {
      pWriter.reset();
    }

    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION, "Loaded CRD records: " << crd_counter);
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION, "Stored RHD records: " << stored_rhd_counter);

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
