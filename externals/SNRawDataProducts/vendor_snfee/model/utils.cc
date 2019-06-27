//! \file  snfee/model/utils.cc
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

// Ourselves:
#include <snfee/model/utils.h>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>

// This project:
#include <snfee/utils.h>

namespace snfee {
  namespace model {
 
    std::string subdetector_label(const subdetector_type subdetector_)
    {
      switch (subdetector_) {
      case SUBDETECTOR_CALORIMETER:
        return std::string("Calorimeter");
      case SUBDETECTOR_TRACKER:
        return std::string("Tracker");
      default:
        return none_label();
      }
    }
  
    std::string access_label(const access_type access_)
    {
      switch (access_) {
      case ACCESS_READ:  return std::string("read-only");
      case ACCESS_WRITE: return std::string("write-only");
      case ACCESS_RW:    return std::string("read-write");
      default:
        return none_label();
      }
    }
  
    access_type access_from(const std::string & label_)
    {
      if (label_ == access_label(ACCESS_READ)) return ACCESS_READ;
      if (label_ == access_label(ACCESS_WRITE)) return ACCESS_WRITE;
      if (label_ == access_label(ACCESS_RW)) return ACCESS_RW;
      return ACCESS_UNDEF;
    }
  
    std::string usage_label(const usage_type usage_)
    {
      switch (usage_) {
      case USAGE_EXPERT: return std::string("expert");
      case USAGE_PRODUCTION: return std::string("production");
      default:
        return none_label();
      }
    }
  
    usage_type usage_from(const std::string & label_)
    {
      if (label_ == usage_label(USAGE_EXPERT)) return USAGE_EXPERT;
      if (label_ == usage_label(USAGE_PRODUCTION)) return USAGE_PRODUCTION;
      return USAGE_UNDEF;
    }
  
    std::string action_label(const action_type action_)
    {
      switch (action_) {
      case ACTION_CONFIGURATION: return std::string("configuration");
      case ACTION_DYNCONFIG: return std::string("dynconfig");
      case ACTION_MEASUREMENT: return std::string("measurement");
      case ACTION_DIAGNOSIS: return std::string("diagnosis");
      default:
        return none_label();
      }
    }
 
    action_type action_from(const std::string & label_)
    {
      if (label_ == action_label(ACTION_CONFIGURATION)) return ACTION_CONFIGURATION;
      if (label_ == action_label(ACTION_DYNCONFIG)) return ACTION_DYNCONFIG;
      if (label_ == action_label(ACTION_MEASUREMENT)) return ACTION_MEASUREMENT;
      if (label_ == action_label(ACTION_DIAGNOSIS)) return ACTION_DIAGNOSIS;
      return ACTION_UNDEF;
    }

    std::string crate_model_label(const crate_model_type model_)
    {
      switch(model_) {
      case CRATE_CALORIMETER:
        return subdetector_label(SUBDETECTOR_CALORIMETER);
      case CRATE_TRACKER:
        return subdetector_label(SUBDETECTOR_TRACKER);
      case CRATE_COMMISSIONING:
        return std::string("Commissioning");
      default:
        return none_label();
      }
    }

    crate_model_type crate_model_from(const std::string & label_)
    {
      if (label_ == crate_model_label(CRATE_CALORIMETER)) return CRATE_CALORIMETER;
      if (label_ == crate_model_label(CRATE_TRACKER)) return CRATE_TRACKER;
      if (label_ == crate_model_label(CRATE_COMMISSIONING)) return CRATE_COMMISSIONING;
      return CRATE_UNDEF;
    }

    std::string board_model_label(const board_model_type model_)
    {
      switch (model_) {
      case BOARD_TB: return std::string("TB");
      case BOARD_CB: return std::string("CB");
      case BOARD_CFEB: return std::string("CFEB");
      case BOARD_TFEB: return std::string("TFEB");
      default:
        return none_label();
      }
    }

    board_model_type board_model_from(const std::string & label_)
    {
      if (label_ == board_model_label(BOARD_TB)) return BOARD_TB;
      if (label_ == board_model_label(BOARD_CB)) return BOARD_CB;
      if (label_ == board_model_label(BOARD_CFEB)) return BOARD_CFEB;
      if (label_ == board_model_label(BOARD_TFEB)) return BOARD_TFEB;
      return BOARD_UNDEF;
    } 

