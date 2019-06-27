// -*- mode: c++ ; -*-
/// \file snfee/data/calo_waveform_data-serial.h

#ifndef SNFEE_DATA_CALO_WAVEFORM_DATA_SERIAL_H
#define SNFEE_DATA_CALO_WAVEFORM_DATA_SERIAL_H

// Ourselves:
#include <snfee/data/calo_waveform_data.h>

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
    void calo_waveform::serialize(Archive & ar_,
                                  const unsigned int /* version */)
    {
      ar_ & boost::serialization::make_nvp("tabfunc", _tabfunc_);
      if (Archive::is_loading::value == true) {
        _tabfunc_.export_to_vectors(_times_ns_, _amplitudes_mV_);
      }
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_waveform_baseline::serialize(Archive & ar_,
                                           const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("quality_flags",        quality_flags);
      ar_ & boost::serialization::make_nvp("baseline_start_ns",    baseline_start_ns);
      ar_ & boost::serialization::make_nvp("baseline_duration_ns", baseline_duration_ns);
      ar_ & boost::serialization::make_nvp("nb_samples",           nb_samples);
      ar_ & boost::serialization::make_nvp("baseline_mV",          baseline_mV);
      ar_ & boost::serialization::make_nvp("sigma_baseline_mV",    sigma_baseline_mV);
      ar_ & boost::serialization::make_nvp("noise_mV",             noise_mV);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_waveform_peak::serialize(Archive & ar_,
                                       const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("quality_flags",  quality_flags);
      ar_ & boost::serialization::make_nvp("time_cell",      time_cell);
      ar_ & boost::serialization::make_nvp("time_head_10_percent_cell", time_head_10_percent_cell);
      ar_ & boost::serialization::make_nvp("time_head_90_percent_cell", time_head_90_percent_cell);
      ar_ & boost::serialization::make_nvp("time_tail_90_percent_cell", time_tail_90_percent_cell);
      ar_ & boost::serialization::make_nvp("time_tail_10_percent_cell", time_tail_10_percent_cell);
      ar_ & boost::serialization::make_nvp("time_ns",                 time_ns);
      ar_ & boost::serialization::make_nvp("sigma_time_ns",           sigma_time_ns);
      ar_ & boost::serialization::make_nvp("amplitude_mV",            amplitude_mV);
      ar_ & boost::serialization::make_nvp("sigma_amplitude_mV",      sigma_amplitude_mV);
      ar_ & boost::serialization::make_nvp("time_head_10_percent_ns", time_head_10_percent_ns);
      ar_ & boost::serialization::make_nvp("time_head_90_percent_ns", time_head_90_percent_ns);
      ar_ & boost::serialization::make_nvp("time_tail_90_percent_ns", time_tail_90_percent_ns);
      ar_ & boost::serialization::make_nvp("time_tail_10_percent_ns", time_tail_10_percent_ns);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_waveform_cfd::serialize(Archive & ar_,
                                      const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("fraction",           fraction);
      ar_ & boost::serialization::make_nvp("ref_cell",           ref_cell);
      ar_ & boost::serialization::make_nvp("time_ns",            time_ns);
      ar_ & boost::serialization::make_nvp("sigma_time_ns",      sigma_time_ns);
      ar_ & boost::serialization::make_nvp("amplitude_mV",       amplitude_mV);
      ar_ & boost::serialization::make_nvp("sigma_amplitude_mV", sigma_amplitude_mV);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_waveform_charge::serialize(Archive & ar_,
                                         const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("quality_flags",    quality_flags);
      ar_ & boost::serialization::make_nvp("start_time_ns",    start_time_ns);
      ar_ & boost::serialization::make_nvp("stop_time_ns",     stop_time_ns);
      ar_ & boost::serialization::make_nvp("charge_nVs",       charge_nVs);
      ar_ & boost::serialization::make_nvp("sigma_charge_nVs", sigma_charge_nVs);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_waveform_firmware_data::serialize(Archive & ar_,
                                                const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("baseline_mV",  baseline_mV);
      ar_ & boost::serialization::make_nvp("peak_mV",      peak_mV);
      ar_ & boost::serialization::make_nvp("peak_time_ns", peak_time_ns);
      ar_ & boost::serialization::make_nvp("peak_time_ns", peak_time_ns);
      ar_ & boost::serialization::make_nvp("charge_nVs",   charge_nVs);
      ar_ & boost::serialization::make_nvp("cfd_time_ns",  cfd_time_ns);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calo_waveform_info::serialize(Archive & ar_,
                                       const unsigned int /* version */)
    { 
      ar_ & boost::serialization::make_nvp("waveform",     waveform);
      ar_ & boost::serialization::make_nvp("has_fw_data",  has_fw_data);
      ar_ & boost::serialization::make_nvp("fw_data",      fw_data);
      ar_ & boost::serialization::make_nvp("has_baseline", has_baseline);
      ar_ & boost::serialization::make_nvp("baseline",     baseline);
      ar_ & boost::serialization::make_nvp("has_peak",     has_peak);
      ar_ & boost::serialization::make_nvp("peak",         peak);
      ar_ & boost::serialization::make_nvp("has_cfd",      has_cfd);
      ar_ & boost::serialization::make_nvp("cfd",          cfd);
      ar_ & boost::serialization::make_nvp("has_charge",   has_charge);
      ar_ & boost::serialization::make_nvp("charge",       charge);
      return;
    }

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_CALO_WAVEFORM_DATA_SERIAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
