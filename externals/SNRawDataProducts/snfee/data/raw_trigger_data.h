//! \file  snfee/data/raw_trigger_data.h
//! \brief Description of the SuperNEMO raw trigger data
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

#ifndef SNFEE_DATA_RAW_TRIGGER_DATA_H
#define SNFEE_DATA_RAW_TRIGGER_DATA_H

// Standard Library:
#include <iostream>
#include <vector>

// Third Party Libraries:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/data/calo_hit_record.h>
#include <snfee/data/tracker_hit_record.h>
#include <snfee/data/trigger_record.h>
#include <snfee/utils.h>

namespace snfee {
  namespace data {

    /// \brief SuperNEMO raw trigger data (RTD)
    class raw_trigger_data : public datatools::i_tree_dumpable,
                             public datatools::i_serializable {
    public:
      /// Default constructor
      raw_trigger_data();

      /// Destructor
      virtual ~raw_trigger_data();

      /// Check if the record is complete
      bool is_complete() const;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::data::raw_trigger_data rawTrigData
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Calorimeter data record:");
      /// poptions.put("indent", ">>> ");
      /// myCaloData.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream& out_ = std::clog,
                              const boost::property_tree::ptree& options_ =
                                empty_options()) const override;

      //! Reset the record
      void invalidate();

      //! Check if run ID is set
      bool has_run_id() const;

      //! Return the run ID
      int32_t get_run_id() const;

      //! Set the run ID
      void set_run_id(const int32_t);

      //! Check if trigger ID is set
      bool has_trigger_id() const;

      //! Return the trigger ID
      int32_t get_trigger_id() const;

      //! Set the trigger ID
      void set_trigger_id(const int32_t);

      //! Check if the trigger record is set
      bool has_trig() const;

      //! Set the trigger record
      void set_trig(const const_trigger_record_ptr&);

      //! Return the handle for the trigger record
      const const_trigger_record_ptr& get_trig() const;

      //! Return the trigger record
      const trigger_record& get_trig_cref() const;

      //! Append a new calo hit record
      void append_calo_hit(const const_calo_hit_record_ptr&);

      //! Return the collection of calorimeter hit records
      const std::vector<const_calo_hit_record_ptr>& get_calo_hits() const;

      //! Append a new tracker hit record
      void append_tracker_hit(const const_tracker_hit_record_ptr&);

      //! Return the collection of tracker hit records
      const std::vector<const_tracker_hit_record_ptr>& get_tracker_hits() const;

      //! Print
      friend std::ostream& operator<<(std::ostream& out_,
                                      const raw_trigger_data& rtd_);

    private:
      int32_t _run_id_ = INVALID_RUN_ID;         ///< Run ID
      int32_t _trigger_id_ = INVALID_TRIGGER_ID; ///< Trigger ID
      const_trigger_record_ptr _trig_; ///< Handle for the trigger record
      std::vector<const_calo_hit_record_ptr>
        _calo_hits_; ///< Collection of handles for calorimeter hit records
      std::vector<const_tracker_hit_record_ptr>
        _tracker_hits_; ///< Collection of handles for tracker hit records

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // namespace data
} // namespace snfee

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snfee::data::raw_trigger_data,
                        "snfee::data::raw_trigger_data")

#endif // SNFEE_DATA_CALO_HIT_RECORD_H
