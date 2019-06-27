//! \file  snfee/data/rtd_selection.h
//! \brief Selection algorithms for RTD records 
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

#ifndef SNFEE_DATA_RTD_SELECTION_H
#define SNFEE_DATA_RTD_SELECTION_H

// Standard Library:
#include <vector>
#include <iostream>

// This project:
#include <snfee/data/utils.h>
#include <snfee/data/raw_trigger_data.h>

namespace snfee {
  namespace data {

    /// \brief Simple selector about calorimeter raw hit records stored in RTD objects
    class calo_selection
    {
    public:

      struct config_type {
        int16_t crate_num = -1;    ///< Select RTD with at least one calo hit from this crate (-1 : unused)
        int16_t board_num = -1;    ///< Select RTD with at least one calo hit from this board (-1 : unused)
        int16_t chip_num  = -1;    ///< Select RTD with at least one calo hit from this chip (-1 : unused)
        bool    reverse   = false; ///< Reverse the selection
      };
      
      /// Constructor
      calo_selection(const config_type & cfg_);
      
      /// Constructor
      calo_selection(const int16_t crate_num_,
                     const int16_t board_num_ = -1,
                     const int16_t chip_num_ = -1,
                     const bool    reverse_ = false);

      /// Check if the selection is activated
      bool is_activated() const;
      
      /// Selection operator
      bool operator()(const snfee::data::raw_trigger_data & rtd_) const;

    private :

      int16_t _crate_num_ = -1;    ///< Select RTD with at least one calo hit from this crate (-1 : unused)
      int16_t _board_num_ = -1;    ///< Select RTD with at least one calo hit from this board (-1 : unused)
      int16_t _chip_num_  = -1;    ///< Select RTD with at least one calo hit from this chip (-1 : unused)
      bool    _reverse_   = false; ///< Reverse the selection
    
    };
   
  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_RTD_SELECTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
