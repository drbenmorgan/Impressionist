//! \file  snfee/data/calo_channel_traits.h
//! \brief Calorimeter channel traits
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

#ifndef SNFEE_DATA_CALO_CHANNEL_TRAITS_H
#define SNFEE_DATA_CALO_CHANNEL_TRAITS_H

// Standard library:
#include <string>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <map>
#include <limits>

// Third party:
// - Boost:
// #include <boost/serialization/access.hpp>
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/mygsl/tabulated_function.h>
#include <bayeux/datatools/serialization_macros.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/data/channel_id.h>
#include <snfee/data/calo_waveform_data.h>

namespace snfee {
  namespace data {

    ///! Charge interval:
    ///
    /// A charge range is defined via negative signal charge:
    ///   0 <= lower charge < Q < upper charge (charge unit: nV.s)
    /// Example: [-2.0 nV.s: -3.0 nV.s]
    class charge_range
    {
    public:

      /// Default constructor
      charge_range();

      /// Constructor
      ///
      /// \code
      /// snfee::data::charge_range qGpr0( 0.0, -2.0); // [ 0.0 nV.s: -2.0 nV.s]
      /// snfee::data::charge_range qGpr1(-2.0, -4.0); // [-2.0 nV.s: -4.0 nV.s]
      /// snfee::data::charge_range qGpr2(-4.0, -6.0); // [-4.0 nV.s: -6.0 nV.s]
      /// snfee::data::charge_range qGpr2(-6.0);       // [-4.0 nV.s: -infinity [
      /// \encode
      charge_range(const double lower_charge_nVs_, const double upper_charge_nVs_ = -std::numeric_limits<double>::infinity());

      /// Set the lower and upper charges (unit: nV.s) of the range
      void set(const double lower_charge_nVs_, const double upper_charge_nVs_ = -std::numeric_limits<double>::infinity());

      /// Return the lower charge (unit: nV.s) of the range
      double get_lower_charge_nVs() const;

      /// Return the upper charge (unit: nV.s) of the range
      double get_upper_charge_nVs() const;

      /// Check if a charge lies in the range
      bool inside(const double charge_nVs_) const;

    private:

      double _lower_charge_nVs_; ///< Lower bound of the charge range (unit: nV.s)
      double _upper_charge_nVs_; ///< Upper bound of the charge range (unit: nV.s)

      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };

