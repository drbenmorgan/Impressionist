//! \file  snfee/utils.h
//! \brief Utilities 
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

#ifndef SNFEE_UTILS_H
#define SNFEE_UTILS_H

// Standard library:
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/properties.h>

namespace snfee {
  
    /// Label representing a missing resource
    const std::string & none_label();
  
    /// Label representing an invalid resource
    const std::string & invalid_label();
  
    /// Explicit singleton for an empty configuration (Bayeux/datatools properties)
    const datatools::properties & empty_config();

} // namespace snfee

#endif //SNFEE_UTILS_H
