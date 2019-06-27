//! \file  snfee/geometry.h
//! \brief SNFrontEndElectronics geometry constants
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

#ifndef SNFEE_GEOMETRY_H
#define SNFEE_GEOMETRY_H

namespace snfee {

  struct geometry {
    // static const uint16_t NSIDES          = 2;
    // static const uint16_t TRACKER_NROWS   = 113;
    // static const uint16_t TRACKER_NLAYERS = 9;

    static const uint16_t SN_MODULE_NB_OF_SIDES = 2;

    static const uint16_t SN_MODULE_NB_OF_COLUMNS_IN_CALO_MAIN_WALL = 20;
    static const uint16_t SN_MODULE_NB_OF_ROWS_IN_CALO_MAIN_WALL = 13;

    static const uint16_t SN_MODULE_NB_COLUMNS_IN_CALO_X_WALL = 2;
    static const uint16_t SN_MODULE_NB_OF_ROWS_IN_CALO_X_WALL = 16;
    static const uint16_t SN_MODULE_NB_OF_WALLS_IN_CALO_X_WALL = 2;

    static const uint16_t SN_MODULE_NB_OF_WALLS_IN_CALO_GAMMA_VETO = 2;
    static const uint16_t SN_MODULE_NB_OF_COLUMNS_IN_CALO_GAMMA_VETO = 16;

    static const uint16_t SN_MODULE_NB_OF_LAYERS_IN_TRACKER_FRAME = 9;
    static const uint16_t SN_MODULE_NB_OF_ROW_IN_TRACKER_FRAME = 113;

    enum calo_type {
      CALO_MAIN_WALL_SLOT = 0,
      CALO_X_WALL_SLOT = 1,
      CALO_GAMMA_VETO_SLOT = 2
    };
  };

} // namespace snfee

#endif // SNFEE_GEOMETRY_H
