//! \file snfee/data/channel_id.h
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_DATA_CHANNEL_ID_H
#define SNFEE_DATA_CHANNEL_ID_H

// Standard Library:
#include <cstdint>
#include <vector>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/serialization_macros.h>
// - SNCabling:
//#include <sncabling/calo_signal_id.h>
//#include <sncabling/label.h>

namespace snfee {
  namespace data {

    //! \brief Readout channel ID from a front-end board
    struct channel_id : public datatools::i_tree_dumpable {

      static const int16_t INVALID_NUMBER = -1;
      static const int16_t DEFAULT_MODULE_NUMBER = 0;

      /// Default constructor
      channel_id();

      /// Constructor
      channel_id(const int16_t crate_num_,
                 const int16_t board_num_,
                 const int16_t channel_num_);

      /// Constructor
      channel_id(const int16_t module_num_,
                 const int16_t crate_num_,
                 const int16_t board_num_,
                 const int16_t channel_num_);

      /// Destructor
      virtual ~channel_id();

      /// Reset/invalidate the channel ID
      void reset();

      /// Set the channel ID numbers
      void set(const int16_t module_num_,
               const int16_t crate_num_,
               const int16_t board_num_,
               const int16_t channel_num_);

      /// Check channel ID completeness
      bool is_complete() const;

      /// Return the module number
      int16_t get_module_number() const;

      /// Return the crate number
      int16_t get_crate_number() const;

      /// Return the board number
      int16_t get_board_number() const;

      /// Return the channel number
      int16_t get_channel_number() const;

      /// Smart print
      virtual void print_tree(
        std::ostream& out_ = std::clog,
        const boost::property_tree::ptree& options_ = empty_options()) const;

      /// Comparison operator
      friend bool operator<(const channel_id& id0_, const channel_id& id1_);

      /// Comparison operator
      friend bool operator>(const channel_id& id0_, const channel_id& id1_);

      /// Comparison operator
      friend bool operator==(const channel_id& id0_, const channel_id& id1_);

      /// Comparison operator
      friend bool operator!=(const channel_id& id0_, const channel_id& id1_);

      /// Export to a string
      std::string to_string(const char symbol_ = 0,
                            bool with_module_ = false) const;

      /// Converty from a string
      bool from_string(const std::string& token_);

      /// Export to the SNCabling calo signal channel identifier
      // bool export_to(sncabling::calo_signal_id & id_) const;

    public:
      int16_t _module_number_ = INVALID_NUMBER;  ///< Module number
      int16_t _crate_number_ = INVALID_NUMBER;   ///< Crate number
      int16_t _board_number_ = INVALID_NUMBER;   ///< Board number
      int16_t _channel_number_ = INVALID_NUMBER; ///< Channel number

      BOOST_SERIALIZATION_BASIC_DECLARATION()
    };

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_CHANNEL_ID_H
