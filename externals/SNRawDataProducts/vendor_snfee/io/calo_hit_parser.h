//! \file snfee/io/calo_hit_parser.h
//! \brief Parser for a calorimeter hit record
//
// Copyright (c) 2016-2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_CALO_HIT_PARSER_H
#define SNFEE_IO_CALO_HIT_PARSER_H

// Standard library:
#include <string>
#include <iostream>
#include <limits>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/version_id.h>

// This project:
#include <snfee/data/calo_hit_record.h>

namespace snfee {
  namespace io {

    //! \brief Commissioning parser for calorimeter hit records
    class calo_hit_parser
    {
    public:

      enum format_version_type {
        FORMAT_INVALID    = 0,
        FORMAT_BEFORE_2_3 = 1,
        FORMAT_FROM_2_3   = 2,
        FORMAT_FROM_2_4   = 3
      };

      struct config_type {
        // int16_t module_num = 0;
        int16_t crate_num  = 0;
        datatools::version_id firmware_version;
        bool with_waveforms = true;
      };
      
      //! Number of header lines
      static const std::size_t NB_CALO_HEADER_LINES = 1;

      //! Number of calorimeter channel per record
      static const std::size_t NB_CALO_CHANNELS     = 2;

      //! Constructor
      calo_hit_parser(const config_type & cfg_,
                      const datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);

      datatools::logger::priority get_logging() const;

      void set_logging(const datatools::logger::priority);

      const config_type & get_config() const;

      void set_config(const config_type &);
    
      //! Parse
      bool parse(std::istream & in_, snfee::data::calo_hit_record & hit_);

      /// \brief SuperNEMO Crate Software output
      struct header_type
      {
        uint32_t  slot_id       = std::numeric_limits<uint32_t>::max();
        uint32_t  channel_id    = std::numeric_limits<uint32_t>::max();
        uint16_t  lto_flag      = 0; ///< Boolean
        uint16_t  ht_flag       = 0; ///< Boolean
        uint32_t  event_id      = std::numeric_limits<uint32_t>::max();
        uint64_t  raw_tdc       = std::numeric_limits<uint64_t>::max();
        double    raw_tdc_ns    = std::numeric_limits<double>::quiet_NaN(); ///< Software computed (not raw data)
        uint32_t  lt_trig_count = 0;
        uint32_t  lt_time_count = 0;

        int32_t   raw_baseline  = std::numeric_limits<int32_t>::max();
        double    baseline_volt = std::numeric_limits<double>::quiet_NaN(); ///< Software computed (not raw data)
        
        int16_t   raw_peak      = std::numeric_limits<int16_t>::max();
        uint16_t  peak_cell     = std::numeric_limits<uint16_t>::max();
        double    peak_volt     = std::numeric_limits<double>::quiet_NaN(); ///< Software computed (not raw data)

        int32_t   raw_charge         = std::numeric_limits<int32_t>::max();
        double    charge_picocoulomb = std::numeric_limits<double>::quiet_NaN(); ///< Software computed (not raw data)
        int16_t   charge_overflow    = 0; ///< Boolean

        uint32_t  rising_cell    = std::numeric_limits<uint32_t>::max();
        uint32_t  rising_offset  = std::numeric_limits<uint32_t>::max();
        double    rising_ns      = std::numeric_limits<double>::quiet_NaN(); ///< Software computed (not raw data)

        uint32_t  falling_cell   = std::numeric_limits<uint32_t>::max();
        uint32_t  falling_offset = std::numeric_limits<uint32_t>::max();
        double    falling_ns     = std::numeric_limits<double>::quiet_NaN(); ///< Software computed (not raw data)

        uint16_t  l2_id          = std::numeric_limits<uint16_t>::max();
        uint16_t  fcr            = std::numeric_limits<uint16_t>::max();
        double    unix_time      = std::numeric_limits<double>::quiet_NaN(); ///< Software computed (not raw data)

        void print(std::ostream & out_, const std::string & indent_ = "") const;
        
      };
 
      /// Print
      void print(std::ostream & out_, const std::string & indent_ = "") const;
   
    private:
      
      /// Header parsing
      void _parse_header_(const std::string & header_line_,
                          const int index_,
                          header_type & header_);
      
      
      /// Header parsing
      void _parse_header_from_2_4_(const std::string & header_line_,
                                   const int index_,
                                   header_type & header_);
      
      /// Header parsing
      void _parse_header_from_2_3_(const std::string & header_line_,
                                   const int index_,
                                   header_type & header_);

      /// Header parsing
      void _parse_header_legacy_(const std::string & header_line_,
                                 const int index_,
                                 header_type & header_);

      /// Waveform samples parsing for one SAMLONG channel
      void _parse_waveform_(const std::string & samples_line_,
                            const uint16_t channel_index_,
                            snfee::data::calo_hit_record::waveforms_record & waveforms_);

    private:
    
      // Management:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

      // Configuration:
      config_type         _config_;
      
      // Working:
      format_version_type _format_ = FORMAT_INVALID;
      // status_type _status_;
      header_type _current_header_;
      
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_CALO_HIT_PARSER_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
