//! \file snfee/io/raw_record_parser.h
//! \brief Parser for hit records (calorimeter or tracker)
//
// Copyright (c) 2016-2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RAW_RECORD_PARSER_H
#define SNFEE_IO_RAW_RECORD_PARSER_H

// Standard library:
#include <fstream>
#include <memory>
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include "calo_hit_parser.h"
#include "tracker_hit_parser.h"

namespace snfee {
  namespace io {

    //! \brief Commissioning hit parser
    class raw_record_parser {
    public:
      //! Number of header lines
      static const std::size_t NB_HIT_HEADER_LINES = 1;

      enum record_type {
        RECORD_UNDEF = 0,
        RECORD_CALO = 1,
        RECORD_TRACKER = 2,
        RECORD_TRIGGER = 3
      };

      struct config_type {
        // int16_t module_num = 0;
        int16_t crate_num = 0;
        datatools::version_id firmware_version;
        bool with_calo = true;
        bool with_tracker = true;
        bool with_calo_waveforms = true;
      };

      //! Constructor
      raw_record_parser(const config_type& cfg_,
                        const datatools::logger::priority logging_ =
                          datatools::logger::PRIO_FATAL);

      datatools::logger::priority get_logging() const;

      void set_logging(const datatools::logger::priority);

      const config_type& get_config() const;

      void set_config(const config_type&);

      //! Parse
      record_type parse(std::istream& in_,
                        snfee::data::calo_hit_record& calo_hit_,
                        snfee::data::tracker_hit_record& tracker_channel_hit_);

    private:
      void _parse_hit_header_(const std::string& header_line_, const int index);

    public:
      // Management:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

      // Configuration:
      config_type _config_;

      // Working :
      std::unique_ptr<calo_hit_parser>
        _calo_hit_parser_; //!< Handler for the calo hit parser
      std::unique_ptr<tracker_hit_parser>
        _tracker_hit_parser_;    //!< Handler for the tracker hit parser
      uint64_t _hit_id_;         //!< Current hit ID
      record_type _record_type_; //!< Current record type
      uint64_t _trigger_id_;     //!< Current trigger ID
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RAW_RECORD_PARSER_H
