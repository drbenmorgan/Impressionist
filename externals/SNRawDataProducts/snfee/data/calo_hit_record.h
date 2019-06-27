//! \file  snfee/data/calo_hit_record.h
//! \brief Description of the SuperNEMO calorimeter frontend board raw hit data
//! record
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef SNFEE_DATA_CALO_HIT_RECORD_H
#define SNFEE_DATA_CALO_HIT_RECORD_H

// Standard Library:
#include <memory>
#include <vector>

// Third Party Libraries:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/data/has_trigger_id_interface.h>
#include <snfee/data/utils.h>
#include <snfee/model/feb_constants.h>

namespace snfee {
  namespace data {

    /// \brief SuperNEMO calorimeter frontend board raw hit record
    ///
    /// A calorimeter hit record stores the readout data extracted from a
    /// single SAMLONG chipset, including its two channels.
    ///
    class calo_hit_record : public datatools::i_tree_dumpable,
                            public datatools::i_serializable,
                            public has_trigger_id_interface {
    public:
      // Constants:
      static const int32_t INVALID_NUMBER = -1;
      static const int16_t INVALID_NUMBER_16 = -1;
      static const uint16_t INVALID_NUMBER_16U = 0xFFFF;
      static const uint16_t INVALID_WAVEFORM_START_SAMPLE =
        snfee::model::feb_constants::SAMLONG_INVALID_SAMPLE;
      static const uint16_t INVALID_WAVEFORM_NUMBER_OF_SAMPLES = 0;
      static const uint16_t SAMPLE_ADC_MAX = 4095;
      static const uint16_t SAMPLE_ADC_DEFAULT = SAMPLE_ADC_MAX;
      static const uint64_t TDC_INVALID = std::numeric_limits<uint64_t>::max();
      static const uint64_t TDC_MAX = 0xFFFFFFFFFF;

      /// Default constructor
      calo_hit_record();

      /// Destructor
      virtual ~calo_hit_record();

      /// Check if the record is complete
      bool is_complete() const;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::data::calo_hit_record myCaloHitRec
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Calorimeter hit record:");
      /// poptions.put("indent", ">>> ");
      /// myCaloHitRec.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream& out_ = std::clog,
                              const boost::property_tree::ptree& options_ =
                                empty_options()) const override;

      /// \bried Two channel ADC sample record for the SAMLONG ASIC
      struct two_channel_adc_record {
      public:
        /// Constructor
        two_channel_adc_record();

        /// Set the ADC value for a given channel
        void set_adc(const uint16_t channel_, const uint16_t adc_);

        /// Return the ADC value for a given channel
        uint16_t get_adc(const uint16_t channel_) const;

      private:
        /// ADC values associated to both channels
        ///
        /// ADC values for each 2 channels are stored  on 12 bits (here we use
        /// 16-bit unsigned integers to store them). The layout of the ADC
        /// record is: \code
        ///     Chanel #0         Chanel #1
        /// 15..11..........0 15..11..........0
        /// [UUUUAAAAAAAAAAAA][UUUUAAAAAAAAAAAA]
        /// \endcode
        /// where :
        /// - U means unused bit
        /// - A means ADC data bits
        ///
        uint16_t _adc_[snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS];

        friend class calo_hit_record;
      };

      /// \brief Waveforms record for the SAMLONG ASIC (2-channels waveforms
      /// data)
      ///
      /// Up to 1024 records of type \t two_channel_adc_record_type
      struct waveforms_record {
      public:
        /// Constructor
        waveforms_record(
          const uint16_t nb_samples_ =
            snfee::model::feb_constants::SAMLONG_MAX_NUMBER_OF_SAMPLES);

        /// Return the vector of ADC samples
        const std::vector<two_channel_adc_record>& get_samples() const;

        /// Set a ADC value at a given sample and channel
        void set_adc(const uint16_t sample_index_,
                     const uint16_t channel_,
                     const uint16_t adc_);

        /// Return the ADC value at a given sample and channel
        uint16_t get_adc(const uint16_t sample_index_,
                         const uint16_t channel_) const;

        /// Reset the vector of ADC samples
        void reset(const uint16_t nb_samples_);

        /// Invalidate the record
        void invalidate();

      private:
        /// Vector of two-channel ADC samples
        std::vector<two_channel_adc_record> _samples_;

