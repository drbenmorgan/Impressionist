// -*- mode: c++ ; -*-
/// \file snfee/data/run_info-serial.h

#ifndef SNFEE_DATA_RUN_INFO_SERIAL_H
#define SNFEE_DATA_RUN_INFO_SERIAL_H 1

// Ourselves:
#include <snfee/data/run_info.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

namespace snfee {
  namespace data {

    /// Serialization method
    template <class Archive>
    void run_info::serialize(Archive & ar_,
			     const unsigned int /* version */)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp("run_id", _run_id_);
      ar_ & boost::serialization::make_nvp("run_start_time", _run_start_time_);
      ar_ & boost::serialization::make_nvp("run_stop_time", _run_stop_time_);
      ar_ & boost::serialization::make_nvp("run_category", _run_category_);
      ar_ & boost::serialization::make_nvp("run_subcategory", _run_subcategory_);
      return;
    }

  } // end of namespace data
} // end of namespace snfee

#endif  // SNFEE_DATA_RUN_INFO_SERIAL_H
