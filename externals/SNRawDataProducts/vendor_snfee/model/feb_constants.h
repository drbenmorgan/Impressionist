//! \file  snfee/model/feb_constants.h
//! \brief SNFrontEndElectronics model constants
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

#ifndef SNFEE_MODEL_FEB_CONSTANTS_H
#define SNFEE_MODEL_FEB_CONSTANTS_H

// Standard library:
#include <cstdint>

// Third Party Libraries:
#include <bayeux/datatools/clhep_units.h>

namespace snfee {
  namespace model {

    /// \brief FEB constants
    struct feb_constants
    {
      // Genaral constants:
      static const     uint16_t MAIN_CLOCK_FREQUENCY_MHZ      = 40;     

      // SAMLONG ASIC:
      static const     uint16_t SAMLONG_MAX_NUMBER_OF_SAMPLES = 1024;
      static const     uint16_t SAMLONG_INVALID_SAMPLE        = 1024;
      static const     uint16_t SAMLONG_NUMBER_OF_CHANNELS    = 2;  
      static constexpr uint16_t SAMLONG_DEFAULT_SAMPLING_FREQUENCY_MHZ = 64 * MAIN_CLOCK_FREQUENCY_MHZ;     
      static const     double   SAMLONG_DEFAULT_SAMPLING_FREQUENCY;     
      static constexpr uint16_t SAMLONG_ADC_NBITS             = (1 << 12);
      static constexpr uint16_t SAMLONG_ADC_ZERO              = SAMLONG_ADC_NBITS / 2;
      static const     double   SAMLONG_ADC_MIN_VOLTAGE;
      static const     double   SAMLONG_ADC_MAX_VOLTAGE;
      static const     double   SAMLONG_ADC_MIN_VOLTAGE_MV;
      static const     double   SAMLONG_ADC_MAX_VOLTAGE_MV;
      static const     double   SAMLONG_ADC_VOLTAGE_LSB;
      static const     double   SAMLONG_ADC_VOLTAGE_LSB_MV;
      static const     double   SAMLONG_DEFAULT_TDC_LSB;
      static const     double   SAMLONG_DEFAULT_TDC_LSB_NS;
     
      // FEAST ASIC:
      static const     uint16_t FEAST_CLOCK_FREQUENCY_MHZ         = 2 * MAIN_CLOCK_FREQUENCY_MHZ;     
      static const     uint16_t FEAST_NUMBER_OF_CHANNELS          = 54;  
      static const     uint16_t FEAST_DEFAULT_CLOCK_FREQUENCY_MHZ = 80;     

      // Calorimeter FEB:
      static const     uint16_t CFEB_NUMBER_OF_FE_FPGA     = 4;
      static const     uint16_t CFEB_NUMBER_OF_SAMLONGS_PER_FE_FPGA = 2;
      static constexpr uint16_t CFEB_NUMBER_OF_SAMLONGS    = CFEB_NUMBER_OF_FE_FPGA * CFEB_NUMBER_OF_SAMLONGS_PER_FE_FPGA;
      static constexpr uint16_t CFEB_NUMBER_OF_CHANNELS    = SAMLONG_NUMBER_OF_CHANNELS * CFEB_NUMBER_OF_SAMLONGS;
      static const     uint16_t CFEB_INVALID_BOARD_CHANNEL = 16;
    
      // Calorimeter crates:
      static const     uint16_t MAX_NUMBER_OF_CALO_CRATES     = 3;
      static const     uint16_t MAX_CALO_CRATE_NUMBER_OF_FEBS = 20;
    
      // Tracker FEB:
      static const     uint16_t TFEB_NUMBER_OF_FEASTS   = 2;
      static const     uint16_t TFEB_NUMBER_OF_FE_FPGA  = 2;
      static constexpr uint16_t TFEB_NUMBER_OF_CHANNELS = TFEB_NUMBER_OF_FEASTS * FEAST_NUMBER_OF_CHANNELS;
   
      // Control board:
      static const     uint16_t CB_NUMBER_OF_FE_FPGA    = 4;
    
      // Tracker crates:
      static const     uint16_t MAX_NUMBER_OF_TRACKER_CRATES     = 3;
      static const     uint16_t MAX_TRACKER_CRATE_NUMBER_OF_FEBS = 19;
   
    };
 
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_FEB_CONSTANTS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
