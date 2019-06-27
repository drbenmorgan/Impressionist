// -*- mode: c++ ; -*-
/// \file snfee/data/calo_channel_traits-serial.h

#ifndef SNFEE_DATA_CALO_CHANNEL_TRAITS_SERIAL_H
#define SNFEE_DATA_CALO_CHANNEL_TRAITS_SERIAL_H 1

// Ourselves:
#include <snfee/data/calo_channel_traits.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

namespace snfee {
  namespace data {

    /// Serialization method
    template <class Archive>
    void charge_range::serialize(Archive & ar_,
                                 const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("lower_charge_nVs", _lower_charge_nVs_);
      ar_ & boost::serialization::make_nvp("upper_charge_nVs", _upper_charge_nVs_);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_channel_traits::charge_range_info::serialize(Archive & ar_,
                                                           const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("range_id",       range_id);
      ar_ & boost::serialization::make_nvp("ref_time_ns",    ref_time_ns);
      ar_ & boost::serialization::make_nvp("mean_signal",    mean_signal);
      ar_ & boost::serialization::make_nvp("q2a_factor",     q2a_factor);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_channel_traits::hv_regime_info::serialize(Archive & ar_,
                                                        const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("nominal_hv_V",       _nominal_hv_V_);
      ar_ & boost::serialization::make_nvp("baseline",           _baseline_);
      ar_ & boost::serialization::make_nvp("charge_ranges",      _charge_ranges_);
      ar_ & boost::serialization::make_nvp("charge_range_infos", _charge_range_infos_);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_channel_traits::serialize(Archive & ar_,
                                        const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("ch_id",           _ch_id_);
      ar_ & boost::serialization::make_nvp("flags",           _flags_);
      ar_ & boost::serialization::make_nvp("hv_regime_infos", _hv_regime_infos_);
      ar_ & boost::serialization::make_nvp("pmt_id",          _pmt_id_);
      return;
    }

  } // end of namespace data 
}  // end of namespace snfee

#endif // SNFEE_DATA_CALO_CHANNEL_TRAITS_SERIAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
