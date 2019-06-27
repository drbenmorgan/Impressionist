//! \file  snfee/data/calo_waveform_data.h
//! \brief Data models associated to calorimeter signal waveforms 
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

#ifndef SNFEE_DATA_CALO_WAVEFORM_DATA_H
#define SNFEE_DATA_CALO_WAVEFORM_DATA_H

// Standard library:
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>

// Third party:
// - Bayeux:
#include <bayeux/mygsl/tabulated_function.h>
#include <bayeux/datatools/serialization_macros.h>

namespace snfee {
  namespace data {
    
    /// \brief Waveform expressed in physical units (sample time: ns, signal amplitude: mV)
    class calo_waveform
    {
    public:
      
      /// Constructor
      calo_waveform();

      /// Load from a file
      void load(const std::string & filename_);
      
      /// Add a sample (ns, mV)
      void add_sample(const double time_ns_, const double amp_mV_);
      
      /// Reserve number of samples
      void reserve(const uint16_t);
      
      /// Check the lock flag
      bool is_locked() const;

      /// Return the number of samples
      std::size_t size() const;

      /// Lock the waveform shape
      void lock();

      /// Reset the waveform 
      void reset();

      /// Set the tabulated waveform 
      void set_tabfunc(const mygsl::tabulated_function &);

      /// Return the non mutable waveform
      const mygsl::tabulated_function & get_tabfunc() const;

      /// Return the non mutable table of time
      const std::vector<double> & get_times_ns() const;
 
      /// Return the non mutable table of voltage
      const std::vector<double> & get_amplitudes_mV() const;

      /// Return the time (in ns) associated to a given cell
      double get_time_ns(const int cell_) const;

      /// Return the signal amplitude (in mV) associated to a given cell
      double get_amplitude_mV(const int cell_) const;

      /// Return the time sampling step
      double get_time_step() const;
      
    private:

      /// Actions performed at waveform lock
      void _at_lock_();
      
    private:
      
      mygsl::tabulated_function _tabfunc_;       ///< Embedded tabulated function
      std::vector<double>       _times_ns_;      ///< Embedded tabulated times per cell (unit: ns)
      std::vector<double>       _amplitudes_mV_; ///< Embedded tabulated amplitudes per cell (unit: mV)
              
      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };

    /// \brief Waveform baseline
    struct calo_waveform_baseline
    {
      /// Default constructor
      calo_waveform_baseline();

      /// Check the validity of the baseline informations
      bool is_valid() const;

      /// Invalidate the baseline informations
      void invalidate();

      /// Smart print
      void print(std::ostream & out_, const std::string & title_ = "", const std::string & indent_ = "") const;

      enum quality_flags_type {
        quality_poor = 0x1 ///< Poor estimation flag
      };

      uint32_t quality_flags = 0;    ///< Special quality flags 
      double   baseline_start_ns;    ///< Baseline start (unit: ns)
      double   baseline_duration_ns; ///< Duration of the time window for baseline computing (unit: ns)
      int16_t  nb_samples = 0;       ///< Number of samples used to compute the baseline
      double   baseline_mV;          ///< Baseline averaged value (unit: mV)
      double   sigma_baseline_mV;    ///< Standard deviation on baseline value (unit: mV)
      double   noise_mV;             ///< Amplitude of the signal noise (sigma) along the baseline (unit: mV)
        
      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };
 
    /// \brief Waveform peak information
    struct calo_waveform_peak
    {

      enum quality_flags_type {
        quality_positive       = 0x1, ///< Positive flag
        quality_overflow       = 0x2, ///< Overflow peak flag
        quality_head_truncated = 0x4, ///< Time truncated peak head flag
        quality_tail_truncated = 0x8  ///< Time truncated peak tail flag
      };

      uint32_t quality_flags = 0;             ///< Special quality flags 
      int16_t time_cell = -1;                 ///< Peak reference cell
      int16_t time_head_10_percent_cell = -1; ///< Reference cell of the head edge crossing 10 % amplitude
      int16_t time_head_90_percent_cell = -1; ///< Reference cell of the head edge crossing 10 % amplitude
      int16_t time_tail_90_percent_cell = -1; ///< Reference cell of the tail edge crossing 90 % amplitude
      int16_t time_tail_10_percent_cell = -1; ///< Reference cell of the tail edge crossing 90 % amplitude
      double  time_ns;                        ///< Peak position (unit: ns)
      double  sigma_time_ns;                  ///< Standard error on the peak position (unit: ns)
      double  amplitude_mV;                   ///< Peak max amplitude (unit: mV)
      double  sigma_amplitude_mV;             ///< Standard error on the peak max amplitude (unit: mV)
      double  time_head_10_percent_ns;        ///< Time of the head edge crossing 10 % amplitude (unit: ns)
      double  time_head_90_percent_ns;        ///< Time of the head edge crossing 90 % amplitude (unit: ns)
      double  time_tail_90_percent_ns;        ///< Time of the tail edge crossing 90 % amplitude (unit: ns)
      double  time_tail_10_percent_ns;        ///< Time of the tail edge crossing 10 % amplitude (unit: ns)

