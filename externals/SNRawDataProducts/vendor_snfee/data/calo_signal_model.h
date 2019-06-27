//! \file  snfee/data/calo_signal_model.h
//! \brief Calorimeter signal model 
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

#ifndef SNFEE_DATA_CALO_SIGNAL_MODEL_H
#define SNFEE_DATA_CALO_SIGNAL_MODEL_H

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
#include <bayeux/datatools/serialization_macros.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/mygsl/i_unary_function.h>
#include <bayeux/mygsl/i_unary_function_with_derivative.h>

// This project:
 
namespace snfee {
  namespace data {

    /// \brief Simplified calorimeter signal model (time unit: ns, voltage unit: mV, polarity; negative)
    class calo_signal_model
      : public mygsl::i_unary_function
    {
    public:

      /// \brief Cnofiguration parameters
      struct config_type
      {
        double t0;                ///< Signal start (unit: ns)
        double f2;                ///< Amplitude of the peak (unit: mV)
        double p1;                ///< Exponant of power law in region 1 (dimensionless)
        double Dt01;  ///< Region 1 duration (point of inflection, unit: ns)
        double Dt02;        ///< Peak time after start (unit: ns)
        double alpha1;     ///< Fraction of amplitude at region 2 start (dimensionless)
        double beta1; ///< Slope factor at point of inflection (dimensionless)
        double lambda3;           ///< First decay constant in region 3 (unit: ns^-1)
        double lambda4;           ///< Second decay constant in region 3 (unit: ns^-1)
        double alpha3;            ///< Proportion of first decay constant in region 3 (dimensionless)
      };
      
      /// Default constructor
      calo_signal_model();
      
      /// Default constructor
      calo_signal_model(const config_type & cfg_);

      /// Reset/invalidate
      void reset() override;

      /// Set the main parameters
      void set(const config_type cfg_);
 
      /// Set the main parameters
      void set(const double t0_,
               const double f2_,                             
               const double p1_,
               const double Dt01_,
               const double Dt02_,
               const double alpha1_,
               const double beta1_,
               const double lambda3_,
               const double lambda4_,
               const double alpha3_);
      
      double eval_f1(double t_) const;
      double eval_f2(double t_) const;
      double eval_f3(double t_) const;
      double eval_df1(double t_) const;
      double eval_df2(double t_) const;
      double eval_df3(double t_) const;
      double eval_root3(double t_) const;
      double eval_droot3(double t_) const;

      struct t3_root_functor 
        : public mygsl::i_unary_function_with_derivative
      {
        /// Constructor:
        t3_root_functor(const calo_signal_model &);

        double eval_df(double t_) const override;
 
      protected:
        
        //! The function evaluation abstract method
        double _eval(double t_) const override;

      private:
        
        const calo_signal_model & _sig_;
        
      };

      double get_t0() const
      {
        return _t0_;
      }
      
      double get_f2() const
      {
        return _f2_;
      }

      double get_p1() const
      {
        return _p1_;
      }

      double get_Dt01() const
      {
        return _Dt01_;
      }

      double get_Dt02() const
      {
        return _Dt02_;
      }

      double get_alpha1() const
      {
        return _alpha1_;
      }

      double get_beta1() const
      {
        return _beta1_;
      }

      double get_lambda3() const
      {
        return _lambda3_;
      }

      double get_lambda4() const
      {
        return _lambda4_;
      }

      double get_alpha3() const
      {
        return _alpha3_;
      }

      double get_t1() const
      {
        return _t1_;
      }

      double get_f1() const
      {
        return _f1_;
      }
      
      double get_t2() const
      {
        return _t2_;
      }
      
      double get_t3() const
      {
        return _t3_;
      }
      
    protected:

      //! The function evaluation abstract method
      double _eval(double t_) const override;

    private:

      void _construct_();

    private:

      // Configuration parameters:
      double _t0_; ///< Signal start  [ > 0.0] (unit: ns)
      double _f2_; ///< Amplitude of the peak [< 0.0] (unit: mV)
      double _p1_; ///< Exponent of power law in region 1 [>0.0, ~4-4.5]
      double _Dt01_;    ///< Region 1 duration (point of inflection) [>0.0, ~7] (unit: ns)
      double _Dt02_;    ///< Peak time after start
      double _alpha1_;  ///< Fraction of amplitude at region 2 start
      double _beta1_;   ///< Slope factor at point of inflection
      double _lambda3_; ///< First decay constant in region 3
      double _lambda4_; ///< Second decay constant in region 3
      double _alpha3_;  ///< Proportion of first decay constant in region 3

      // Computed parameters:
      double _t1_;  ///< Time of end of region 1 (begin of region 2) 
      double _f1_;  ///< Amplitude at end of region 1 (begin of region 2)
      double _df1_; ///< Derivative at end of region 1 (begin of region 2)
      double _a1_;  ///< Coefficient of power law in region 1 (<0.0)
      double _b1_;  ///< Coefficient of linear regime in region 1 (<0.0)
      double _t2_;  ///< Time of maximum amplitude peak
      double _a2_;  ///< Coefficient of power law in region 2 (<0.0)
      double _p2_;  ///< Exponent of power law in region 2
      double _t3_;  ///< Time of end of region 2
      double _f3_;  ///< Amplitude at end of region 2 (begin of region 3)
      double _Lambda3_;  ///< Weighted decay constant in region 3 
 
    };
    
      
  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_CALO_SIGNAL_MODEL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
