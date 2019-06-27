//! \file  snfee/rtdb/builder_config.h
//! \brief Builder configiguration
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of SNFrontEndElectronics.
//
// SNFrontEndElectronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SNFrontEndElectronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SNFrontEndElectronics. If not, see <http://www.gnu.org/licenses/>.

#ifndef SNFEE_RTDB_BUILDER_CONFIG_H
#define SNFEE_RTDB_BUILDER_CONFIG_H

// Standard Library:
#include <string>
#include <vector>

// Third Party Libraries:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/data/utils.h>
#include <snfee/model/utils.h>

namespace snfee {
  namespace rtdb {

    /// \brief Configuration for the raw trigger data (RTD) builder
    class builder_config : public datatools::i_tree_dumpable {
    public:
      static const uint32_t DEFAULT_CALO_RHD_BUFFER_CAPACITY = 100;
      static const uint32_t DEFAULT_TRACKER_RHD_BUFFER_CAPACITY = 500;

      /// \brief Input format
      enum format_type { FORMAT_UNDEF = 0, FORMAT_BOOST_SERIAL = 1 };

      static std::string format_label(const format_type);

      /// \brief Input configuration description
      struct input_config_type {
        std::string label; /// Identification (human friendly string)
        snfee::model::crate_model_type crate_model =
          snfee::model::CRATE_UNDEF; ///< Crate model
        int32_t crate_id = -1;       ///< Crate ID
        std::string
          listname; ///< Name of a file containing the list of input RHD files
        std::vector<std::string>
          filenames;        ///< Explicit list of input RHD files
        format_type format; ///< Format description (unused)
      };

      /// \brief Output configuration description
      struct output_config_type {
        std::string label; /// Identification (human friendly string)
        std::vector<std::string>
          filenames; ///< Explicit list of output RTD files
        std::size_t max_records_per_file =
          0; ///< Maximum number of RTD records per output file
        std::size_t max_total_records =
          0; ///< Maximum total number of RTD records
        bool terminate_on_overrun =
          false; ///< Flag to silently terminate the overrunning writer (dont'
                 ///< throw if set, unused)
        format_type format; ///< Format description (unused)
      };

      /// Default constructor
      builder_config();

      /// Destructor
      virtual ~builder_config();

      /// Set the run ID
      void set_run_id(const int32_t rid_);

      /// Return the run ID
      int32_t get_run_id() const;

      /// Check if complete RTD is forced
      bool is_force_complete_rtd() const;

      /// Set the forced complete RTD flag
      void set_force_complete_rtd(bool f_);

      /// Check if the output config
      bool has_output_config() const;

      /// Set the output config
      void set_output_config(const std::string& label_,
                             const std::vector<std::string>& filepaths_,
                             const std::size_t max_records_per_file_,
                             const std::size_t max_total_records_,
                             const format_type format_ = FORMAT_UNDEF);

      /// Return the output config
      const output_config_type& get_output_config() const;

      /// Add an input configuration
      void add_input_config(const std::string& label_,
                            const snfee::model::crate_model_type crate_model_,
                            const int32_t crate_id_,
                            const std::vector<std::string>& filepaths_,
                            const format_type format_ = FORMAT_UNDEF);

      /// Add an input configuration
      void add_input_config(const std::string& label_,
                            const snfee::model::crate_model_type crate_model_,
                            const int32_t crate_id_,
                            const std::string& listpath_,
                            const format_type format_ = FORMAT_UNDEF);

      /// Return the collection of input configurations
      const std::vector<input_config_type>& get_input_configs() const;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::rtdb::builder_config rtdBuilderCfg
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "RTD builder config:");
      /// poptions.put("indent", ">>> ");
      /// rtdBuilderCfg.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(
        std::ostream& out_ = std::clog,
        const boost::property_tree::ptree& options_ = empty_options()) const;

      /// Reset the configuration
      void reset();

      /// Check the validity and completeness of the configuration
      static void check(const builder_config& cfg_);

      /// Load the configuration from a file
      static void load(const std::string& config_filename_,
                       builder_config& cfg_);

      /// Print documentation
      static void print_skel_config(std::ostream& out_,
                                    const int run_id_ = 100,
                                    const uint16_t nbcrates_ = 2);

      /// Print documentation
      static void print_documentation(std::ostream& out);

    public:
      int32_t run_id = snfee::data::INVALID_RUN_ID; ///< Run identifier
      std::vector<input_config_type>
        input_configs; ///< List of configuration for RHD inputs
      output_config_type output_config; ///< Configuration for RTD output
      bool force_complete_rtd =
        false; ///< Flag to force complete RTD (for production runs)
      uint32_t calo_rhd_buffer_capacity =
        DEFAULT_CALO_RHD_BUFFER_CAPACITY; ///< Input RHD buffer capacity for
                                          ///< calorimeter hits
      uint32_t tracker_rhd_buffer_capacity =
        DEFAULT_TRACKER_RHD_BUFFER_CAPACITY; ///< Input RHD buffer capacity for
                                             ///< tracker hits
      bool accept_unsorted_records = false;
      std::size_t unsorted_records_min_popping_safety_depth = 3;
    };

  } // namespace rtdb
} // namespace snfee

#endif // SNFEE_RTDB_BUILDER_CONFIG_H
