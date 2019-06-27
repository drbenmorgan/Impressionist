// Ourselves:
#include "builder_config.h"

// Standard Library:
#include <sstream>

// This project:
#include <snfee/model/utils.h>

namespace snfee {
  namespace rtdb {

    // static
    std::string
    builder_config::format_label(const format_type f_)
    {
      if (f_ == FORMAT_BOOST_SERIAL) {
        return "boost::serialization";
      }
      return "";
    }

    builder_config::builder_config() { return; }

    builder_config::~builder_config() { return; }

    void
    builder_config::set_run_id(const int32_t rid_)
    {
      run_id = rid_;
      return;
    }

    int32_t
    builder_config::get_run_id() const
    {
      return run_id;
    }

    bool
    builder_config::is_force_complete_rtd() const
    {
      return force_complete_rtd;
    }

    void
    builder_config::set_force_complete_rtd(bool f_)
    {
      force_complete_rtd = f_;
      return;
    }

    bool
    builder_config::has_output_config() const
    {
      return !output_config.filenames.size();
    }

    void
    builder_config::set_output_config(
      const std::string& label_,
      const std::vector<std::string>& filepaths_,
      const std::size_t max_records_per_file_,
      const std::size_t max_total_records_,
      const format_type format_)
    {
      output_config_type oc;
      oc.label = label_;
      oc.max_records_per_file = max_records_per_file_;
      oc.max_total_records = max_total_records_;
      oc.filenames = filepaths_;
      oc.format = format_;
      output_config = oc;
      return;
    }

    const builder_config::output_config_type&
    builder_config::get_output_config() const
    {
      return output_config;
    }

    void
    builder_config::add_input_config(
      const std::string& label_,
      const snfee::model::crate_model_type crate_model_,
      const int32_t crate_id_,
      const std::vector<std::string>& filepaths_,
      const format_type format_)
    {
      input_config_type ic;
      ic.label = label_;
      ic.crate_model = crate_model_;
      ic.crate_id = crate_id_;
      ic.filenames = filepaths_;
      ic.format = format_;
      input_configs.push_back(ic);
      return;
    }

    void
    builder_config::add_input_config(
      const std::string& label_,
      const snfee::model::crate_model_type crate_model_,
      const int32_t crate_id_,
      const std::string& listpath_,
      const format_type format_)
    {
      input_config_type ic;
      ic.label = label_;
      ic.crate_model = crate_model_;
      ic.crate_id = crate_id_;
      ic.listname = listpath_;
      ic.format = format_;
      input_configs.push_back(ic);
      return;
    }

    const std::vector<builder_config::input_config_type>&
    builder_config::get_input_configs() const
    {
      return input_configs;
    }