      /// Default constructor
      calo_waveform_peak();

      /// Check the validity of the peak informations
      bool is_valid() const;

      /// Invalidate the baseline informations
      void invalidate();

      /// Return the waveform head/rising time (10%->90% amplitude, in ns)
      double get_rising_time() const;

      /// Return the waveform tail/falling time (90%->10% amplitude, in ns)
      double get_falling_time() const;

      /// Smart print
      void print(std::ostream & out_, const std::string & title_ = "", const std::string & indent_ = "") const;
              
      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };

    /// \brief Waveform CFD reference time
    struct calo_waveform_cfd
    {
      
      /// Default constructor
      calo_waveform_cfd();

      /// Check the validity of the baseline informations
      bool is_valid() const;

      /// Invalidate the baseline informations
      void invalidate();

      /// Smart print
      void print(std::ostream & out_, const std::string & title_ = "", const std::string & indent_ = "") const;

      double   fraction;      ///< CFD fraction (dimensionless)
      int16_t  ref_cell;      ///< CFD reference cell index (unit: TDC LSB)
      double   time_ns;       ///< CFD time (unit: ns) (linear interpolation)
      double   sigma_time_ns; ///< Standard deviation on CFD time (unit: ns)
      double   amplitude_mV;  ///< Signal amplitude at CFD fraction (unit: mV)
      double   sigma_amplitude_mV;  ///< Standard error on signal amplitude at CFD fraction (unit: mV)
       
      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };

    /// \brief Waveform charge (integrated with respect to a peak candidate)
    struct calo_waveform_charge
    {
      
      /// Default constructor
      calo_waveform_charge();

      /// Check the validity of the baseline informations
      bool is_valid() const;

      /// Invalidate the baseline informations
      void invalidate();

      /// Smart print
      void print(std::ostream & out_, const std::string & title_ = "", const std::string & indent_ = "") const;

      enum quality_flags_type {
        quality_baseline_overlap = 0x1,
        quality_head_truncated   = 0x2,
        quality_tail_truncated   = 0x4
      };
      
      uint32_t quality_flags = 0; ///< Special flags 
      double start_time_ns;       ///< Start time of the charge integration window (unit: ns)
      double stop_time_ns;        ///< Stop time of the charge integration window (unit: ns)
      double charge_nVs;          ///< Integrated charge (unit: nV.s)
      double sigma_charge_nVs;    ///< Standard error on the integrated charge (unit: nV.s)
              
      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };
 
    /// \brief Information extracted from the firmware (raw data : channel_data_record)
    struct calo_waveform_firmware_data
    {
     
      /// Default constructor
      calo_waveform_firmware_data();

      /// Check the validity of the baseline informations
      bool is_valid() const;

      /// Invalidate the baseline informations
      void invalidate();

      /// Smart print
      void print(std::ostream & out_, const std::string & title_ = "", const std::string & indent_ = "") const;
      
      double baseline_mV;  ///< Baseline computed by the calo FEB frontend firmware (unit: mV)
      double peak_mV;      ///< Peak computed by the calo FEB frontend firmware (unit: mV)
      double peak_time_ns; ///< Peak time computed by the calo FEB frontend firmware (unit: ns)
      double charge_nVs;   ///< Charge computed by the calo FEB frontend firmware (unit: nV.s)
      double cfd_time_ns;  ///< CFD time computed by the calo FEB frontend firmware  (unit: ns)
              
      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };
 
    /// \brief Waveform info
    struct calo_waveform_info
    {
      snfee::data::calo_waveform          waveform; ///< Waveform (TDC unit: ns, ADC: unit mV)
      bool                                has_fw_data = false;
      calo_waveform_firmware_data         fw_data;
      bool                                has_baseline = false;
      snfee::data::calo_waveform_baseline baseline;
      bool                                has_peak = false;
      snfee::data::calo_waveform_peak     peak;
      bool                                has_cfd = false;
      snfee::data::calo_waveform_cfd      cfd;
      bool                                has_charge = false;
      snfee::data::calo_waveform_charge   charge;

      /// Reset informations
      void reset(const bool preserve_raw_ = false);
      
      /// Smart print
      void print(std::ostream & out_, const std::string & title_ = "", const std::string & indent_ = "") const;
              
      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };
    
  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_CALO_WAVEFORM_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
