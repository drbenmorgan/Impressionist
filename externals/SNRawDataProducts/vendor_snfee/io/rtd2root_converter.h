//! \file snfee/io/rtd2root_converter.h
//! \brief RTD to Root converter
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RTD2ROOT_CONVERTER_H
#define SNFEE_IO_RTD2ROOT_CONVERTER_H

// Standard library:
#include <string>
#include <vector>
#include <memory>

// Third party:
// - Boost:
#include <boost/utility.hpp>
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project
#include <snfee/io/multifile_data_reader.h>
#include <snfee/data/rtd_selection.h>

namespace snfee {
  namespace io {

    //! \brief RTD to Root converter
    class rtd2root_converter
      : private boost::noncopyable
    {
    public:
      
      /// \brief Configuration data:
      struct config_type
      {
        std::string input_rtd_listname; ///< Name of a file with the sequence of RTD input filenames
        std::vector<std::string> input_rtd_filenames; ///< Sequence of RTD input filenames
        std::string output_root_filename;     ///< Output Root filename
        std::size_t max_total_records = 0;    ///< Max number of converted RTD records
        /// Calorimeter hit record selection:
        snfee::data::calo_selection::config_type calo_sel_config;
      };
      
      //! Default constructor
      rtd2root_converter();

      //! Destructor
      virtual ~rtd2root_converter();

      //! Set the logging priority
      void set_logging(const datatools::logger::priority);
      
      //! Set the configuration
      void set_config(const config_type &);
      
      //! Check if the converter is initialized
      bool is_initialized() const;

      //! Initialize the converter
      void initialize();

      //! Run the converter
      void run();
      
      //! Reset the converter
      void terminate();
      
    private:

      // Management:
      bool _initialized_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      
      // Configuration:
      config_type _config_;  ///< Configuration

      // Working data:
      struct pimpl_type;
      std::unique_ptr<pimpl_type> _pimpl_; ///< Private working data
    
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RTD2ROOT_CONVERTER_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