        BOOST_SERIALIZATION_BASIC_DECLARATION()
      };

      /// \brief SAMLONG channel data record (post-data)
      struct channel_data_record {
        /// Check if low-threshold (LT) bit is activated
        bool is_lt() const;

        /// Check if the hit is LTO (low-threshold only)
        bool is_lto() const;

        /// Check if high-threshold (HT) bit is activated
        bool is_ht() const;

        /// Check if underflow bit is activated
        bool is_underflow() const;

        /// Check if overflow bit is activated
        bool is_overflow() const;

        /// Return the baseline computed by the firmware
        int16_t get_baseline() const;

        /// Return the peak amplitude computed by the firmware
        int16_t get_peak() const;

        /// Return the peak position computed by the firmware
        int16_t get_peak_cell() const;

        /// Return the charge computed by the firmware
        int32_t get_charge() const;

        /// Return the position of rising edge threshold crossing
        int32_t get_rising_cell() const;

        /// Return the position of falling edge threshold crossing
        int32_t get_falling_cell() const;

        /// Initialize the record
        void make(const bool lt_,
                  const bool ht_,
                  const bool underflow_,
                  const bool overflow_,
                  const int16_t baseline_,
                  const int16_t peak_,
                  const int16_t peak_cell_,
                  const int32_t charge_,
                  const int32_t rising_cell_,
                  const int32_t falling_cell_);

        /// Reset the record
        void invalidate();

        /// Smart print
        virtual void print_tree(
          std::ostream& out_ = std::clog,
          const boost::property_tree::ptree& options_ = empty_options()) const;

      private:
        bool _lt_ = false;        //!< Channel passed LT (1 bit)
        bool _ht_ = false;        //!< Channel passed HT (1 bit)
        bool _underflow_ = false; //!< Underflow flag (1 bit) [Not used]
        bool _overflow_ =
          false; //!< Charge time overflow flag (1 bit) [To be confirmed]
        int16_t _baseline_ = 0; //!< Baseline (16 bits, in unit of ADC LSB / 16,
                                //!< computed in FEB FPGA)
        int16_t _peak_ = 0; //!< Peak maximum amplitude (16 bits, in unit of ADC
                            //!< LSB / 8, computed in FEB FPGA)
        int16_t _peak_cell_ = 0; //!< Peak time at maximum amplitude (10 bits,
                                 //!< in unit of TDC LSB, computed in FEB FPGA)
        int32_t _charge_ =
          0; //!< Computed charge (23 bits, computed in FEB FPGA)
        int32_t _rising_cell_ =
          0; //!< Positive edge cell index (19 from 24 bits, in unit of TDC LSB
             //!< / 256, computed in FEB FPGA)
        int32_t _falling_cell_ =
          0; //!< Negative edge cell index (19 from 24 bits, in unit of TDC LSB
             //!< / 256, computed in FEB FPGA)

        BOOST_SERIALIZATION_BASIC_DECLARATION()

        friend class calo_hit_record;
      };

      /// Set the hit ID
      void set_hit_num(const int32_t);

      /// Set the trigger ID
      void set_trigger_id(const int32_t);

      //! Return the TDC
      uint64_t get_tdc() const;

      //! Return the hit number
      int32_t get_hit_num() const;

      //! Return the trigger ID
      int32_t get_trigger_id() const override;

      //! Return the crate number
      int16_t get_crate_num() const;

      //! Return the board number
      int16_t get_board_num() const;

      //! Return the chip number
      int16_t get_chip_num() const;

      //! Return the local event ID
      uint16_t get_event_id() const;

      //! Return the L2 ID
      uint16_t get_l2_id() const;

      //! Return the FCR
      uint16_t get_fcr() const;

      //! Check if record has registered waveform
      bool has_waveforms() const;

      //! Return the waveform start sample
      uint16_t get_waveform_start_sample() const;

      //! Return the waveform number of samples
      uint16_t get_waveform_number_of_samples() const;

      //! Return the waveforms record
      const waveforms_record& get_waveforms() const;

      //! Return the channel record per channel
      const channel_data_record& get_channel_data(
        const uint16_t channel_num_) const;

      //! Return the mutable channel record per channel
      channel_data_record& grab_channel_data(const uint16_t channel_num_);

