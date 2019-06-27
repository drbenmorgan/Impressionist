// This project:
#include <snfee/data/calo_channel_traits.h>

// Standard Library:
#include <random>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/feb_constants.h>
#include <snfee/data/calo_waveform_drawer.h>
#include <snfee/data/calo_waveform_data.h>

namespace snfee {
  namespace data {

    charge_range::charge_range()
    {
      datatools::invalidate(_lower_charge_nVs_);
      datatools::invalidate(_upper_charge_nVs_);
      return;
    }
    
    charge_range::charge_range(const double lower_charge_nVs_, const double upper_charge_nVs_)
    {
      set(lower_charge_nVs_, upper_charge_nVs_);
      return;
    }

    void charge_range::set(const double lower_charge_nVs_, const double upper_charge_nVs_)
    {
      DT_THROW_IF(lower_charge_nVs_ > 0.0, std::range_error,
                  "Invalid lower charge (Q=" << lower_charge_nVs_ << ")!");
      DT_THROW_IF(upper_charge_nVs_ >= 0.0, std::range_error,
                  "Invalid upper charge (Q=" << upper_charge_nVs_ << ")!");
      DT_THROW_IF(lower_charge_nVs_ <= upper_charge_nVs_, std::range_error,
                  "Invalid charge bounds [" << lower_charge_nVs_ << ":" << upper_charge_nVs_ <<"]!");
      _lower_charge_nVs_ = lower_charge_nVs_;
      _upper_charge_nVs_ = upper_charge_nVs_;
    }
    
    double charge_range::get_lower_charge_nVs() const
    {
      return _lower_charge_nVs_;
    }
    
    double charge_range::get_upper_charge_nVs() const
    {
      return _upper_charge_nVs_;
    }
 
    bool charge_range::inside(const double charge_nVs_) const
    {
      if (!datatools::is_valid(charge_nVs_)) return false;
      if (charge_nVs_ < _lower_charge_nVs_) return false;
      if (charge_nVs_ >= _upper_charge_nVs_) return false;
      return true;
    }

    // ========================================================= //

    calo_channel_traits::charge_range_info::charge_range_info()
    {
      invalidate();
      return;
    }

    bool calo_channel_traits::charge_range_info::is_valid() const
    {
      if (range_id < 0) return false;
      if (!datatools::is_valid(ref_time_ns)) return false;
      if (mean_signal.is_table_locked()) return false;
      if (!has_q2a_factor()) return false;
      return true;
    }

    void calo_channel_traits::charge_range_info::invalidate()
    {
      range_id = -1;
      datatools::invalidate(ref_time_ns);
      mean_signal.reset();
      datatools::invalidate(q2a_factor);      
      return;
    }

    bool calo_channel_traits::charge_range_info::has_q2a_factor() const
    {
      return datatools::is_valid(q2a_factor);
    }
  