    // virtual
    void
    builder_config::print_tree(
      std::ostream& out_,
      const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      std::ostringstream outs;
      if (popts.title.length()) {
        outs << popts.indent << popts.title << std::endl;
      }

      outs << popts.indent << tag << "Run ID : " << run_id << std::endl;

      outs << popts.indent << tag << "Input configs : " << input_configs.size()
           << std::endl;
      for (std::size_t i = 0; i < input_configs.size(); i++) {
        const auto& ic = input_configs[i];
        outs << popts.indent << skip_tag;
        std::ostringstream tagss;
        if (i == input_configs.size() - 1) {
          outs << last_tag;
          tagss << last_skip_tag;
        } else {
          outs << tag;
          tagss << skip_tag;
        }
        outs << "Input config #" << i << " : " << std::endl;

        outs << popts.indent << skip_tag << tagss.str() << tag << "Label : '"
             << ic.label << "'" << std::endl;

        outs << popts.indent << skip_tag << tagss.str() << tag
             << "Crate model : '"
             << snfee::model::crate_model_label(ic.crate_model) << "'"
             << std::endl;

        outs << popts.indent << skip_tag << tagss.str() << tag
             << "Crate ID : " << ic.crate_id << std::endl;

        outs << popts.indent << skip_tag << tagss.str() << tag << "Listname : '"
             << ic.listname << "'" << std::endl;

        outs << popts.indent << skip_tag << tagss.str() << tag
             << "Filenames : " << ic.filenames.size() << std::endl;

        for (int ifilename = 0; ifilename < (int)ic.filenames.size();
             ifilename++) {
          outs << popts.indent << skip_tag << tagss.str() << skip_tag;
          if ((ifilename + 1) == (int)ic.filenames.size()) {
            outs << last_tag;
          } else {
            outs << tag;
          }
          outs << "Filename #" << ifilename << " : '" << ic.filenames[ifilename]
               << "'";
          outs << std::endl;
        }

        outs << popts.indent << skip_tag << tagss.str() << last_tag
             << "Format : '" << format_label(ic.format) << "'" << std::endl;
      }

      outs << popts.indent << tag << "Output config : " << std::endl;

      outs << popts.indent << skip_tag << tag << "Label : '"
           << output_config.label << "'" << std::endl;

      outs << popts.indent << skip_tag << tag
           << "Filenames : " << output_config.filenames.size() << std::endl;

      for (int ifilename = 0; ifilename < (int)output_config.filenames.size();
           ifilename++) {
        outs << popts.indent << skip_tag << skip_tag;
        if ((ifilename + 1) == (int)output_config.filenames.size()) {
          outs << last_tag;
        } else {
          outs << tag;
        }
        outs << "Filename #" << ifilename << " : '"
             << output_config.filenames[ifilename] << "'";
        outs << std::endl;
      }

      outs << popts.indent << skip_tag << tag
           << "Max records/file : " << output_config.max_records_per_file
           << std::endl;

      outs << popts.indent << skip_tag << tag
           << "Max total records : " << output_config.max_total_records
           << std::endl;

      outs << popts.indent << skip_tag << tag
           << "Terminate/overrun : " << std::boolalpha
           << output_config.terminate_on_overrun << std::endl;

      outs << popts.indent << skip_tag << last_tag << "Format : '"
           << format_label(output_config.format) << "'" << std::endl;

      outs << popts.indent << skip_tag << last_tag
           << "Calo RHD buffer capacity : " << calo_rhd_buffer_capacity
           << std::endl;

      outs << popts.indent << skip_tag << last_tag
           << "Tracker RHD buffer capacity : " << tracker_rhd_buffer_capacity
           << std::endl;

      outs << popts.indent << inherit_tag(popts.inherit)
           << "Force complete RTD : " << std::boolalpha << force_complete_rtd
           << std::endl;

      out_ << outs.str();
      return;
    }

    void
    builder_config::reset()
    {
      run_id = snfee::data::INVALID_RUN_ID;
      input_configs.clear();
      {
        output_config_type empty;
        output_config = empty;
      }
      force_complete_rtd = false;
      return;
    }

    void
    builder_config::check(const builder_config& cfg_)
    {
      DT_THROW_IF(cfg_.run_id == snfee::data::INVALID_RUN_ID,
                  std::logic_error,
                  "Missing run ID!");
      DT_THROW_IF(cfg_.input_configs.size() == 0,
                  std::logic_error,
                  "Missing RHD input configurationss!");
      int icount = 0;
      for (const auto& iconf : cfg_.input_configs) {
        DT_THROW_IF(iconf.crate_model == snfee::model::CRATE_UNDEF,
                    std::logic_error,
                    "Missing RHD input #" << icount << " crate model!");
        DT_THROW_IF(iconf.crate_id == -1,
                    std::logic_error,
                    "Missing RHD input #" << icount << " crate ID!");
        DT_THROW_IF(iconf.filenames.size() == 0 and iconf.listname.empty(),
                    std::logic_error,
                    "Missing RHD input #" << icount
                                          << " filenames or listname!");
        icount++;
      }
      DT_THROW_IF(cfg_.output_config.filenames.size() == 0,
                  std::logic_error,
                  "Missing RTD output files!");
      return;
    }

