// -*- mode: c++ ; -*-
/// \file snfee/data/tracker_hit_record-serial.h

#ifndef SNFEE_DATA_TRACKER_HIT_RECORD_SERIAL_H
#define SNFEE_DATA_TRACKER_HIT_RECORD_SERIAL_H 1

// Ourselves:
#include <snfee/data/tracker_hit_record.h>

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
    tracker_hit_record::serialize(Archive& ar_,
                                  const unsigned int /* version */)
    {
      ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_& boost::serialization::make_nvp("hit_num", _hit_num_);
      ar_& boost::serialization::make_nvp("trigger_id", _trigger_id_);
      // ar_ & boost::serialization::make_nvp("module_num",  _module_num_);
      ar_& boost::serialization::make_nvp("crate_num", _crate_num_);
      ar_& boost::serialization::make_nvp("board_num", _board_num_);
      ar_& boost::serialization::make_nvp("chip_num", _chip_num_);
      ar_& boost::serialization::make_nvp("channel_num", _channel_num_);
      ar_& boost::serialization::make_nvp("channel_category",
                                          _channel_category_);
      ar_& boost::serialization::make_nvp("timestamp_category",
                                          _timestamp_category_);
      ar_& boost::serialization::make_nvp("timestamp", _timestamp_);
      return;
    }

  } // end of namespace data
} // end of namespace snfee

#endif // SNFEE_DATA_TRACKER_HIT_RECORD_SERIAL_H
