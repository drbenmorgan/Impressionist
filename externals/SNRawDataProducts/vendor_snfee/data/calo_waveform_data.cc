// Ourselves:
#include <snfee/data/calo_waveform_data.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux:
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/mygsl/error.h>
// #include <bayeux/mygsl/i_unary_function.h>
// #include <bayeux/mygsl/one_dimensional_root_finding.h>
// #include <bayeux/mygsl/kernel_smoother.h>
// #include <bayeux/mygsl/mean.h>
// #include <bayeux/mygsl/min_max.h>
// #include <bayeux/mygsl/linear_regression.h>

namespace snfee {
  namespace data {

    // ======================================================================== //

    calo_waveform::calo_waveform()
    {   
      return;
    }

    void calo_waveform::load(const std::string & filename_)
    {
      this->reset();
      std::string wf_path(filename_);
      datatools::fetch_path_with_env(wf_path);
      _tabfunc_.load_from_file(wf_path);
      this->lock();
      return;
    }

    void calo_waveform::add_sample(const double time_ns_, const double amp_mV_)
    {
      _tabfunc_.add_point(time_ns_, amp_mV_);
      return;
    }

    void calo_waveform::reserve(const uint16_t nsamples_)
    {
      _times_ns_.reserve(nsamples_);
      _amplitudes_mV_.reserve(nsamples_);
      return;
    }

    bool calo_waveform::is_locked() const
    {
      return _times_ns_.size() > 0;
    }

    std::size_t calo_waveform::size() const
    {
      return _tabfunc_.size();
    }

    void calo_waveform::lock()
    {
      _at_lock_();
      return;
    }

    void calo_waveform::_at_lock_()
    {
      if (!_tabfunc_.is_table_locked()) {
        _tabfunc_.lock_table("linear");
      }
      _tabfunc_.export_to_vectors(_times_ns_, _amplitudes_mV_);
      // const mygsl::tabulated_function::points_map_type & pm = _tabfunc_.points();
      // _times_ns_.reserve(pm.size());
      // _amplitudes_mV_.reserve(pm.size());
      // for (mygsl::tabulated_function::points_map_type::const_iterator i = pm.begin();
      //      i != pm.end();
      //      i++) {
      //   double ti = i->first;
      //   double fi = i->second;
      //   _times_ns_.push_back(ti);
      //   _amplitudes_mV_.push_back(fi);
      // }
      return;
    }

    double calo_waveform::get_time_step() const
    {
      if (!is_locked()) {
        return std::numeric_limits<double>::quiet_NaN();
      }
      return _times_ns_[1] - _times_ns_[0];
    }
    
    void calo_waveform::reset()
    {
      _tabfunc_.reset();
      _times_ns_.clear();
      _amplitudes_mV_.clear();
      return;
    }

    void calo_waveform::set_tabfunc(const mygsl::tabulated_function & twf_)
    {
      _tabfunc_ = twf_;
      lock();
      return;
    }


    const mygsl::tabulated_function & calo_waveform::get_tabfunc() const
    {
      return _tabfunc_;
    }
    
    const std::vector<double> & calo_waveform::get_times_ns() const
    {
      return _times_ns_;
    }
    
    const std::vector<double> & calo_waveform::get_amplitudes_mV() const 
    {
      return _amplitudes_mV_;
    }

    double calo_waveform::get_time_ns(const int cell_) const
    {
      return _times_ns_[cell_];
    }
    
    double calo_waveform::get_amplitude_mV(const int cell_) const
    {
      return _amplitudes_mV_[cell_];
    }

    // ======================================================================== //

    calo_waveform_baseline::calo_waveform_baseline()
    {
      invalidate();
      return;
    }

    void calo_waveform_baseline::invalidate()
    {
      quality_flags = 0; 
      datatools::invalidate(baseline_start_ns);
      datatools::invalidate(baseline_duration_ns);
      nb_samples = 0;
      datatools::invalidate(baseline_mV);
      datatools::invalidate(sigma_baseline_mV);
      datatools::invalidate(noise_mV);
      return;
    }
   