    // virtual
    void calo_channel_traits::charge_range_info::print_tree(std::ostream & out_,
                                                            const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
  
      out_ << popts.indent << tag
           << "Charge range ID : " << range_id << std::endl;
  
      out_ << popts.indent << tag
           << "Reference time  : " << ref_time_ns << " ns" << std::endl;
  
      out_ << popts.indent << tag
           << "Mean signal shape : ";
      if (mean_signal.is_initialized()) {
        out_ << "<initialized>";
      } else {
        out_ << "<not defined>";
      }
      out_ << std::endl;
   
      out_ << popts.indent <<  inherit_tag(popts.inherit)
           << "Q/A factor : ";
      if (has_q2a_factor()) {
        out_ << q2a_factor << " mV/nV/s";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      
      return;
    }

    // ========================================================= //
    
    calo_channel_traits::hv_regime_info::hv_regime_info()
    {
      invalidate();
      return;
    }

    bool calo_channel_traits::hv_regime_info::is_valid() const
    {
      if (!datatools::is_valid(_nominal_hv_V_)) return false;
      if (!_baseline_.is_valid()) return false;
      if (_charge_ranges_.size() < 1) return false;
      if (_charge_range_infos_.size() < 1) return false;
      return true;
    }

    void calo_channel_traits::hv_regime_info::invalidate()
    {
      datatools::invalidate(_nominal_hv_V_);
      _baseline_.invalidate();
      _charge_ranges_.clear();
      clear_charge_range_infos();
      return;
    }

    void calo_channel_traits::hv_regime_info::set_nominal_hv_V(const double hv_)
    {
      _nominal_hv_V_ = hv_;
      return;
    }
        
    double calo_channel_traits::hv_regime_info::get_nominal_hv_V() const
    {
      return _nominal_hv_V_;
    }
 
    void calo_channel_traits::hv_regime_info::set_baseline(const calo_waveform_baseline & baseline_)
    {
      _baseline_ = baseline_;
      return;
    }
    
    const calo_waveform_baseline & calo_channel_traits::hv_regime_info::get_baseline() const
    {
      return _baseline_;
    }

    int calo_channel_traits::hv_regime_info::get_charge_range_index(const double q_nVs_) const
    {
      int i = -1;
      for (int j = 0; j < (int) _charge_ranges_.size();  j++) {
        if (_charge_ranges_[j].inside(q_nVs_)) {
          i = j;
          break;
        }
      }
      return i;
    }

    bool calo_channel_traits::hv_regime_info::has_charge_range(const double q_nVs_) const
    {
      return get_charge_range_index(q_nVs_) != -1;
    }

    const charge_range & calo_channel_traits::hv_regime_info::get_charge_range(const double q_nVs_) const
    {
      int i = get_charge_range_index(q_nVs_);
      DT_THROW_IF(i < 0, std::logic_error,
                  "No charge range is associated to charge " << q_nVs_ << " nV.s!");
      return _charge_ranges_[i];
    }

    int calo_channel_traits::hv_regime_info::add_charge_range(const charge_range & qgrp_)
    {
      if (_charge_ranges_.size() == 0) {
        _charge_ranges_.push_back(qgrp_);
      } else {
        DT_THROW_IF(qgrp_.get_lower_charge_nVs() != _charge_ranges_.back().get_upper_charge_nVs(),
                    std::domain_error,
                    "Not a valid charge range to be added, bounds don't match!");
        _charge_ranges_.push_back(qgrp_);
      }
      return (int) _charge_ranges_.size() - 1;
    }
         
    void calo_channel_traits::hv_regime_info::clear_charge_ranges()
    {
      _charge_ranges_.clear();
      return;
    }
 
    const std::vector<charge_range> &
    calo_channel_traits::hv_regime_info::get_charge_ranges() const
    {
      return _charge_ranges_;
    }

    void calo_channel_traits::hv_regime_info::add_charge_range_info(const charge_range_info & cgi_)
    {
      _charge_range_infos_[cgi_.range_id] = cgi_;
      return;
    }
        
    void calo_channel_traits::hv_regime_info::clear_charge_range_infos()
    {
      _charge_range_infos_.clear();
      return;
    }
        
    const calo_channel_traits::hv_regime_info::charge_range_info_dict_type &
    calo_channel_traits::hv_regime_info::get_charge_range_infos() const
    {
      return _charge_range_infos_;
    }

    bool calo_channel_traits::hv_regime_info::has_charge_range_info(const int range_id_) const
    {
      charge_range_info_dict_type::const_iterator found = _charge_range_infos_.find(range_id_);
      return found != _charge_range_infos_.end();
    }

    const calo_channel_traits::charge_range_info &
    calo_channel_traits::hv_regime_info::get_charge_range_info(const int range_id_) const
    {
      charge_range_info_dict_type::const_iterator found = _charge_range_infos_.find(range_id_);
      DT_THROW_IF(found == _charge_range_infos_.end(),
                  std::logic_error,
                  "No charge range info associated to range ID=" << range_id_ << "!");
      return found->second;
    }
  
    // virtual
    void calo_channel_traits::hv_regime_info::print_tree(std::ostream & out_,
                                                                 const boost::property_tree::ptree & options_) const
    {
      // interactive = options_.get("interactive", false);
      // bool print_waveform_samples = false;
      // print_waveform_samples = options_.get("with_waveform_samples", false);
      // interactive = options_.get("interactive", false);
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
  
      out_ << popts.indent << tag
           << "Nominal HV : " << _nominal_hv_V_ << " V" << std::endl;
  
      out_ << popts.indent << tag
           << "Baseline : " << std::endl;
      {
        std::ostringstream indent2s;
        indent2s << popts.indent << skip_tag;
        _baseline_.print(out_, "", indent2s.str());
      }
      
      out_ << popts.indent << tag
           << "Charge ranges : " << _charge_ranges_.size() << std::endl;
      for (int i = 0; i < (int) _charge_ranges_.size(); i++) {
        const charge_range & cr = _charge_ranges_[i];
        out_ << popts.indent << skip_tag;
        if (i + 1 == (int) _charge_ranges_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "#" << i << " : "
             << '[' << cr.get_lower_charge_nVs()  << ":" << cr.get_upper_charge_nVs() << "] nV.s" << std::endl;
      }
 
      out_ << popts.indent << tag
           << "Charge range infos : " << _charge_range_infos_.size() << std::endl;
      {
        std::size_t cri_counter = 0;
        for (charge_range_info_dict_type::const_iterator icri = _charge_range_infos_.begin();
             icri != _charge_range_infos_.end();
             icri++) {
          const charge_range_info & cri = icri->second;
          out_ << popts.indent << skip_tag;
          std::ostringstream indents2;
          indents2 << popts.indent << skip_tag;
          if (cri_counter + 1 == _charge_range_infos_.size()) {
            out_ << last_tag;
            indents2 << last_skip_tag;
          } else {
            out_ << tag;
            indents2 << skip_tag;
          }
          out_ << "Charge regime info #" << icri->first << " : " << std::endl;
          {
            boost::property_tree::ptree cri_poptions;
            cri_poptions.put("indent", indents2.str());
            cri.print_tree(std::clog, cri_poptions);
          }
          cri_counter++;
        }
      }

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Validity : " << is_valid() << std::endl;
       
      return;
    }

    // static
    void calo_channel_traits::hv_regime_info::display(const hv_regime_info & hvrinfo_,
                                                      const int range_id_,
                                                      const std::string & id_,
                                                      const std::string & title_,
                                                      const uint32_t /* flags_ */)
    {
      calo_waveform_drawer drawer;
      drawer.set_range_voltage(-1250.0, +150);
      drawer.set_auto_range_time(true);
      drawer.set_auto_range_voltage(true);
      calo_waveform_info wfinfo;
      if (hvrinfo_.has_charge_range_info(range_id_)) {
        wfinfo.waveform.set_tabfunc(hvrinfo_.get_charge_range_info(range_id_).mean_signal);
        wfinfo.has_baseline = true;
        wfinfo.baseline     = hvrinfo_._baseline_;
        std::string id = id_;
        std::string title = title_;
        if (title.empty()) {
          std::ostringstream titles;
          titles << "Mean waveform for HV = " << hvrinfo_.get_nominal_hv_V() << " V";
          title = titles.str();
        }
        drawer.draw(wfinfo, title, id);
      } else {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "No charge range info for charge range ID=" << range_id_ << "!");
      }
      return;
    }
    
    // ======================================================================== //
    
    calo_channel_traits::calo_channel_traits()
    {
      _flags_ = 0;
      return;
    }

    void calo_channel_traits::set_ch_id(const channel_id & chid_)
    {
      _ch_id_ = chid_;
      return;
    }

    const channel_id & calo_channel_traits::get_ch_id() const
    {
      return _ch_id_;
    }

    bool calo_channel_traits::has_flags(const uint32_t flags_) const
    {
      return _flags_ & flags_;
    }

    void calo_channel_traits::add_flags(const uint32_t flags_)
    {
      _flags_ |= flags_;
      return;
    }
      
    uint32_t calo_channel_traits::get_flags() const
    {
      return _flags_;
    }

    bool calo_channel_traits::is_dead() const
    {
      return has_flags(flag_dead_channel);
    }

    bool calo_channel_traits::is_5inch() const
    {
      return has_flags(flag_pmt_5inch);
    }
      
    bool calo_channel_traits::is_8inch() const
    {
      return has_flags(flag_pmt_8inch);
    }

    void calo_channel_traits::clear_hv_regime_infos()
    {
      _hv_regime_infos_.clear();
      return;
    }
    
    void calo_channel_traits::add_hv_regime_info(const hv_regime_info & char_)
    {
      int hv_mv = (int) (char_.get_nominal_hv_V() * 1000.0 + 0.000001);
      _hv_regime_infos_[hv_mv] = char_;
      return;
    }

    const calo_channel_traits::hv_regime_info_dict_type &
    calo_channel_traits::get_hv_regime_infos() const
    {
      return _hv_regime_infos_;
    }

    bool calo_channel_traits::has_hv_regime_info(const double hv_V_) const
    {
      int hv_mv = (int) (hv_V_ * 1000.0 + 0.000001);
      hv_regime_info_dict_type::const_iterator found = _hv_regime_infos_.find(hv_mv);
      return found != _hv_regime_infos_.end();
    }
    
    const calo_channel_traits::hv_regime_info &
    calo_channel_traits::get_hv_regime_info(const double hv_V_) const
    {
      int hv_mv = (int) (hv_V_ * 1000.0 + 0.000001);
      hv_regime_info_dict_type::const_iterator found = _hv_regime_infos_.find(hv_mv);
      DT_THROW_IF(found == _hv_regime_infos_.end(), std::logic_error,
                  "No characteristics associated to nominal HV = " << hv_V_ << " V!");
      return found->second;
    }

    /// Check if the PMT ID is set
    bool calo_channel_traits::has_pmt_id() const
    {
      return !_pmt_id_.empty();
    }
    
    /// Set the PMT ID
    void calo_channel_traits::set_pmt_id(const std::string & pmt_id_)
    {
      _pmt_id_ = pmt_id_;
      return;
    }
    
    /// Return the PMT ID
    const std::string & calo_channel_traits::get_pmt_id() const
    {
      return _pmt_id_;
    }
        
    // virtual
    void calo_channel_traits::print_tree(std::ostream & out_,
                                         const boost::property_tree::ptree & options_) const
    {
      // interactive = options_.get("interactive", false);
      // bool print_waveform_samples = false;
      // print_waveform_samples = options_.get("with_waveform_samples", false);
      // interactive = options_.get("interactive", false);
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
  
      out_ << popts.indent << tag
           << "Readout channel ID : " << _ch_id_.to_string() << std::endl;
  
      out_ << popts.indent << tag
           << "Flags              : " << _flags_ << std::endl;
 
      out_ << popts.indent << tag
           << "HV regime informations : " << _hv_regime_infos_.size() << std::endl;
      {
        std::size_t hvri_counter = 0;
        for (hv_regime_info_dict_type::const_iterator ichar = _hv_regime_infos_.begin();
             ichar != _hv_regime_infos_.end();
             ichar++) {
          const hv_regime_info & hvrinfo = ichar->second;
          out_ << popts.indent << skip_tag;
          std::ostringstream indents2;
          indents2 << popts.indent << skip_tag;
          if (hvri_counter + 1 == _hv_regime_infos_.size()) {
            out_ << last_tag;
            indents2 << last_skip_tag;
          } else {
            out_ << tag;
            indents2 << skip_tag;
          }
          out_ << "Informations for HV = " << ichar->first / 1000 << " V " << std::endl;
          {
            boost::property_tree::ptree hvri_poptions;
            hvri_poptions.put("indent", indents2.str());
            hvrinfo.print_tree(std::clog, hvri_poptions);
          }
          hvri_counter++;
        }
      }   

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "PMT ID    : ";
      if (has_pmt_id()) {
        out_ << _pmt_id_;
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      
      return;
    }

  } // namespace data
} // namespace snfee
