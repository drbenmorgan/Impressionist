//! \file  snfee/data/has_trigger_id_interface.h
//! \brief Interface with garanteed trigger ID getter 
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

#ifndef SNFEE_DATA_HAS_TRIGGER_ID_INTERFACE_H
#define SNFEE_DATA_HAS_TRIGGER_ID_INTERFACE_H

namespace snfee {
  namespace data {
    
    class has_trigger_id_interface
    {
    public:

      //! Return the trigger ID 
      virtual int32_t get_trigger_id() const = 0;
      
    };

  } // namespace data
} // namespace snfee


#endif // SNFEE_DATA_HAS_TRIGGER_ID_INTERFACE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
