// This project:
#include <snfee/data/calo_signal_model.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux
#include <bayeux/mygsl/one_dimensional_root_finding.h>

namespace snfee {
  namespace data {

    calo_signal_model::calo_signal_model()
    {
      set_epsilon(1.e-7);
      reset();
      return;
    }

    calo_signal_model::calo_signal_model(const config_type & cfg_)
    {
      set_epsilon(1.e-7);
      reset();
      set(cfg_);
      return;
    }

    void calo_signal_model::reset()
    {
      datatools::invalidate(_t0_);
      datatools::invalidate(_f2_);
      datatools::invalidate(_p1_);
      datatools::invalidate(_Dt01_);
      datatools::invalidate(_Dt02_);
      datatools::invalidate(_alpha1_);
      datatools::invalidate(_beta1_);
      datatools::invalidate(_lambda3_);
      datatools::invalidate(_lambda4_);
      datatools::invalidate(_alpha3_);

      datatools::invalidate(_t1_);
      datatools::invalidate(_f1_);
      datatools::invalidate(_df1_);
      datatools::invalidate(_a1_);
      datatools::invalidate(_b1_);
      datatools::invalidate(_t2_);
      datatools::invalidate(_a2_);
      datatools::invalidate(_p2_);
      datatools::invalidate(_t3_);
      datatools::invalidate(_f3_);
      datatools::invalidate(_Lambda3_);
      
      i_unary_function::_base_reset();
      return;
    }
    
    void calo_signal_model::set(const config_type cfg_)
    {
      set(cfg_.t0,
          cfg_.f2,                             
          cfg_.p1,
          cfg_.Dt01,
          cfg_.Dt02,
          cfg_.alpha1,
          cfg_.beta1,
          cfg_.lambda3,
          cfg_.lambda4,
          cfg_.alpha3);
      return;
    }
   
    void calo_signal_model::set(const double t0_,
                                const double f2_,                             
                                const double p1_,
                                const double Dt01_,
                                const double Dt02_,
                                const double alpha1_,
                                const double beta1_,
                                const double lambda3_,
                                const double lambda4_,
                                const double alpha3_)
    {
      bool devel = false;
      // devel = true;
      DT_THROW_IF(t0_ < 0.0, std::domain_error, "Invalid time start (" << t0_ << " ns)!");
      DT_THROW_IF(f2_ <= 0.0, std::domain_error, "Invalid signal maximum amplitude (" << f2_ << " ns)!");
      DT_THROW_IF(Dt01_ < 1.0, std::domain_error, "Invalid region1 duration (" << Dt01_ << " ns)!");
      DT_THROW_IF(Dt02_ <= Dt01_, std::domain_error, "Invalid peak delay (" << Dt02_ << " ns)!");
      DT_THROW_IF(alpha1_ <= 0.1 or alpha1_ >= 0.9, std::domain_error, "Invalid flex fraction (" << alpha1_ << " ns)!");
      DT_THROW_IF(beta1_ <= 0.0, std::domain_error, "Invalid flex slope factor (" << beta1_ << " A.U.)!");
      DT_THROW_IF(lambda3_ <= 0.0, std::domain_error, "Invalid lambda3 (" << lambda3_ << " /ns)!");
      DT_THROW_IF(lambda4_ <= 0.0, std::domain_error, "Invalid lambda4 (" << lambda4_ << " /ns)!");
      DT_THROW_IF(alpha3_ <= 0.5 or alpha3_ >= 0.95, std::domain_error, "Invalid alpha3 (" << alpha3_ << " /ns)!");
      _t0_ = t0_;
      if (devel) std::cerr << "[devel] t0 = " << _t0_ << " ns" << std::endl;
      _f2_ = f2_;
      if (devel) std::cerr << "[devel] f2 = " << _f2_ << " mV" << std::endl;
      _p1_ = p1_;
      if (devel) std::cerr << "[devel] p1 = " << _p1_ << " " << std::endl;
      _Dt01_ = Dt01_;
      if (devel) std::cerr << "[devel] Dt01 = " << _Dt01_ << " ns" << std::endl;
      _Dt02_ = Dt02_;
      if (devel) std::cerr << "[devel] Dt02 = " << _Dt02_ << " ns" << std::endl;
      _alpha1_ = alpha1_;
      if (devel) std::cerr << "[devel] alpha1 = " << _alpha1_ << " " << std::endl;
      _beta1_ = beta1_;
      if (devel) std::cerr << "[devel] beta1 = " << _beta1_ << " " << std::endl;
      _lambda3_ = lambda3_;
      if (devel) std::cerr << "[devel] lambda3 = " << _lambda3_ << " " << std::endl;
      _lambda4_ = lambda4_;
      if (devel) std::cerr << "[devel] lambda4 = " << _lambda4_ << " " << std::endl;
      _alpha3_ = alpha3_;
      if (devel) std::cerr << "[devel] alpha3 = " << _alpha3_ << " " << std::endl;
      _construct_();
      return;
    }