    void
    builder_config::load(const std::string& rtdb_config_filename_,
                         builder_config& cfg_)
    {
      // Clear the config:
      cfg_.reset();

      std::string filename = rtdb_config_filename_;
      datatools::fetch_path_with_env(filename);
      datatools::properties rtdb_config;
      datatools::properties::read_config(filename, rtdb_config);

      // Run ID:
      if (rtdb_config.has_key("run_id")) {
        int32_t run_id = snfee::data::INVALID_RUN_ID;
        run_id = rtdb_config.fetch_integer("run_id");
        cfg_.set_run_id(run_id);
      }

      // Force complete output RTD:
      bool force_complete_rtd = false;
      if (rtdb_config.has_key("force_complete_rtd")) {
        force_complete_rtd = rtdb_config.fetch_boolean("force_complete_rtd");
        cfg_.set_force_complete_rtd(force_complete_rtd);
      }

      // RHD inputs:
      std::vector<std::string> inputs;
      if (rtdb_config.has_key("rhd.inputs")) {
        rtdb_config.fetch("rhd.inputs", inputs);
      }
      for (const std::string& inputName : inputs) {
        std::string prefix = "rhd.inputs." + inputName;
        input_config_type icfg;
        {
          std::string key = prefix + ".label";
          if (rtdb_config.has_key(key)) {
            icfg.label = rtdb_config.fetch_string(key);
          }
        }
        {
          std::string key = prefix + ".crate_model";
          if (rtdb_config.has_key(key)) {
            icfg.crate_model =
              snfee::model::crate_model_from(rtdb_config.fetch_string(key));
          }
        }
        {
          std::string key = prefix + ".crate_id";
          if (rtdb_config.has_key(key)) {
            icfg.crate_id = rtdb_config.fetch_positive_integer(key);
          }
        }
        {
          std::string key = prefix + ".listname";
          if (rtdb_config.has_key(key)) {
            icfg.listname = rtdb_config.fetch_string(key);
          }
        }
        {
          std::string key = prefix + ".filenames";
          if (rtdb_config.has_key(key)) {
            rtdb_config.fetch(key, icfg.filenames);
          }
        }
        cfg_.input_configs.push_back(icfg);
      }

      {
        // RTD output:
        output_config_type ocfg;
        {
          std::string key = "rtd.output.label";
          if (rtdb_config.has_key(key)) {
            ocfg.label = rtdb_config.fetch_string(key);
          }
        }
        {
          std::string key = "rtd.output.filenames";
          if (rtdb_config.has_key(key)) {
            rtdb_config.fetch(key, ocfg.filenames);
          }
        }
        {
          std::string key = "rtd.output.max_records_per_file";
          if (rtdb_config.has_key(key)) {
            ocfg.max_records_per_file = rtdb_config.fetch_positive_integer(key);
          }
        }
        {
          std::string key = "rtd.output.max_total_records";
          if (rtdb_config.has_key(key)) {
            ocfg.max_total_records = rtdb_config.fetch_positive_integer(key);
          }
        }
        {
          std::string key = "rtd.output.terminate_on_overrun";
          if (rtdb_config.has_key(key)) {
            ocfg.terminate_on_overrun = rtdb_config.fetch_boolean(key);
          }
        }
        cfg_.output_config = ocfg;
      }

      // Buffer capacity:
      if (rtdb_config.has_key("calo_rhd_buffer_capacity")) {
        cfg_.calo_rhd_buffer_capacity =
          rtdb_config.fetch_positive_integer("calo_rhd_buffer_capacity");
      }

      // Buffer capacity:
      if (rtdb_config.has_key("tracker_rhd_buffer_capacity")) {
        cfg_.tracker_rhd_buffer_capacity =
          rtdb_config.fetch_positive_integer("tracker_rhd_buffer_capacity");
      }

      return;
    }

