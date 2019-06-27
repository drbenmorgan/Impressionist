//! \file  snfee/rtdb/builder.h
//! \brief Raw trigger data (RTD) builder from the SuperNEMO raw hit data records (RHD) 
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

#ifndef SNFEE_RTDB_BUILDER_H
#define SNFEE_RTDB_BUILDER_H

// Standard Library:
#include <string>
#include <vector>
#include <memory>

// Third Party Libraries:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/has_trigger_id_interface.h>
#include <snfee/model/utils.h>
#include <snfee/rtdb/builder_config.h>

namespace snfee {
  namespace rtdb {

    /// \brief Raw trigger data (RTD) builder
    class builder
      : public datatools::i_tree_dumpable
    {

    public:
 
      /// Constructor
      builder();

      /// Destructor
      virtual ~builder();

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;
      
      /// Set the logging priority threshold
      void set_logging(const datatools::logger::priority);

      /// Set the configuration
      void set_config(const builder_config &);
      
      /// Return the configuration
      const builder_config & get_config() const;

      /// Check is the builder is initialized
      bool is_initialized() const;

      /// Initialize the builder
      void initialize();

      /// Run the builder
      void run();
      
      /// Terminate the builder
      void terminate();
 
      /// Check is the builder is stopped
      bool is_stopped() const;

      /// Stop request
      void stop();
      
      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::rtdb::builder rtdBuilder
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "RTD builder:");
      /// poptions.put("indent", ">>> ");
      /// rtdBuilder.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

      /// \brief Worker category
      enum worker_category_type {
        WORKER_UNDEF        = 0, 
        WORKER_INPUT_RHD    = 1, 
        WORKER_OUTPUT_RTD   = 2
      };
      
      /// \brief Worker results (statistics)
      struct worker_results_type
      { 
        worker_category_type           category = WORKER_UNDEF;
        snfee::model::crate_model_type crate_model = snfee::model::CRATE_UNDEF;
        std::size_t                    processed_records_counter1 = 0;
        std::size_t                    processed_records_counter2 = 0;
      };

      /// Return the results of the run
      const std::vector<worker_results_type> & get_results() const;
      
    private:

      void _at_run_();

      void _at_init_();

      void _at_terminate_();

    public:
      
      struct pimpl_type; ///!< Private implementation type

    private:

      // Management
      bool _initialized_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      bool _stop_request_ = false;
      
      // Configuration
      builder_config _config_;

      // Results:
      std::vector<worker_results_type> _results_;
      
      // Working data:
      std::unique_ptr<pimpl_type> _pimpl_;
      
    };
    
  } // namespace rtdb
} // namespace snfee

#endif // SNFEE_RTDB_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
