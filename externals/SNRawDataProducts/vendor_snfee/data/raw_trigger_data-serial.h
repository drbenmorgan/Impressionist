// -*- mode: c++ ; -*-
/// \file snfee/data/raw_trigger_data-serial.h

#ifndef SNFEE_DATA_RAW_TRIGGER_DATA_SERIAL_H
#define SNFEE_DATA_RAW_TRIGGER_DATA_SERIAL_H 1

// Ourselves:
#include <snfee/data/raw_trigger_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace snfee {
  namespace data {

    /// Serialization method
    template <class Archive>
      void raw_trigger_data::serialize(Archive & ar_,
                                       const unsigned int /* version */)
      {
        ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
        ar_ & boost::serialization::make_nvp("run_id",       _run_id_);
        ar_ & boost::serialization::make_nvp("trigger_id",   _trigger_id_);
        ar_ & boost::serialization::make_nvp("trig",         _trig_);
        ar_ & boost::serialization::make_nvp("calo_hits",    _calo_hits_);
        ar_ & boost::serialization::make_nvp("tracker_hits", _tracker_hits_);
        return;
      }

  } // end of namespace data
} // end of namespace snfee

#endif  // SNFEE_DATA_RAW_TRIGGER_DATA_SERIAL_H