    calo_signal_model::t3_root_functor::t3_root_functor(const calo_signal_model & s_)
      : _sig_(s_)
    {
      return;
    }

    double calo_signal_model::t3_root_functor::_eval(double t_) const
    {
      return _sig_.eval_root3(t_);
    }
    
    double calo_signal_model::t3_root_functor::eval_df(double t_) const
    {
      return _sig_.eval_droot3(t_);
    }

    void calo_signal_model::_construct_()
    {
      bool devel = false;
      // devel = true;
      _t1_ = _t0_ + _Dt01_;
      if (devel) std::cerr << "[devel] t1 = " << _t1_ << " ns" << std::endl;
      _f1_ = _f2_ * _alpha1_;
      if (devel) std::cerr << "[devel] f1 = " << _f1_ << " mV" << std::endl;
      _df1_ = _f2_ * _beta1_;
      if (devel) std::cerr << "[devel] df1 = " << _df1_ << " mV/ns" << std::endl;
      double dt01 = _Dt01_;
      if (devel) std::cerr << "[devel] dt01 = " << dt01 << " ns" << std::endl;
      _a1_ = (_df1_ * dt01 - _f1_) / (std::pow(dt01, _p1_) * (_p1_ - 1));
      if (devel) std::cerr << "[devel] a1 = " << _a1_ << " a.u." << std::endl;
      _b1_ = (_f1_ - _a1_ * std::pow(dt01, _p1_)) / dt01;
      if (devel) std::cerr << "[devel] b1 = " << _b1_ << " a.u." << std::endl;
      _t2_ = _t0_ + _Dt02_;
      if (devel) std::cerr << "[devel] t2 = " << _t2_ << " ns" << std::endl;
      double dt12 = _t2_ - _t1_;
      if (devel) std::cerr << "[devel] dt12 = " << dt12 << " ns" << std::endl;
      double df12 = _f2_ - _f1_;
      if (devel) std::cerr << "[devel] df12 = " << df12 << " mV" << std::endl;
      _p2_ = _df1_ * dt12 / df12;
      if (devel) std::cerr << "[devel] p2 = " << _p2_ << " " << std::endl;
      _a2_ = df12 / std::pow(dt12, _p2_);
      if (devel) std::cerr << "[devel] a2 = " << _a2_ << " a.u." << std::endl;
      _Lambda3_ = (_alpha3_ * _lambda3_ + (1.0 - _alpha3_) * _lambda4_);
      if (devel) std::cerr << "[devel] Lambda3 = " << _Lambda3_ << " a.u." << std::endl;

      double Dt23 = 0.0;
      t3_root_functor f(*this);
      double rmin = 0.0;
      double fmin = f(rmin);
      double rmax = 1. / _lambda4_;
      double fmax = f(rmax);
      // if (fmin * fmax >= 0.0) {
      //        DT_THROW(std::logic_error, "Cannot compute signal tail shape (fmin * fmax >= 0.0) !");        
      // }
      
      if (fmin * fmax < 0.0 or _p2_ > 1.0) {
        mygsl::one_dimensional_root_solver odrs;
        std::string odrs_method_name
          = mygsl::one_dimensional_root_solver::BRENT_METHOD_LABEL;
        odrs.init(f, odrs_method_name);
        // devel = true;
        if (devel) std::cerr << "calo_signal_model::_construct_ : " << std::endl;
        if (devel) std::cerr << "[devel] rmin = " << rmin <<  "  fmin = " << fmin << std::endl;
        if (devel) std::cerr << "[devel] rmax = " << rmax <<  "  fmax = " << fmax << std::endl;
        double r1 = rmin;
        double f1 = fmin;
        double r2 = rmax;
        double f2 = fmax;
        double dr = 0.1;
        for (double r = rmin + dr; r < rmax; r += dr) {
          double fr = f(r);
          // if (devel) std::cerr << "[devel] r = " << r  << "   fr = " << fr << std::endl;
          if (fr * f1 < 0.0) {
            r2 = r;
            f2 = fr;
            break;
          } else {
            r1 = r;
            f1 = fr;
          }
        }
        if (devel) std::cerr << "[devel] r1 = " << r1  << "   f1 = " << f1 << std::endl;
        if (devel) std::cerr << "[devel] r2 = " << r2  << "   f2 = " << f2 << std::endl;
        double odrs_eps = 0.01;
        if (odrs.solve(odrs_eps, r1, r2) != 0) {
          DT_THROW(std::logic_error, "Cannot compute signal tail shape!");
        }
        Dt23 = odrs.get_root();
        if (devel) std::cerr << "[devel] root = " << Dt23 << std::endl;
      }
      _t3_ = _t2_ + Dt23;
      
      if (devel) std::cerr << "[devel] t3 = " << _t3_ << " ns" << std::endl;
      _f3_ = -eval_f2(_t3_);
      // devel = true;
      if (devel) std::cerr << "[devel] f3 = " << _f3_ << " mV" << std::endl;
      if (devel) std::cerr << "[devel] f2(t3) = " << eval_f2(_t3_) << " mV" << std::endl;
      if (devel) std::cerr << "[devel] f3(t3) = " << eval_f3(_t3_) << " mV" << std::endl;
      if (devel) std::cerr << "[devel] f2'(t3) = " << eval_df2(_t3_) << " mV/ns" << std::endl;
      if (devel) std::cerr << "[devel] f3'(t3) = " << eval_df3(_t3_) << " mV/ns" << std::endl;
      return;
    }
    
