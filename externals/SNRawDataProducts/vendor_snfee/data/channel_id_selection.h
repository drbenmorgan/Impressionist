//! \file  snfee/data/channel_id_selection.h
//! \brief Selection for channel ID 
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

#ifndef SNFEE_DATA_CHANNEL_ID_SELECTION_H
#define SNFEE_DATA_CHANNEL_ID_SELECTION_H

// Standard Library:
#include <iostream>
#include <set>
#include <vector>
#include <cstdint>

// This project:
#include <snfee/data/utils.h>
#include <snfee/data/channel_id.h>

namespace snfee {
  namespace data {

    /// \brief Simple selector about channel ID
    class channel_id_selection
    {
    public:

      struct config_type {
        std::vector<int16_t>    selected_crates;   ///< Select channel IDs with these crates
        std::vector<int16_t>    selected_boards;   ///< Select channel IDs with these boards
        std::vector<int16_t>    selected_channels; ///< Select channel IDs with channels
        std::vector<channel_id> selected_ids;      ///< Select channel IDs with this list
        bool                    reverse   = false; ///< Reverse the selection
      };
     
      /// Default constructor
      channel_id_selection();
     
      /// Constructor
      channel_id_selection(const config_type & cfg_);

      void configure(const config_type & cfg_);
      
      void add_crate(const int16_t);

      void add_board(const int16_t);

      void add_channel(const int16_t);

      void add_id(const channel_id &);

      void set_reverse(const bool = false);
      
      /// Selection operator
      bool operator()(const snfee::data::channel_id & chid_) const;

    private :
      
      std::set<int16_t>    _selected_crates_;   ///< List of selected crates
      std::set<int16_t>    _selected_boards_;   ///< List of selected boards
      std::set<int16_t>    _selected_channels_; ///< List of selected channels
      std::set<channel_id> _selected_ids_;      ///< List of selected ids
      bool                 _reverse_ = false;   ///< Reverse the selection
    
    };

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_CHANNEL_ID_SELECTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
