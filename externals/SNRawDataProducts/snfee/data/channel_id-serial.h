// -*- mode: c++ ; -*-
/// \file snfee/data/channel_id-serial.h

#ifndef SNFEE_DATA_CHANNEL_ID_SERIAL_H
#define SNFEE_DATA_CHANNEL_ID_SERIAL_H 1

// This project:
#include <snfee/data/channel_id.h>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

namespace snfee {
  namespace data {

    /** The main Boost/Serialization template method for class channel_id
     *  @arg ar_ an archive type (ASCII, XML or binary)
     *  @arg version_ the archive version number (not used)
     */
    template <class Archive>
    void
    channel_id::serialize(Archive& ar_, const unsigned int /*version_*/)
    {
      ar_& boost::serialization::make_nvp("module_number", _module_number_);
      ar_& boost::serialization::make_nvp("crate_number", _crate_number_);
      ar_& boost::serialization::make_nvp("board_number", _board_number_);
      ar_& boost::serialization::make_nvp("channel_number", _channel_number_);
      return;
    }

  } // end of data
} // end of snfee namespace

#endif // SNFEE_DATA_CHANNEL_ID_SERIAL_H