    bool calo_waveform_baseline::is_valid() const
    {
      if (!datatools::is_valid(baseline_start_ns)) return false;
      if (!datatools::is_valid(baseline_duration_ns)) return false;
      if (nb_samples == 0) return false;
      if (!datatools::is_valid(baseline_mV)) return false;
      if (!datatools::is_valid(sigma_baseline_mV)) return false;
      if (!datatools::is_valid(noise_mV)) return false;
      return true;
    }

    void calo_waveform_baseline::print(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_) const
    {
      static std::string tag = "|-- ";
      static std::string last_tag = "`-- ";
      std::ostringstream outs;
      if (!title_.empty()) {
        outs << indent_ << title_ << std::endl;
      }
      outs << indent_ << tag << "Baseline          : " << baseline_mV << " (mV)" << std::endl;
      outs << indent_ << tag << "Sigma baseline    : " << sigma_baseline_mV << " (mV)" << std::endl;
      outs << indent_ << tag << "Noise             : " << noise_mV << " (mV)" << std::endl;
      outs << indent_ << tag << "Baseline duration : " << baseline_duration_ns << " (ns)" << std::endl;
      outs << indent_ << last_tag << "Nb samples        : " << nb_samples << std::endl;  
      out_ << outs.str();
      return;
    }
 
    // ======================================================================== //
    
    calo_waveform_peak::calo_waveform_peak()
    {
      invalidate();
      return;
    }
  
    bool calo_waveform_peak::is_valid() const
    {
      if (!datatools::is_valid(time_ns)) return false;
      if (time_cell < 0) return false;
      if (!datatools::is_valid(amplitude_mV)) return false;
      if (!datatools::is_valid(time_head_90_percent_ns)) return false;
      if (!datatools::is_valid(time_tail_10_percent_ns)) return false;
      return true;
    }

    void calo_waveform_peak::invalidate()
    {
      quality_flags = 0;      
      time_cell = -1;
      time_head_10_percent_cell = -1;
      time_head_90_percent_cell = -1;
      time_tail_10_percent_cell = -1;
      time_tail_90_percent_cell = -1;
      datatools::invalidate(time_ns);
      datatools::invalidate(amplitude_mV);
      datatools::invalidate(time_head_10_percent_ns);
      datatools::invalidate(time_head_90_percent_ns);
      datatools::invalidate(time_tail_10_percent_ns);
      datatools::invalidate(time_tail_90_percent_ns);
      return;
    }

    double calo_waveform_peak::get_rising_time() const
    {
      return time_head_90_percent_ns - time_head_10_percent_ns;
    }

    double calo_waveform_peak::get_falling_time() const
    {
      return time_tail_10_percent_ns - time_tail_90_percent_ns;
    }

    void calo_waveform_peak::print(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_) const
    {
      static std::string tag = "|-- ";
      static std::string last_tag = "`-- ";
      std::ostringstream outs;
      if (!title_.empty()) {
        outs << indent_ << title_ << std::endl;
      }
      outs << indent_ << tag << "Quality flags   : " << quality_flags << std::endl;
      // outs << indent_ << tag << "Overflow        : " << std::boolalpha << overflow << std::endl;
      outs << indent_ << tag << "Time cell       : " << time_cell << std::endl;
      outs << indent_ << tag << "Head 10% cell   : " << time_head_10_percent_cell << std::endl;
      outs << indent_ << tag << "Head 90% cell   : " << time_head_90_percent_cell << std::endl;
      outs << indent_ << tag << "Tail 90% cell   : " << time_tail_90_percent_cell << std::endl;
      outs << indent_ << tag << "Tail 10% cell   : " << time_tail_10_percent_cell << std::endl;
      outs << indent_ << tag << "Time            : " << time_ns << " (ns)" << std::endl;
      outs << indent_ << tag << "Amplitude       : " << amplitude_mV << " (mV)" << std::endl;
      outs << indent_ << tag << "Sigma amplitude : " << sigma_amplitude_mV << " (mV)" << std::endl;
      outs << indent_ << tag << "Time head (10%) : " << time_head_10_percent_ns << " (ns)" << std::endl;
      outs << indent_ << tag << "Time head (90%) : " << time_head_90_percent_ns << " (ns)" << std::endl;
      outs << indent_ << tag << "Time tail (90%) : " << time_tail_90_percent_ns << " (ns)" << std::endl;  
      outs << indent_ << tag << "Time tail (10%) : " << time_tail_10_percent_ns << " (ns)" << std::endl;
      outs << indent_ << tag << "Rising time (10->90%)  : " << get_rising_time() << " (ns)" << std::endl;  
      outs << indent_ << last_tag << "Falling time (90->10%) : " << get_falling_time() << " (ns)" << std::endl;  
      out_ << outs.str();
      return;
    }
 
