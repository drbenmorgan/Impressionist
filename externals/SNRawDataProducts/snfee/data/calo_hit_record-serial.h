// -*- mode: c++ ; -*-
/// \file snfee/data/calo_hit_record-serial.h

#ifndef SNFEE_DATA_CALO_HIT_RECORD_SERIAL_H
#define SNFEE_DATA_CALO_HIT_RECORD_SERIAL_H 1

// Ourselves:
#include <snfee/data/calo_hit_record.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

namespace snfee {
  namespace data {

    /// Serialization method
    ///
    /// Based on the conversion of 2 * 12-bit words (stored in 16-bit words,
    /// U==unused bits) in 3 * 8-bit words (3 chars stored in a string).
    ///
    ///  ADC0                 ADC1
    /// [UUUU.6666.5555.4444][UUUU.2222.1111.0000]
    ///
    ///  W0         W1         W2
    /// [6666.5555][4444.2222][1111.0000]
    ///
    template <class Archive>
    void
    calo_hit_record::waveforms_record::serialize(
      Archive& ar_,
      const unsigned int /* version */)
    {
      if (Archive::is_saving::value) {
        uint16_t bufsize = 3 * _samples_.size();
        std::string tmp(bufsize, 0);
        for (std::size_t i = 0; i < _samples_.size(); i++) {
          uint16_t adc0_12 = _samples_[i]._adc_[0] % 4096;
          uint16_t adc1_12 = _samples_[i]._adc_[1] % 4096;
          // Portable formulas (do not use shift operators on 16-bit words but
          // standard arithmetics):
          tmp[3 * i] = adc0_12 / 16;
          tmp[3 * i + 1] = (adc0_12 % 16) * 16 + adc1_12 / 256;
          tmp[3 * i + 2] = adc1_12 % 256;
        }
        ar_& boost::serialization::make_nvp("samples", tmp);
      } else {
        uint16_t bufsize = 1024;
        two_channel_adc_record null_record;
        std::string tmp;
        tmp.reserve(bufsize);
        ar_& boost::serialization::make_nvp("samples", tmp);
        _samples_.assign(tmp.length() / 3, null_record);
        for (std::size_t i = 0; i < _samples_.size(); i++) {
          two_channel_adc_record& rec = _samples_[i];
          uint8_t c0 = tmp[3 * i];
          uint8_t c1 = tmp[3 * i + 1];
          uint8_t c2 = tmp[3 * i + 2];
          rec._adc_[0] = c0 * 16 + c1 / 16;
          rec._adc_[1] = (c1 % 16) * 256 + c2;
        }
      }
      return;
    }

    /// Serialization method
    template <class Archive>
    void
    calo_hit_record::channel_data_record::serialize(
      Archive& ar_,
      const unsigned int /* version */)
    {
      ar_& boost::serialization::make_nvp("lt", _lt_);
      ar_& boost::serialization::make_nvp("ht", _ht_);
      ar_& boost::serialization::make_nvp("underflow", _underflow_);
      ar_& boost::serialization::make_nvp("overflow", _overflow_);
      ar_& boost::serialization::make_nvp("baseline", _baseline_);
      ar_& boost::serialization::make_nvp("peak", _peak_);
      ar_& boost::serialization::make_nvp("peak_cell", _peak_cell_);
      ar_& boost::serialization::make_nvp("charge", _charge_);
      ar_& boost::serialization::make_nvp("rising_cell", _rising_cell_);
      ar_& boost::serialization::make_nvp("falling_cell", _falling_cell_);
      return;
    }

    /// Serialization method
    template <class Archive>
    void
    calo_hit_record::serialize(Archive& ar_, const unsigned int /* version */)
    {
      ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_& boost::serialization::make_nvp("hit_num", _hit_num_);
      ar_& boost::serialization::make_nvp("trigger_id", _trigger_id_);
      ar_& boost::serialization::make_nvp("tdc", _tdc_);
      // ar_ & boost::serialization::make_nvp("module_num",   _module_num_);
      ar_& boost::serialization::make_nvp("crate_num", _crate_num_);
      ar_& boost::serialization::make_nvp("board_num", _board_num_);
      ar_& boost::serialization::make_nvp("chip_num", _chip_num_);
      ar_& boost::serialization::make_nvp("event_id", _event_id_);
      ar_& boost::serialization::make_nvp("l2_id", _l2_id_);
      ar_& boost::serialization::make_nvp("fcr", _fcr_);
      ar_& boost::serialization::make_nvp("has_waveforms", _has_waveforms_);
      if (_has_waveforms_) {
        ar_& boost::serialization::make_nvp("waveform_start_sample",
                                            _waveform_start_sample_);
        ar_& boost::serialization::make_nvp("waveform_number_of_samples",
                                            _waveform_number_of_samples_);
        ar_& boost::serialization::make_nvp("waveforms_record", _waveforms_);
      }
      ar_& boost::serialization::make_nvp("channel_data", _channel_data_);
      return;
    }

  } // end of namespace data
} // end of namespace snfee

#endif // SNFEE_DATA_CALO_HIT_RECORD_SERIAL_H
