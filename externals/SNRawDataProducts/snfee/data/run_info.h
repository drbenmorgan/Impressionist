//! \file snfee/data/run_info.h
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_DATA_RUN_INFO_H
#define SNFEE_DATA_RUN_INFO_H

// Standard Library:
#include <string>

// Third party:
// - Boost:
#include <boost/date_time/posix_time/posix_time.hpp>

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/data/utils.h>

namespace snfee {
  namespace data {

    /// \brief Raw run informations
    class run_info : public datatools::i_tree_dumpable,
                     public datatools::i_serializable {
    public:
      /// Constructor
      run_info();

      /// Check if the unique run ID is set
      bool has_run_id() const;

      /// Return the unique ID of the run
      int32_t get_run_id() const;

      /// Set the unique run ID
      void set_run_id(const int32_t);

      /// Check if the run start time is set
      bool has_run_start_time() const;

      /// Set the run start time
      void set_run_start_time(const boost::posix_time::ptime&);

      /// Return the run start time
      const boost::posix_time::ptime& get_run_start_time() const;

      /// Check if the run stop time is set
      bool has_run_stop_time() const;

      /// Set the run stop time
      void set_run_stop_time(const boost::posix_time::ptime&);

      /// Return the run stop time
      const boost::posix_time::ptime& get_run_stop_time() const;

      /// Check if the run category is set
      bool has_run_category() const;

      /// Return the run category
      const std::string& get_run_category() const;

      /// Set the run category
      void set_run_category(const std::string&);

      /// Check if the run sub-category is set
      bool has_run_subcategory() const;

      /// Return the run sub-category
      const std::string& get_run_subcategory() const;

      /// Set the run sub-category
      void set_run_subcategory(const std::string&);

      /// Check if minimal attributes are set
      bool is_complete() const;

      /// Reset the run informations
      void reset();

      /// Smart print
      virtual void print_tree(std::ostream& out_ = std::clog,
                              const boost::property_tree::ptree& options_ =
                                empty_options()) const override;

    private:
      int32_t _run_id_ = INVALID_RUN_ID;         ///< Unique run ID (mandatory)
      boost::posix_time::ptime _run_start_time_; ///< Start run time (mandatory)
      boost::posix_time::ptime _run_stop_time_;  ///< Stop run time (optional)
      std::string _run_category_;                ///< Run category (optional)
      std::string _run_subcategory_;             ///< Run subcategory (optional)

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // namespace data
} // namespace snfee

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snfee::data::run_info, "snfee::data::run_info")

#endif // SNFEE_DATA_RUN_INFO_H