    // ======================================================================== //
    
    calo_waveform_cfd::calo_waveform_cfd()
    {
      invalidate();
      return;
    }
  
    bool calo_waveform_cfd::is_valid() const
    {
      if (!datatools::is_valid(fraction)) return false;
      if (ref_cell < 0) return false;
      if (!datatools::is_valid(time_ns)) return false;
      // if (!datatools::is_valid(sigma_time_ns)) return false;
      return true;
    }

    void calo_waveform_cfd::invalidate()
    {
      datatools::invalidate(fraction);
      datatools::invalidate(time_ns);
      datatools::invalidate(sigma_time_ns);
      return;
    }

    void calo_waveform_cfd::print(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_) const
    {
      static std::string tag = "|-- ";
      static std::string last_tag = "`-- ";
      std::ostringstream outs;
      if (!title_.empty()) {
        outs << indent_ << title_ << std::endl;
      }
      outs << indent_ << tag << "Fraction        : " << fraction << std::endl;
      outs << indent_ << tag << "Ref. cell       : " << ref_cell << std::endl;
      outs << indent_ << tag << "Time            : " << time_ns << " (ns)" << std::endl;
      outs << indent_ << last_tag << "Sigma time      : " << sigma_time_ns << " (ns)" << std::endl;
      out_ << outs.str();
      return;
    }
 
    // ======================================================================== //
  
    calo_waveform_charge::calo_waveform_charge()
    {
      invalidate();
      return;
    }
    
    bool calo_waveform_charge::is_valid() const
    {
      if (!datatools::is_valid(start_time_ns)) return false;
      if (!datatools::is_valid(stop_time_ns)) return false;
      if (!datatools::is_valid(charge_nVs)) return false;
      if (!datatools::is_valid(sigma_charge_nVs)) return false;
      return true;
    }

    void calo_waveform_charge::invalidate()
    {
      quality_flags = 0;      
      datatools::invalidate(start_time_ns);
      datatools::invalidate(stop_time_ns);
      datatools::invalidate(charge_nVs);
      datatools::invalidate(sigma_charge_nVs);
      return;
    }

    void calo_waveform_charge::print(std::ostream & out_,
                                     const std::string & title_,
                                     const std::string & indent_) const
   {
      static std::string tag = "|-- ";
      static std::string last_tag = "`-- ";
      std::ostringstream outs;
      if (!title_.empty()) {
        outs << indent_ << title_ << std::endl;
      }
      outs.precision(9);
      outs << indent_ << tag      << "Quality flags : " << quality_flags << std::endl;
      outs << indent_ << tag      << "Start time    : " << start_time_ns << " (ns)" << std::endl;
      outs << indent_ << tag      << "Stop time     : " << stop_time_ns << " (ns)" << std::endl;
      outs << indent_ << tag      << "Charge        : " << charge_nVs << " (nV.s)" << std::endl;
      outs << indent_ << last_tag << "Sigma charge  : " << sigma_charge_nVs << " (nV.s)";
      outs.precision(5);
      outs << " [" << 100 * sigma_charge_nVs / std::abs(charge_nVs) << " %]"
           << std::endl;
      out_ << outs.str();
      return;
    }
 