    /// \brief Calorimeter channel traits
    class calo_channel_traits
      : public datatools::i_tree_dumpable
    {
    public:

      /// \brief Flags
      enum flag_type {
        flag_dead_channel = 0x1, ///< Dead channel flag
        flag_pmt_5inch    = 0x2, ///< 5-inch PMT
        flag_pmt_8inch    = 0x4  ///< 8-inch PMT
      };

      /// \brief Information associated to a range of charge
      struct charge_range_info
        : public datatools::i_tree_dumpable
      {
        /// Constructor
        charge_range_info();

        /// Check validity
        bool is_valid() const;

        /// Invalidation
        void invalidate();

        // Check if the Q/A factor is set
        bool has_q2a_factor() const;

        /// Smart print
        ///
        /// Usage:
        /// \code
        /// snfee::data::calo_channel_traits::charge_range_info qRangeInfo
        /// ...
        /// boost::property_tree::ptree poptions;
        /// poptions.put("title", "Charge range info:");
        /// poptions.put("indent", ">>> ");
        /// qRangeInfo.print_tree(std::clog, poptions);
        /// \endcode
        virtual void print_tree(std::ostream & out_ = std::clog,
                                const boost::property_tree::ptree & options_ = empty_options()) const;

        int    range_id = -1;  ///< Charge range unique ID
        double ref_time_ns;    ///< Reference time of the mean signal (unit: ns)
        mygsl::tabulated_function mean_signal; ///< Mean signal (time unit: ns, signal amplitude unit: mV)
        double q2a_factor;     ///< Charge to effective amplitude conversion factor: A = f * Q  (unit: mV / nV / s)

        BOOST_SERIALIZATION_BASIC_DECLARATION()

      };

      /// \brief High voltage regime information
      class hv_regime_info
        : public datatools::i_tree_dumpable
      {
      public:

        /// Constructor
        hv_regime_info();

        /// Check validity
        bool is_valid() const;

        /// Invalidate attributes
        void invalidate();

        /// Set the nominal HV (V)
        void set_nominal_hv_V(const double);

        /// Return the nominal HV (V)
        double get_nominal_hv_V() const;

        /// Set the expected baseline data
        void set_baseline(const calo_waveform_baseline & baseline_);

        /// Return the expected baseline data
        const calo_waveform_baseline & get_baseline() const;

        /// Return the index of charge range associated to a charge
        int get_charge_range_index(const double q_nVs_) const;

        /// Check if a charge (nV.s) is associated to a range of charge
        bool has_charge_range(const double q_nVs_) const;

        /// Return the charge range associated to a charge
        const charge_range & get_charge_range(const double q_nVs_) const;

        /// Add a range of charge
        ///
        /// \return charge range index
        int add_charge_range(const charge_range &);

        /// Return the vector of range of charges
        const std::vector<charge_range> & get_charge_ranges() const;

        /// Clear the vector of charge ranges
        void clear_charge_ranges();

        /// \brief Dictionary of informations classified by charge ranges
        typedef std::map<int, charge_range_info> charge_range_info_dict_type;

        /// Add a charge range info
        void add_charge_range_info(const charge_range_info &);

        /// Clear the informations classified by charge ranges
        void clear_charge_range_infos();

        /// Return the dictionary of informations on mean waveform for different charge ranges
        const charge_range_info_dict_type & get_charge_range_infos() const;

        /// Check if a charge range info associated to a given charge range exists
        bool has_charge_range_info(const int range_id_) const;

        /// Return the charge range info associated to a given charge range
        const charge_range_info & get_charge_range_info(const int range_id_) const;

        /// Smart print
        ///
        /// Usage:
        /// \code
        /// snfee::data::calo_channel_traits myCaloChTraits
        /// ...
        /// boost::property_tree::ptree poptions;
        /// poptions.put("title", "Calorimeter hit record:");
        /// poptions.put("indent", ">>> ");
        /// myCaloChTraits.print_tree(std::clog, poptions);
        /// \endcode
        virtual void print_tree(std::ostream & out_ = std::clog,
                                const boost::property_tree::ptree & options_ = empty_options()) const;

        /// Display
        static void display(const hv_regime_info & hvrinfo_,
                            const int range_id_,
                            const std::string & id_ = "",
                            const std::string & title_ = "",
                            const uint32_t flags_ = 0);

      private:

        double                      _nominal_hv_V_;       ///< Nominal HV (unit: V)
        calo_waveform_baseline      _baseline_;           ///< Baseline data
        std::vector<charge_range>   _charge_ranges_;      ///< Definition of ranges of charge
        charge_range_info_dict_type _charge_range_infos_; ///< Informations on mean waveform for different charge ranges

        BOOST_SERIALIZATION_BASIC_DECLARATION()

      };

      /// Constructor
      calo_channel_traits();

      /// Set the channel ID
      void set_ch_id(const channel_id &);

      /// Return the channel ID
      const channel_id & get_ch_id() const;

      /// Check some flag(s)
      bool has_flags(const uint32_t flag_) const;

      /// Return the flags
      uint32_t get_flags() const;

      /// Add flags
      void add_flags(const uint32_t flag_);

      /// Check if the channel is dead
      bool is_dead() const;

      /// Check if the PMT associated to the channel is a 5''
      bool is_5inch() const;

      /// Check if the PMT associated to the channel is a 8''
      bool is_8inch() const;

      /// \brief Dictionary of characteristics associated to HV
      ///
      /// The main key is the HV value expressed in mV and converted
      /// an integer. Example:  HV=1354.5 V --> key=1354500
      typedef std::map<int, hv_regime_info> hv_regime_info_dict_type;

      /// Clear vector of HV regimes associated to various HVs
      void clear_hv_regime_infos();

      /// Return HV regime informations associated to various HVs
      const hv_regime_info_dict_type & get_hv_regime_infos() const;

      /// Add some HV regime informations associated to a HVs
      void add_hv_regime_info(const hv_regime_info &);

      /// Check if HV regime informations associated to a given HVs is set
      bool has_hv_regime_info(const double hv_V_) const;

      /// Return HV regime informations associated to a given HVs
      const hv_regime_info & get_hv_regime_info(const double hv_V_) const;

      /// Check if the PMT ID is set
      bool has_pmt_id() const;

      /// Set the PMT ID
      void set_pmt_id(const std::string & pmt_id_);

      /// Return the PMT ID
      const std::string & get_pmt_id() const;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::data::calo_channel_traits myCaloChTraits
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Calorimeter hit record:");
      /// poptions.put("indent", ">>> ");
      /// myCaloChTraits.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:

      channel_id _ch_id_; ///< Readout channel ID
      uint32_t   _flags_; ///< Flags
      hv_regime_info_dict_type _hv_regime_infos_; ///< Dictionary of HV regime informations object
      std::string _pmt_id_; ///< PMT ID

      BOOST_SERIALIZATION_BASIC_DECLARATION()

    };

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_CALO_CHANNEL_TRAITS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
