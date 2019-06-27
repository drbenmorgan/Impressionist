//! \file  snfee/data/rtd2root_data.h
//! \brief Data structure for RTD to Root export

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

#ifndef SNFEE_DATA_RTD2ROOT_DATA_H
#define SNFEE_DATA_RTD2ROOT_DATA_H
 
// Standard Library:
#include <cstdint>

// This project:
#include <snfee/data/utils.h>
#include <snfee/data/raw_trigger_data.h>

namespace snfee {
  namespace data {
 
    struct rtd2root_data
    {
      static const uint16_t MAX_CALO_HITS    = 800;
      static const uint16_t MAX_TRACKER_HITS = 15000;
      static const uint16_t MAX_WAVEFORM_SAMPLES = 1024;
      
      // General:
      int32_t run_id     = INVALID_RUN_ID;
      int32_t trigger_id = INVALID_TRIGGER_ID;

      // Trigger record:
      bool has_trig = false;
      
      // Calo hit records:
      uint32_t nb_calo_hits = 0;
      uint64_t calo_tdc[MAX_CALO_HITS];
      int16_t  calo_crate_num[MAX_CALO_HITS];
      int16_t  calo_board_num[MAX_CALO_HITS];
      int16_t  calo_chip_num[MAX_CALO_HITS];
      uint16_t calo_event_id[MAX_CALO_HITS];
      uint16_t calo_l2_id[MAX_CALO_HITS];
      uint16_t calo_fcr[MAX_CALO_HITS];
      bool     calo_has_waveforms[MAX_CALO_HITS];
      uint16_t calo_waveform_start_sample[MAX_CALO_HITS];
      uint16_t calo_waveform_number_of_samples[MAX_CALO_HITS];

      bool     calo_ch0_lt[MAX_CALO_HITS];      
      bool     calo_ch0_ht[MAX_CALO_HITS];      
      bool     calo_ch0_underflow[MAX_CALO_HITS];      
      bool     calo_ch0_overflow[MAX_CALO_HITS];      
      int16_t  calo_ch0_baseline[MAX_CALO_HITS];      
      int16_t  calo_ch0_peak[MAX_CALO_HITS];      
      int16_t  calo_ch0_peak_cell[MAX_CALO_HITS];      
      int32_t  calo_ch0_charge[MAX_CALO_HITS];      
      int32_t  calo_ch0_rising_cell[MAX_CALO_HITS];      
      int32_t  calo_ch0_falling_cell[MAX_CALO_HITS];      
      int16_t  calo_ch0_waveform[MAX_CALO_HITS][MAX_WAVEFORM_SAMPLES];

      bool     calo_ch1_lt[MAX_CALO_HITS];      
      bool     calo_ch1_ht[MAX_CALO_HITS];      
      bool     calo_ch1_underflow[MAX_CALO_HITS];      
      bool     calo_ch1_overflow[MAX_CALO_HITS];      
      int16_t  calo_ch1_baseline[MAX_CALO_HITS];      
      int16_t  calo_ch1_peak[MAX_CALO_HITS];      
      int16_t  calo_ch1_peak_cell[MAX_CALO_HITS];      
      int32_t  calo_ch1_charge[MAX_CALO_HITS];      
      int32_t  calo_ch1_rising_cell[MAX_CALO_HITS];      
      int32_t  calo_ch1_falling_cell[MAX_CALO_HITS];
      int16_t  calo_ch1_waveform[MAX_CALO_HITS][MAX_WAVEFORM_SAMPLES];
      
      // Tracker hit records:
      uint32_t nb_tracker_hits = 0;
      int16_t  tracker_crate_num[MAX_TRACKER_HITS];
      int16_t  tracker_board_num[MAX_TRACKER_HITS];
      int16_t  tracker_chip_num[MAX_TRACKER_HITS];
      int16_t  tracker_channel_num[MAX_TRACKER_HITS];
      int16_t  tracker_channel_category[MAX_TRACKER_HITS];
      int16_t  tracker_timestamp_category[MAX_TRACKER_HITS];
      uint64_t tracker_timestamp[MAX_TRACKER_HITS];

      void clear();
      
      static void export_to_root(const raw_trigger_data & in_, rtd2root_data & out_);

    };

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_RTD2ROOT_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