    std::string firmware_category_label(const firmware_category_type category_)
    {
      switch (category_) {
      case FWCAT_TB        : return std::string("TB");
      case FWCAT_CB_CTRL   : return std::string("CB_CTRL");
      case FWCAT_CB_FE     : return std::string("CB_FE");
      case FWCAT_CFEB_CTRL : return std::string("CFEB_CTRL");
      case FWCAT_CFEB_FE   : return std::string("CFEB_FE");
      case FWCAT_TFEB_CTRL : return std::string("TFEB_CTRL");
      case FWCAT_TFEB_FE   : return std::string("TFEB_FE");
      default:
        return none_label();
      }
    }

    firmware_category_type firmware_category_from(const std::string & label_)
    {
      if (label_ == firmware_category_label(FWCAT_TB)) return FWCAT_TB;
      if (label_ == firmware_category_label(FWCAT_CB_CTRL)) return FWCAT_CB_CTRL;
      if (label_ == firmware_category_label(FWCAT_CB_FE)) return FWCAT_CB_FE;
      if (label_ == firmware_category_label(FWCAT_CFEB_CTRL)) return FWCAT_CFEB_CTRL;
      if (label_ == firmware_category_label(FWCAT_CFEB_FE)) return FWCAT_CFEB_FE;
      if (label_ == firmware_category_label(FWCAT_TFEB_CTRL)) return FWCAT_TFEB_CTRL;
      if (label_ == firmware_category_label(FWCAT_TFEB_FE)) return FWCAT_TFEB_FE;
      return FWCAT_UNDEF;
    }

    std::string fpga_model_label(const fpga_model_type model_)
    {
      switch(model_) {
      case FPGA_CONTROL:
        return "Control";
      case FPGA_FRONTEND:
        return "Frontend";
      case FPGA_GENERIC:
        return "Generic";
      default:
        return "";
      }
    }

    fpga_model_type fpga_model_from(const std::string & label_)
    {
      if (label_ == fpga_model_label(FPGA_CONTROL)) return FPGA_CONTROL;
      if (label_ == fpga_model_label(FPGA_FRONTEND)) return FPGA_FRONTEND;
      if (label_ == fpga_model_label(FPGA_GENERIC)) return FPGA_GENERIC;
      return FPGA_UNDEF;
    }

    bool parse(const std::string & word_, unsigned long & value_, int base_)
    {
      DT_THROW_IF(base_ != 0 && base_ != 2 && base_ != 10  && base_ != 16,
                  std::logic_error,
                  "Base '" << base_ << "' is not supported!");
      bool debug = false;
      // debug = true;
      if (debug) std::cerr << "[debug] input word = '" << word_ << "'" << std::endl;
      std::string token = word_;
      boost::trim(token);
      if (debug) std::cerr << "[debug] token = '" << token << "'" << std::endl;
      if (!token.empty()) {
        if (token[0] == '-') {
          // Reject negative number:
          return false;
        }
      }
      boost::to_upper(token);
      int base = base_;
      if (base == 0) {
        if (boost::starts_with(token, "0X")) {
          base = 16;
        } else if (boost::starts_with(token, "0B")) {
          base = 2;
          token = token.substr(2);
        } else if (boost::starts_with(token, "0")) {
          base = 8;
        } else {
          base = 10;
        }
      }
      try {
        std::size_t sz = 0;
        unsigned long ul = std::stoul(token, &sz, base);
        if (debug) {
          std::cerr << "[debug]   sz = " << sz << std::endl;
          std::cerr << "[debug]   ul = " << ul << std::endl;
          std::cerr << "[debug] remaining = '" << token.substr(sz) << "'" << std::endl;
        }
        if (sz != token.size()) {
          // Reject trailing chars:
          return false;
        }
        value_ = ul;
      } catch (std::exception & x) {
        return false;
      }
      return true;
    }

    std::ostream & path_to_stream(std::ostream & out_, const path_type & p_)
    {
      for (std::size_t i = 0; i < p_.size(); i++) {
        out_ << p_[i];
        if (i + 1 < p_.size()) out_ << '/';
      } 
      return out_;
    }

    std::string path_to_string(const path_type & path_)
    {
      std::ostringstream ss;
      path_to_stream(ss, path_);
      return ss.str();
    }

    path_type string_to_path(const std::string & token_)
    {
      path_type p;
      boost::split(p,token_,boost::is_any_of("/"));
      return p;
    }

    void truncate_path(const path_type & path_,
                       path_type & trunc_path_,
                       const std::size_t n_)
    {
      trunc_path_ = path_;
      for (std::size_t i = 0; i < n_; i++) {
        trunc_path_.pop_back();
      }
      return;
    }

    path_type truncate_path(const path_type & path_, const std::size_t n_)
    {
      path_type tp;
      truncate_path(path_, tp, n_);
      return tp;
    }

  } // namespace model
} // namespace snfee

