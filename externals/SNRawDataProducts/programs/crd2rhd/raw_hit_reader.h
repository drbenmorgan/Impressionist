//! \file snfee/io/raw_hit_reader.h
//! \brief Wavecatcher readout raw event reader
//
// Copyright (c) 2016-2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RAW_HIT_READER_H
#define SNFEE_IO_RAW_HIT_READER_H

// Standard library:
#include <fstream>
#include <memory>
#include <string>

// Third party:
// - Boost:
#include <boost/utility.hpp>
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include "raw_record_parser.h"
#include "raw_run_header.h"

namespace snfee {
  namespace io {

    //! \brief Commissioning raw hit record file reader
    class raw_hit_reader : private boost::noncopyable {
    public:
      static const std::size_t HEADER_NBLINES = 9;

      /// \brief Reader configuration
      struct config_type {
        std::string input_filename;
        int16_t crate_num = -1;
        bool with_calo = true;
        bool with_tracker = true;
        bool with_calo_waveforms = true;
      };

      //! Default constructor
      raw_hit_reader();

      //! Return the logging level
      datatools::logger::priority get_logging() const;

      //! Set the logging level
      void set_logging(const datatools::logger::priority);

      //! Return the config
      const config_type& get_config() const;

      //! Set the config
      void set_config(const config_type&);

      //! Chek if a next hit is available
      bool has_next_hit() const;

      //! Load the next hit
      raw_record_parser::record_type load_next_hit(
        snfee::data::calo_hit_record& calo_hit_,
        snfee::data::tracker_hit_record& tracker_hit_);

      bool has_run_header() const;

      //! Load the run header
      void load_run_header(raw_run_header& header_);

      //! Check initialization flag
      bool is_initialized() const;

      //! Initialize the reader
      void initialize();

      //! Reset the reader
      void reset();

      //! Print
      void print(std::ostream& out_) const;

    private:
      void _init_input_file_();

      void _reset_input_file_();

      void _init_parser_();

      void _reset_parser_();

      void _init_header_();

      void _reset_header_();

      void _decode_header_(const std::string& hline_, const int index_);

    private:
      // Configuration:
      config_type _config_;

      // Management:
      bool _initialized_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

      // Working:
      std::unique_ptr<std::ifstream> _fin_; //!< Handle to the input file stream
      std::unique_ptr<raw_run_header>
        _header_; //!< Handle to the input file header
      std::unique_ptr<raw_record_parser>
        _record_parser_; //!< Raw hit record parser
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RAW_HIT_READER_H
