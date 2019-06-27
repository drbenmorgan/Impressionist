//! \file snfee/io/raw_run_header.hpp
//
// Copyright (c) 2016-2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RAW_RUN_HEADER_H
#define SNFEE_IO_RAW_RUN_HEADER_H

// Standard library:
#include <string>
#include <iostream>

// Third Party Libraries:
#include <bayeux/datatools/i_tree_dump.h>

namespace snfee {
  namespace io {

    /// \brief Run header
    struct raw_run_header
      : public datatools::i_tree_dumpable
    {
      /// Default constructor
      raw_run_header();

      /// Destructor
      virtual ~raw_run_header();

      /// Check if the run header is valid
      bool is_complete() const;

      /// Set the software version
      void set_software_version(const std::string & sw_version_);
 
      /// Make a raw run header
      void make(const std::string & sw_version_,
                const double unix_time_,
                const std::string & date_,
                const std::string & time_,
                const std::string & data_type_);
      
      /// Reset
      void reset();

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

    private:

      void _reset_();

    public:

      std::string software_version;       ///< The parsed software version
      uint32_t    software_major_version; ///< The parsed major software version
      uint32_t    software_minor_version; ///< The parsed minor software version
      double      unix_time;        ///< The parsed unix time
      std::string date;             ///< The parsed date
      std::string time;             ///< The parsed time
      std::string data_type;        ///< The parsed data type

    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RAW_RUN_HEADER_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
