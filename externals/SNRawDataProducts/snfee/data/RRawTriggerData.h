//! \file  snfee/data/RRawTriggerData.h
//! \brief Offline form of the SuperNEMO raw trigger data
//
// Copyright (c) 2019 Ben Morgan <Ben.Morgan@warwick.ac.uk>
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

#ifndef SNFEE_RRAW_TRIGGER_DATA_H
#define SNFEE_RRAW_TRIGGER_DATA_H

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
    // Typedefs that need moving to headers.
    typedef trigger_record TriggerRecord;
    typedef std::vector<calo_hit_record> CaloRecordCollection;
    typedef std::vector<tracker_hit_record> TrackerRecordCollection;

    /// \brief Offline form of the SuperNEMO raw trigger data (RTD)
    class RRawTriggerData : public datatools::i_tree_dumpable,
                            public datatools::i_serializable {
    public:
      RRawTriggerData() = default;
      RRawTriggerData(int32_t run,
                      int32_t trigger,
                      const TriggerRecord& tr,
                      const CaloRecordCollection& crc,
                      const TrackerRecordCollection& trc)
        : runID{run}
        , triggerID{trigger}
        , trigger{tr}
        , caloRecords{crc}
        , trackerRecords{trc}
      {}

      /// Check if the record is complete
      bool isComplete() const;

      //! Check if run ID is set
      bool hasRunID() const;

      //! Return the run ID
      int32_t getRunID() const;

      //! Check if trigger ID is set
      bool hasTriggerID() const;

      //! Return the trigger ID
      int32_t getTriggerID() const;

      //! Return the trigger record
      const trigger_record& getTriggerRecord() const;

      //! Return the collection of calorimeter hit records
      const CaloRecordCollection& getCaloRecords() const;

      //! Return the collection of tracker hit records
      const TrackerRecordCollection& getTrackerRecords() const;

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

    private:
      int32_t runID = INVALID_RUN_ID;         ///< Run ID
      int32_t triggerID = INVALID_TRIGGER_ID; ///< Trigger ID
      TriggerRecord trigger;                  ///< The trigger record
      CaloRecordCollection
        caloRecords; ///< Collection of calorimeter hit records
      TrackerRecordCollection
        trackerRecords; ///< Collection tracker hit records

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // namespace data
} // namespace snfee

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snfee::data::RRawTriggerData,
                        "snfee::data::RRawTriggerData")

#endif
