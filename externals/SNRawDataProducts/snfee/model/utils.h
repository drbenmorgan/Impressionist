//! \file  snfee/model/utils.h
//! \brief SNFrontEndElectronics model utilities
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

#ifndef SNFEE_MODEL_UTILS_H
#define SNFEE_MODEL_UTILS_H

// Standard library:
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/properties.h>

namespace snfee {
  namespace model {

    /// \brief Subdetector : calorimeter/tracker
    enum subdetector_type {
      SUBDETECTOR_UNDEF = 0,       ///< Undefined subdetector
      SUBDETECTOR_CALORIMETER = 1, ///< Calorimeter subdetector
      SUBDETECTOR_TRACKER = 2      ///< Tracker subdetector
    };

    /// Return the label associated to a given subdetector
    std::string subdetector_label(const subdetector_type subdetector_);

    /// \brief R/W access trait
    enum access_type {
      ACCESS_UNDEF = 0,                      ///< Undefined access
      ACCESS_READ = 0x1,                     ///< Read only access
      ACCESS_WRITE = 0x2,                    ///< Write only access
      ACCESS_RW = ACCESS_READ | ACCESS_WRITE ///< Read/write only access
    };

    /// Return the label associated to a given access
    std::string access_label(const access_type access_);

    /// Return the access associated to a label
    access_type access_from(const std::string&);

    /// \brief Usage trait
    enum usage_type {
      USAGE_UNDEF = 0,     ///< Undefined usage
      USAGE_EXPERT = 1,    ///< Expert usage (debug/tests)
      USAGE_PRODUCTION = 2 ///< Production usage
    };

    /// Return the label associated to a given usage
    std::string usage_label(const usage_type usage_);

    /// Return the usage associated to a label
    usage_type usage_from(const std::string&);

    /// \brief Action trait
    enum action_type {
      ACTION_UNDEF = 0,         ///< Undefined action
      ACTION_CONFIGURATION = 1, ///< Static configuration action (at run start
                                ///< configuration, writen only once)
      ACTION_DYNCONFIG = 2,     ///< Dynamic configuration action (on-the-fly
                                ///< configuration, can be changed during a run)
      ACTION_MEASUREMENT = 3,   ///< Measurement action (read production data,
                                ///< check/monitor configuration parameters...)
      ACTION_DIAGNOSIS =
        4 ///< Diagnosis action (read internal/expert status...)
    };

    /// Return the label associated to a given action
    std::string action_label(const action_type action_);

    /// Return the action associated to a label
    action_type action_from(const std::string&);

    /// \brief Crate models
    enum crate_model_type {
      CRATE_UNDEF = 0,        //!< Undefined type of crate
      CRATE_CALORIMETER = 1,  //!< Calorimeter crate
      CRATE_TRACKER = 2,      //!< Tracker crate
      CRATE_COMMISSIONING = 3 //!< Hybrid crate for test and commissioning
    };

    /// Return the label associated to a crate type
    std::string crate_model_label(const crate_model_type);

    /// Return the crate type associated to a label
    crate_model_type crate_model_from(const std::string& label_);

    /// \brief Board models
    enum board_model_type {
      BOARD_UNDEF = 0, ///< Undefined category
      BOARD_TB = 1,    ///< Trigger board
      BOARD_CB = 2,    ///< Control board
      BOARD_CFEB = 3,  ///< Calorimeter front-end board
      BOARD_TFEB = 4   ///< Tracker front-end board
    };

    /// Return the label associated to a firmware category
    std::string board_model_label(const board_model_type model_);

    /// Return the firmware category associated to a label
    board_model_type board_model_from(const std::string&);

    /// \brief Supported categories of firmware, depending on the target FPGA
    /// and board.
    enum firmware_category_type {
      FWCAT_UNDEF = 0,     ///< Undefined category
      FWCAT_TB = 1,        ///< Trigger board
      FWCAT_CB_CTRL = 2,   ///< Control FPGA @ Control board
      FWCAT_CB_FE = 3,     ///< Front-end FPGA @ Control board
      FWCAT_CFEB_CTRL = 4, ///< Control FPGA @ Calorimeter front-end board
      FWCAT_CFEB_FE = 5,   ///< Front-end FPGA @ Calorimeter front-end board
      FWCAT_TFEB_CTRL = 6, ///< Control FPGA @ Tracker front-end board
      FWCAT_TFEB_FE = 7    ///< Front-end FPGA @ Tracker front-end board
    };

    /// Return the label associated to a firmware category
    std::string firmware_category_label(const firmware_category_type cat_);

    /// Return the firmware category associated to a label
    firmware_category_type firmware_category_from(const std::string&);

    /// \brief FPGA chip models
    enum fpga_model_type {
      FPGA_UNDEF = 0,    //!< Undefined type of FPGA
      FPGA_CONTROL = 1,  //!< Control FPGA
      FPGA_FRONTEND = 2, //!< Front-end FPGA
      FPGA_GENERIC = 3   //!< Generic FPGA
    };

    /// Return the label associated to a FPGA model
    std::string fpga_model_label(const fpga_model_type model_);

    /// Return the FPGA type associated to a label
    fpga_model_type fpga_model_from(const std::string&);

    /// Attempt to parse an unsigned long from a word
    bool parse(const std::string& word_, unsigned long& value_, int base_ = 0);

    typedef std::vector<std::string> path_type;

    std::ostream& path_to_stream(std::ostream&, const path_type&);

    std::string path_to_string(const path_type&);

    path_type string_to_path(const std::string&);

    void truncate_path(const path_type& path_,
                       path_type& trunc_path_,
                       const std::size_t n_ = 1);

    path_type truncate_path(const path_type& path_, const std::size_t n_ = 1);

  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_UTILS_H
