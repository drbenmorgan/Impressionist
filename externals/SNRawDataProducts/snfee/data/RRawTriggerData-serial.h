// -*- mode: c++ ; -*-
/// \file snfee/data/raw_trigger_data-serial.h

#ifndef SNFEE_RRAW_TRIGGER_DATA_SERIAL_H
#define SNFEE_RRAW_TRIGGER_DATA_SERIAL_H 1

// Ourselves:
#include "snfee/data/RRawTriggerData.h"

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>

namespace snfee {
  namespace data {
    /// Serialization method
    template <class Archive>
    void
    RRawTriggerData::serialize(Archive& ar_, const unsigned int /* version */)
    {
      ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_& boost::serialization::make_nvp("runID", runID);
      ar_& boost::serialization::make_nvp("triggerID", triggerID);
      ar_& boost::serialization::make_nvp("trigger", trigger);
      ar_& boost::serialization::make_nvp("caloRecords", caloRecords);
      ar_& boost::serialization::make_nvp("trackerRecords", trackerRecords);
    }
  } // end of namespace data
} // end of namespace snfee

#endif // SNFEE_DATA_RAW_TRIGGER_DATA_SERIAL_H