    double calo_signal_model::eval_f1(double t_) const
    {
      double dt0 = t_ - _t0_;
      double f = 0.0;
      if (dt0 > 0.0) {
        f = _a1_ * std::pow(dt0, _p1_) + _b1_ * dt0;
      }
      return -f;
    }

    double calo_signal_model::eval_df1(double t_) const
    {
      double dt0 = t_ - _t0_;
      double df = _a1_ * _p1_ * std::pow(dt0, _p1_ - 1) + _b1_ ;
      return -df;
    }
   
    double calo_signal_model::eval_f2(double t_) const
    {
      double dt2 = std::abs(t_ - _t2_);
      double f = _f2_ - _a2_ * std::pow(dt2, _p2_);
      return -f;
    }

    double calo_signal_model::eval_df2(double t_) const
    {
      double df = 0.0;
      if (t_ < _t2_) {
        df = _a2_ * _p2_ * std::pow(_t2_ - t_, _p2_ - 1);
      } else {
        if (_t3_ > _t2_) {
          df = - _a2_ * _p2_ * std::pow(t_ - _t2_, _p2_ - 1);
        } else {
          df = 0.0;
        }
      }
      return -df;
    }
     
    double calo_signal_model::eval_f3(double t_) const
    {
      double f = _f3_ * (_alpha3_ * std::exp(- _lambda3_ * (t_ - _t3_))
                         +
                         (1.0 - _alpha3_) * std::exp(- _lambda4_ * (t_ - _t3_)));
      return -f;
    }
     
    double calo_signal_model::eval_df3(double t_) const
    {
      double df = - _f3_ * (_alpha3_ * _lambda3_ * std::exp(- _lambda3_ * (t_ - _t3_))
                            +
                            (1.0 - _alpha3_) * _lambda4_ * std::exp(- _lambda4_ * (t_ - _t3_)));
      return -df;
    }
    
    double calo_signal_model::eval_root3(double t_) const
    {
      return _Lambda3_ * _a2_ * std::pow(t_, _p2_)
        + _a2_ * _p2_ * std::pow(t_, _p2_ - 1)
        - _Lambda3_ * _f2_;
    }
    
    double calo_signal_model::eval_droot3(double t_) const
    {
      return - _Lambda3_ * _a2_ * _p2_ * std::pow(t_, _p2_ - 1)   
        + _a2_ * _p2_ * (_p2_ - 1) * std::pow(t_, _p2_ - 2);
    }

    // virtual
    double calo_signal_model::_eval(double t_) const
    {
      if (t_ < _t0_) {
        return 0.0;
      }
      if (t_ < _t1_) {
        return eval_f1(t_);
      }
      if (t_ < _t3_) {
        return eval_f2(t_);
      }
      return eval_f3(t_);
    }

  } // namespace data
} // namespace snfee