    /// Print skeleton configuration
    // static
    void
    builder_config::print_skel_config(std::ostream& out_,
                                      const int run_id_,
                                      const uint16_t nbcrates_)
    {
      int run_id = run_id_;
      if (run_id < 100) {
        run_id = 99; // Test value
      }
      out_ << "# -*- mode: conf-unix; -*-                                 \n\n";

      out_ << "###########################################################\n"
              "#@config Configuration file for the snfee-rhd2rtd merger   \n"
              "###########################################################\n\n";

      out_
        << "#@description Run unique ID (mandatory)                    \n"
           "run_id : integer = "
        << run_id
        << "                        \n"
           "                                                           \n"
           "#@description List of RHD inputs (mandatory)               \n"
           "# At least one source crate (calo or tracker) must be described.\n"
           "rhd.inputs : string["
        << nbcrates_ << "] = \\\n";

      std::vector<std::string> crate_labels;
      for (int icrate = 0; icrate < (int)nbcrates_; icrate++) {
        int crate_id = icrate / 2;
        std::string crateLabel = "CaloCrate" + std::to_string(crate_id);
        if ((icrate % 2) == 1) {
          crateLabel = "TrackoCrate" + std::to_string(crate_id);
        }
        crate_labels.push_back(crateLabel);
      }
      for (int icrate = 0; icrate < (int)nbcrates_; icrate++) {
        out_ << "      \"" << crate_labels[icrate] << "\"";
        if (icrate + 1 != nbcrates_)
          out_ << " \\";
        out_ << "\n";
      }
      out_ << "\n";

      for (int icrate = 0; icrate < (int)nbcrates_; icrate++) {
        out_ << "###########################################################\n";
        int crate_id = icrate / 2;
        std::string crate_label = crate_labels[icrate];
        std::string crate_model = snfee::model::subdetector_label(
          snfee::model::SUBDETECTOR_CALORIMETER);
        std::string crate_prefix = "calo";
        if ((icrate % 2) == 1) {
          crate_model =
            snfee::model::subdetector_label(snfee::model::SUBDETECTOR_TRACKER);
          crate_prefix = "tracker";
        }
        out_ << "                                                           \n"
                "#@description Crate model associated to the '"
             << crate_label
             << "' input (mandatory) \n"
                "rhd.inputs."
             << crate_label << ".crate_model : string = \"" << crate_model
             << "\" \n"
                "                                                              "
                "          \n"
                "#@description Crate ID associated to the '"
             << crate_label
             << "' input (mandatory)\n"
                "rhd.inputs."
             << crate_label << ".crate_id : integer = " << crate_id
             << " \n"
                "                                                           \n"
                "#@description File with the list of RHD input files for the '"
             << crate_label
             << "' input   \n"
                "# This parameter is mandatory if the below parameter "
                "'filenames' is not provided.\n"
                "# The list file uses the following format (one filename per "
                "line) :\n"
                "#   \n"
                "#   # List of RHD filenames: \n"
                "#   snemo_run-"
             << run_id << "_rhd_" << crate_prefix << "-" << crate_id
             << "_part-0.data.gz \n"
                "#   snemo_run-"
             << run_id << "_rhd_" << crate_prefix << "-" << crate_id
             << "_part-1.data.gz \n"
                "#   ... \n"
                "#   snemo_run-"
             << run_id << "_rhd_" << crate_prefix << "-" << crate_id
             << "_part-99.data.gz \n"
                "#   # End.\n"
                "#   \n"
                "rhd.inputs."
             << crate_label << ".listname : string as path = \"snemo_run-"
             << run_id
             << "_rhd_files.lis\" \n"
                "                                                              "
                "               \n"
                "# #@description Explicit list of RHD input files for the '"
             << crate_label
             << "' input \n"
                "# # This parameter is mandatory if the above parameter "
                "'listname' is not provided.\n"
                "# rhd.inputs."
             << crate_label
             << ".filenames : string[3] as path = \\ \n"
                "#   \"snemo_run-"
             << run_id << "_rhd_" << crate_prefix << "-" << crate_id
             << "_part-0.data.gz\" \\    \n"
                "#   \"snemo_run-"
             << run_id << "_rhd_" << crate_prefix << "-" << crate_id
             << "_part-1.data.gz\" \\    \n"
                "#   \"snemo_run-"
             << run_id << "_rhd_" << crate_prefix << "-" << crate_id
             << "_part-2.data.gz\"       \n"
                "                                                    \n";
      }

      out_ << "###########################################################\n";
      out_
        << "#@description Explicit list of RTD output files (mandatory)     \n"
           "# This list must contains enough file to store all RTD records  \n"
           "rtd.output.filenames : string[3] as path = \\                   \n"
           "  \"snemo_run-"
        << run_id
        << "_rtd_part-0.data.gz\" \\          \n"
           "  \"snemo_run-"
        << run_id
        << "_rtd_part-1.data.gz\" \\          \n"
           "  \"snemo_run-"
        << run_id
        << "_rtd_part-2.data.gz\"             \n"
           "                                                                \n"
           "#@description Maximum number of RTD records per output files "
           "(optional)\n"
           "rtd.output.max_records_per_file : integer = 1000000                "
           "    \n"
           "                                                                   "
           "    \n"
           "#@description Maximum total number of RTD records (optional)       "
           "    \n"
           "rtd.output.max_total_records : integer = 3000000                   "
           "    \n"
           "                                                                   "
           "    \n";
      out_ << "###########################################################\n";
      out_ << "#@description Capacity of the buffer for calo RHD records "
              "(optional)   \n"
              "calo_rhd_buffer_capacity    : integer = "
           << DEFAULT_CALO_RHD_BUFFER_CAPACITY
           << "\n"
              "                                                                "
              "       \n"
              "#@description Capacity of the buffer for tracker RHD records "
              "(optional)\n"
              "tracker_rhd_buffer_capacity : integer = "
           << DEFAULT_TRACKER_RHD_BUFFER_CAPACITY
           << "\n"
              "                                                                "
              "       \n";
      out_ << "# end.";
      ;
      return;
    }