    // ======================================================================== //
     
    calo_waveform_firmware_data::calo_waveform_firmware_data()
    {
      invalidate();
      return;
    }

    bool calo_waveform_firmware_data::is_valid() const
    {
      if (!datatools::is_valid(baseline_mV)) return false;
      if (!datatools::is_valid(peak_mV)) return false;
      if (!datatools::is_valid(peak_time_ns)) return false;
      if (!datatools::is_valid(charge_nVs)) return false;
      if (!datatools::is_valid(cfd_time_ns)) return false;
      return true;
    }

    void calo_waveform_firmware_data::invalidate()
    {
      datatools::invalidate(baseline_mV);
      datatools::invalidate(peak_mV);
      datatools::invalidate(peak_time_ns);
      datatools::invalidate(charge_nVs);
      datatools::invalidate(cfd_time_ns);
      return;
    }

    void calo_waveform_firmware_data::print(std::ostream & out_,
                                            const std::string & title_,
                                            const std::string & indent_) const
    {
      static std::string tag = "|-- ";
      static std::string last_tag = "`-- ";
      std::ostringstream outs;
      if (!title_.empty()) {
        outs << indent_ << title_ << std::endl;
      }
      outs.precision(9);
      outs << indent_ << tag      << "Baseline       : " << baseline_mV << " (mV)" << std::endl;
      outs << indent_ << tag      << "Peak amplitude : " << peak_mV << " (mV)" << std::endl;
      outs << indent_ << tag      << "Peak time      : " << peak_time_ns << " (ns)" << std::endl;
      outs << indent_ << tag      << "Charge         : " << charge_nVs << " (nV.s)" << std::endl;
      outs << indent_ << last_tag << "CFD time       : " << cfd_time_ns << " (ns)" << std::endl;
      out_ << outs.str();
      return;
    }
 
    // ======================================================================== //

    void calo_waveform_info::reset(const bool preserve_raw_)
    {
      if (!preserve_raw_) {
        waveform.reset();
        has_fw_data = false;
        fw_data.invalidate();
      }
      has_baseline = false;
      baseline.invalidate();
      has_peak = false;
      peak.invalidate();
      has_cfd = false;
      cfd.invalidate();
      has_charge = false;
      charge.invalidate();
      return;
    }
                        
      
    void calo_waveform_info::print(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_) const
    {
      static std::string tag = "|-- ";
      static std::string last_tag = "`-- ";
      static std::string skip_tag = "|   ";
      static std::string last_skip_tag = "   ";
      std::ostringstream outs;
      if (!title_.empty()) {
        outs << indent_ << title_ << std::endl;
      }

      outs << indent_ << tag << "Firmware data : "
           << std::boolalpha << has_fw_data << std::endl;
      if (has_fw_data) {
        fw_data.print(outs, "", indent_ + skip_tag);
      }
 
      outs << indent_ << tag << "Baseline : "
           << std::boolalpha << has_baseline << std::endl;
      if (has_baseline) {
        baseline.print(outs, "", indent_ + skip_tag);
      }
      
      outs << indent_ << tag << "Peak : "
           << std::boolalpha << has_peak << std::endl;
      if (has_peak) {
        peak.print(outs, "", indent_ + skip_tag);
      }

      outs << indent_ << tag << "CFD : "
           << std::boolalpha << has_cfd << std::endl;
      if (has_cfd) {
        cfd.print(outs, "", indent_ + skip_tag);
      }

      outs << indent_ << last_tag << "Charge : "
           << std::boolalpha << has_charge << std::endl;
      if (has_charge) {
        charge.print(outs, "", indent_ + last_skip_tag);
      }

      out_ << outs.str();
      return;
    }
     
  } // namespace data
} // namespace snfee
