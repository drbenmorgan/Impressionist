//! \file  snfee/data/calo_waveform_drawer.h
//! \brief Gnuplot based calorimeter signal waveform drawer 
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

#ifndef SNFEE_DATA_CALO_WAVEFORM_DRAWER_H
#define SNFEE_DATA_CALO_WAVEFORM_DRAWER_H

// Standard library:
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/mygsl/tabulated_function.h>

// This project:
#include <snfee/snfee.h>
#include <snfee/data/channel_id.h>
#include <snfee/data/calo_waveform_data.h>

namespace snfee {
  namespace data {

    /// \brief Gnuplot based calorimeter signal waveform drawer
    class calo_waveform_drawer
    {
    public:

      /// Constructor
      calo_waveform_drawer(const std::string & tmp_dir_ = "/tmp");

      /// Constructor
      calo_waveform_drawer(const double time_min_ns_,
                           const double time_max_ns_,
                           const double amp_min_mV_,
                           const double amp_max_mV_,
                           const bool auto_range_time_ = false,
                           const bool auto_range_voltage_ = false,
                           const std::string & tmp_dir_ = "/tmp");

      void set_range_time(const double time_min_ns_,
                          const double time_max_ns_);

      void set_range_voltage(const double amp_min_mV_,
                             const double amp_max_mV_);

      void set_auto_range_time(const bool);
      
      void set_auto_range_voltage(const bool);
      
      void set_tmp_dir(const std::string & tmp_dir_);

      /// Draw
      void draw(const calo_waveform_info & wfi_,
                const std::string & title_ = "",
                const std::string & id_ = "") const;

      /// Logging priority threshold:
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

    private:

      std::string _tmp_dir_ = "/tmp";
      double _time_min_ns_  = 0.0;    ///< Lower bound on time axis (ns)
      double _time_max_ns_  = 400.0;  ///< Upper bound on time axis (ns)
      double _amp_min_mV_   = -1500.0; ///< Lower bound on signal amplitude axis (mV)
      double _amp_max_mV_   = +1500.0; ///< Upper bound on signal amplitude axis (mV)
      double _time_step_ns_ = 0.390625; ///< Time sampling step (ns)
      double _adc_step_mV_  = 0.610351563; ///< ADC LSB (mV)
      bool   _auto_range_time_    = false;
      bool   _auto_range_voltage_ = false;
      
    };

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_CALO_WAVEFORM_DRAWER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