      //! Initialize the record with values
      void make(const int32_t hit_num_,
                const int32_t trigger_id_,
                const uint64_t tdc_,
                const int16_t crate_num_,
                const int16_t board_num_,
                const int16_t chip_num_,
                const uint16_t event_id_,
                const uint16_t l2_id_,
                const uint16_t fcr_,
                const bool has_waveforms_,
                const uint16_t waveform_start_sample_,
                const uint16_t waveform_number_of_samples_,
                const bool preserve_waveforms_ = false);

      //! Initialize a channel data record with values
      void make_channel(const int channel_num_,
                        const bool lt_,
                        const bool ht_,
                        const bool underflow_,
                        const bool overflow_,
                        const int16_t baseline_,
                        const int16_t peak_,
                        const int16_t peak_cell_,
                        const int32_t charge_,
                        const int32_t rising_cell_,
                        const int32_t falling_cell_);

      //! Set the waveform ADC value at a given channel and sample
      void set_waveform_adc(const uint16_t channel_index_,
                            const uint16_t sample_index_,
                            const uint16_t adc_);

      //! Reset the record
      void invalidate();

      //! Populate a mock calorimeter hit record (for debug and test)
      static void populate_mock_hit(
        calo_hit_record& hit_,
        const bool signal_on_first_channel,
        const bool signal_on_second_channel,
        const int32_t hit_num_,
        const int32_t trigger_id_,
        const uint64_t tdc_,
        const int16_t crate_num_,
        const int16_t board_num_,
        const int16_t chip_num_,
        const uint16_t event_id_,
        const uint16_t l2_id_,
        const uint16_t fcr_,
        const bool has_waveforms_ = true,
        const uint16_t waveform_start_sample_ = 0,
        const uint16_t waveform_number_of_samples_ = 1024);

      //! Provide a mock sampled signal (for debug and test)
      static const std::vector<int16_t>& mock_adc_samples();

    private:
      // Pre-data:
      int32_t _hit_num_ = INVALID_NUMBER;        //!< Hit number
      int32_t _trigger_id_ = INVALID_TRIGGER_ID; //!< Trigger ID (24 bits)
      uint64_t _tdc_ = TDC_INVALID;              //!< TDC (40 bits)
      int16_t _crate_num_ =
        INVALID_NUMBER_16; //!< Frontend crate number (3 bits)
      int16_t _board_num_ =
        INVALID_NUMBER_16; //!< Frontend board slot number (5 bits)
      int16_t _chip_num_ =
        INVALID_NUMBER_16; //!< SAMLONG chip number (3-4 bits?)
      uint16_t _event_id_ =
        0; //!< Local event ID (8 bits, should be the trigger ID mod 256)
      uint16_t _l2_id_ = INVALID_NUMBER_16U; //!< L2 signal ID (5 bits, should
                                             //!< be the trigger ID mod 32)
      uint16_t _fcr_ = snfee::model::feb_constants::
        SAMLONG_INVALID_SAMPLE; //!< First cell read (10-11 bits ?)

      // Waveform data:
      bool _has_waveforms_ =
        false; //!< Flag indicating if waveforms are recorded
      uint16_t _waveform_start_sample_ =
        INVALID_WAVEFORM_START_SAMPLE; //!< Waveform start sample index
                                       //!< (default: 0)
      uint16_t _waveform_number_of_samples_ =
        INVALID_WAVEFORM_NUMBER_OF_SAMPLES; //!< Waveform number of samples
                                            //!< (default at construct: -1,
                                            //!< typical: 1024)
      waveforms_record
        _waveforms_; // 2-channel waveforms (2 x 12 bits ADC samples)

      // Post-data:
      channel_data_record
        _channel_data_[snfee::model::feb_constants::
                         SAMLONG_NUMBER_OF_CHANNELS]; //!< Channel post-data
                                                      //!< records (features:
                                                      //!< charge, peak...)

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

    typedef std::shared_ptr<calo_hit_record> calo_hit_record_ptr;
    typedef std::shared_ptr<const calo_hit_record> const_calo_hit_record_ptr;

  } // namespace data
} // namespace snfee

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snfee::data::calo_hit_record,
                        "snfee::data::calo_hit_record")

#endif // SNFEE_DATA_CALO_HIT_RECORD_H