    /// Print documentation
    // static
    void
    builder_config::print_documentation(std::ostream& out_)
    {
      out_ << "================================\n"
              "The snfee-rhd2rtd merger        \n"
              "================================\n"
              "                                \n"
              ".. contents::                   \n"
              "                                \n"
              "Configuration file for the merger \n"
              "--------------------------------- \n"
              "                                                              \n"
              "Example of a snfee-rhd2rtd configuration file for the merging \n"
              "of RHD data files produced by two different crates:           \n"
              "                                                              \n"
              " - a *calorimeter* crate with ID = 0                          \n"
              " - a *tracker* crate with ID = 0                              \n"
              "                                                              \n"
              "                                                              \n"
              "Configuration file:                                           \n"
              "                                                              \n"
              ".. code:: bash                                                \n"
              "                                                              \n"
              "   #@description Run unique ID (mandatory)                    \n"
              "   run_id : integer = 104                                     \n"
              "                                                              \n"
              "   #@description List of RHD inputs (mandatory)               \n"
              "   rhd.inputs : string[2] = \"Calo0\" \"Tracker0\"            \n"
              "                                                              \n"
              "   #@description Crate model associated to the first RHD input "
              "(mandatory)\n"
              "   rhd.inputs.Calo0.crate_model : string = \"Calorimeter\"    \n"
              "                                                              \n"
              "   #@description Crate ID associated to the first RHD input "
              "(mandatory)\n"
              "   rhd.inputs.Calo0.crate_id : integer = 0                    \n"
              "                                                              \n"
              "   #@description File with the list of RHD input files for the "
              "first RHD input   \n"
              "   # This parameter is mandatory if the below parameter "
              "'filenames' is not provided.\n"
              "   rhd.inputs.Calo0.listname : string as path = "
              "\"snemo_run-104_rhd_files.lis\" \n"
              "                                                                "
              "                \n"
              "   #@description Explicit list of RHD input files for the first "
              "RHD input \n"
              "   # This parameter is mandatory if the above parameter "
              "'listname' is not provided.\n"
              "   rhd.inputs.Calo0.filenames : string[3] as path = \\  \n"
              "     \"snemo_run-104_rhd_calo-0_part-0.data.gz\" \\    \n"
              "     \"snemo_run-104_rhd_calo-0_part-1.data.gz\" \\    \n"
              "     \"snemo_run-104_rhd_calo-0_part-2.data.gz\"       \n"
              "                                                       \n"
              "   #@description Crate model associated to the second RHD input "
              "(mandatory)      \n"
              "   rhd.inputs.Tracker0.crate_model : string = \"Tracker\"       "
              "    \n"
              "                                                                "
              "    \n"
              "   #@description Crate ID associated to the second RHD input "
              "(mandatory)         \n"
              "   rhd.inputs.Tracker0.crate_id : integer = 0                   "
              "    \n"
              "                                                                "
              "    \n"
              "   #@description File with the list of RHD input files for the "
              "second RHD input \n"
              "   # This parameter is mandatory if the below parameter "
              "'filenames' is not provided.\n"
              "   rhd.inputs.Tracker0.listname : string as path = "
              "\"snemo_run-104_rhd_files.lis\" \n"
              "                                                                "
              "                   \n"
              "   #@description Explicit list of RHD input files for the "
              "second RHD input \n"
              "   # This parameter is mandatory if the above parameter "
              "'listname' is not provided.\n"
              "   rhd.inputs.Tracker0.filenames : string[7] as path = \\       "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-0.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-1.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-2.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-3.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-4.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-5.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-6.data.gz\"             "
              "   \n"
              "                                                                "
              "   \n"
              "   #@description Explicit list of RTD output files (mandatory)  "
              "   \n"
              "   # This list must contains enough file to store all RTD "
              "records  \n"
              "   rtd.output.filenames : string[3] as path = \\                "
              "   \n"
              "     \"snemo_run-104_rtd_part-0.data.gz\" \\                    "
              "   \n"
              "     \"snemo_run-104_rtd_part-1.data.gz\" \\                    "
              "   \n"
              "     \"snemo_run-104_rtd_part-2.data.gz\"                       "
              "   \n"
              "                                                                "
              "   \n"
              "   #@description Maximum number of RTD records per output files "
              "(optional)\n"
              "   rtd.output.max_records_per_file : integer = 1000000          "
              "          \n"
              "                                                                "
              "          \n"
              "   #@description Maximum total number of RTD records (optional) "
              "          \n"
              "   rtd.output.max_total_records : integer = 3000000             "
              "          \n"
              "                                                                "
              "          \n"
              "   #@description Capacity of the buffer for calo RHD records "
              "(optional)   \n"
              "   calo_rhd_buffer_capacity    : integer = 100                  "
              "          \n"
              "                                                                "
              "          \n"
              "   #@description Capacity of the buffer for tracker RHD records "
              "(optional)\n"
              "   tracker_rhd_buffer_capacity : integer = 500                  "
              "          \n"
              "..                                                              "
              "          \n"
              "                                                                "
              "          \n";
      return;
    }

  } // namespace rtdb
} // namespace snfee
