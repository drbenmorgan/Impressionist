// -*- mode: c++ ; -*-
/// \file snfee/data/trigger_record-serial.h

#ifndef SNFEE_DATA_TRIGGER_RECORD_SERIAL_H
#define SNFEE_DATA_TRIGGER_RECORD_SERIAL_H 1

// Ourselves:
#include <snfee/data/trigger_record.h>

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
    trigger_record::serialize(Archive& ar_, const unsigned int /* version */)
    {
      ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_& boost::serialization::make_nvp("trigger_id", _trigger_id_);
      ar_& boost::serialization::make_nvp("trigger_mode", _trigger_mode_);
      ar_& boost::serialization::make_nvp("l2_clocktick_1600ns",
                                          _l2_clocktick_1600ns_);
      return;
    }

  } // end of namespace data
} // end of namespace snfee

#endif // SNFEE_DATA_TRIGGER_RECORD_SERIAL_H
