//! \file snfee/io/tracker_hit_parser.h
//! \brief Parser for a raw tracker hit record
//
// Copyright (c) 2016-2018 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_TRACKER_HIT_PARSER_H
#define SNFEE_IO_TRACKER_HIT_PARSER_H

// Standard library:
#include <fstream>
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/version_id.h>

// This project:
#include <snfee/data/tracker_hit_record.h>

namespace snfee {
  namespace io {

    //! \brief Commissioning tracker hit parser
    class tracker_hit_parser {
    public:
      enum format_version_type {
        FORMAT_INVALID = 0,
        FORMAT_BEFORE_2_4 = 1,
        FORMAT_FROM_2_4 = 2
      };

      struct config_type {
        // int16_t module_num = 0;
        int16_t crate_num = 0;
        datatools::version_id firmware_version;
      };

      struct hit_data_type {
        uint16_t slot_id = 0xFFFF;
        uint16_t feast_id = 0xFFFF;
        uint16_t channel_id = 0xFFFF;
        std::string channel_type = "";
        std::string timestamp_type = "";
        uint64_t timestamp_value = 0;
        double timestamp_ns = 0.0;
        double unix_time = 0.0;
      };

      //! Constructor
      tracker_hit_parser(
        const config_type& cfg_,
        const datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

      datatools::logger::priority get_logging() const;

      void set_logging(const datatools::logger::priority);

      const config_type& get_config() const;

      void set_config(const config_type&);

      //! Parse
      bool parse(std::istream& in_, snfee::data::tracker_hit_record& hit_);

    private:
      void _parse_timestamp_(const std::string& data_line_,
                             hit_data_type& hit_data_);

    private:
      // Management:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

      // Configuration:
      config_type _config_;

      // Working:
      format_version_type _format_ = FORMAT_INVALID;
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_TRACKER_HIT_PARSER_H
