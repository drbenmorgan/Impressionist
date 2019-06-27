//! \file  snfee/data/tracker_hit_record.h
//! \brief Description of the SuperNEMO tracker frontend board hit data record 
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

#ifndef SNFEE_DATA_TRACKER_HIT_RECORD_H
#define SNFEE_DATA_TRACKER_HIT_RECORD_H

// Standard Library:
#include <string>
#include <limits>
#include <memory>

// Third Party Libraries:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <snfee/data/has_trigger_id_interface.h>
#include <snfee/data/utils.h>

namespace snfee {
  namespace data {

    /// \brief SuperNEMO tracker frontend board raw hit record  
    class tracker_hit_record
      : public datatools::i_tree_dumpable
      , public datatools::i_serializable
      , public has_trigger_id_interface
    {
    public:
   
      /// Default constructor
      tracker_hit_record();
     
      /// Destructor
      virtual ~tracker_hit_record();

      /// Check if the record is complete
      bool is_complete() const;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::data::tracker_hit_record myTrackData
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Tracker data:");
      /// poptions.put("indent", ">>> ");
      /// myTrackData.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

      enum channel_category_type {
        CHANNEL_UNDEF   = 0, ///< Undefined category of channel
        CHANNEL_ANODE   = 1, ///< Anode channel
        CHANNEL_CATHODE = 2  ///< Cathode channel
      };

      static std::string channel_category_label(const channel_category_type);

      enum timestamp_category_type {
        TIMESTAMP_UNDEF      = -1, ///< Undefined
        TIMESTAMP_ANODE_R0   =  0, ///< Anode timestamp with low negative threshold (LNT)
        TIMESTAMP_ANODE_R1   =  1, ///< Anode timestamp with first high negative threshold (HNT)
        TIMESTAMP_ANODE_R2   =  2, ///< Anode timestamp with second high negative threshold (HNT)
        TIMESTAMP_ANODE_R3   =  3, ///< Anode timestamp with first high positive threshold (HPT)
        TIMESTAMP_ANODE_R4   =  4, ///< Anode timestamp with second high positive threshold (HPT)
        TIMESTAMP_CATHODE_R5 =  5, ///< Bottom cathode timestamp
        TIMESTAMP_CATHODE_R6 =  6  ///< Top cathode timestamp
      };

      static std::string timestamp_category_label(const timestamp_category_type);

      static const uint64_t INVALID_TIMESTAMP = std::numeric_limits<uint64_t>::max();
      static const int32_t  INVALID_NUMBER    = -1;
      static const int16_t  INVALID_NUMBER_16 = -1;

      /// Set the hit ID
      void set_hit_num(const int32_t);

      /// Set the trigger ID
      void set_trigger_id(const int32_t);
 
      //! Return the hit number 
      int32_t get_hit_num() const;

      //! Return the trigger ID 
      int32_t get_trigger_id() const override;
    
      // //! Return the module number 
      // int16_t get_module_num() const;
    
      //! Return the crate number 
      int16_t get_crate_num() const;
    
      //! Return the board number 
      int16_t get_board_num() const;

      //! Return the chip number  
      int16_t get_chip_num() const;

      //! Return the channel number 
      int16_t get_channel_num() const;

      //! Return the category of channel  
      channel_category_type get_channel_category() const;

      //! Return the category of timestamp register
      timestamp_category_type get_timestamp_category() const; 

      //! Return the timestamp value         
      uint64_t get_timestamp() const;         

      //! Initialize the record with values
      void make(const int32_t hit_num_,
                const int32_t trigger_id_,
                // const int16_t module_num_,
                const int16_t crate_num_,
                const int16_t board_num_,
                const int16_t chip_num_,
                const int16_t channel_num_,
                const channel_category_type channel_category_,
                const timestamp_category_type timestamp_category_,
                const uint64_t timestamp_);

      //! Reset the record
      void invalidate();
    
    private:

      int32_t _hit_num_     = INVALID_NUMBER;    //!< Hit number
      int32_t _trigger_id_  = INVALID_TRIGGER_ID;    //!< Trigger ID
      // int16_t _module_num_  = INVALID_NUMBER_16; //!< Module number
      int16_t _crate_num_   = INVALID_NUMBER_16; //!< Crate number
      int16_t _board_num_   = INVALID_NUMBER_16; //!< Frontend board slot number
      int16_t _chip_num_    = INVALID_NUMBER_16; //!< FEAST chip number (0, 1)
      int16_t _channel_num_ = INVALID_NUMBER_16; //!< FEAST channel number (0, 53)
      channel_category_type   _channel_category_   = CHANNEL_UNDEF;     //!< Category of channel
      timestamp_category_type _timestamp_category_ = TIMESTAMP_UNDEF;   //!< Category of timestamp register
      uint64_t                _timestamp_          = INVALID_TIMESTAMP; //!< 48 bits timestamp value in unit of 12.5 ns (80 MHz)
        
      DATATOOLS_SERIALIZATION_DECLARATION()

    };

    typedef std::shared_ptr<tracker_hit_record> tracker_hit_record_ptr;
    typedef std::shared_ptr<const tracker_hit_record> const_tracker_hit_record_ptr;

  } // namespace data
} // namespace snfee

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snfee::data::tracker_hit_record, "snfee::data::tracker_hit_record")

#endif // SNFEE_DATA_TRACKER_HIT_RECORD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
