// This project:
#include <snfee/data/raw_trigger_data.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/feb_constants.h>
#include <snfee/data/utils.h>

namespace snfee {
  namespace data {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(raw_trigger_data,
                                           "snfee::data::raw_trigger_data")

    raw_trigger_data::raw_trigger_data()
    {
      return;
    }

    raw_trigger_data::~raw_trigger_data()
    {
      return;
    }
  
    bool raw_trigger_data::is_complete() const
    {
      if (!has_run_id()) {
        return false;
      }
      if (!has_trigger_id()) {
        return false;
      }
      if (_trig_ and !_trig_->is_complete()) {
        std::cerr << "[devel] trig issue !" << std::endl;
        return false;
      }
      if (_calo_hits_.size() + _tracker_hits_.size() == 0) {
        std::cerr << "[devel] hits issue !" << std::endl;
        return false;
      }
      for (const auto & chit : _calo_hits_) {
        if (chit and !chit->is_complete()) {
          std::cerr << "[devel] calo hit issue !" << std::endl;
          return false;
        }
      }
      for (const auto & thit : _tracker_hits_) {
        if (thit and !thit->is_complete()) {
          std::cerr << "[devel] tracker hit issue !" << std::endl;
          return false;
        }
      }
      return true;
    }

    void raw_trigger_data::invalidate()
    {
      _run_id_ = INVALID_TRIGGER_ID;
      _trigger_id_ = INVALID_TRIGGER_ID;
      _trig_.reset();
      _calo_hits_.clear();
      _tracker_hits_.clear();
      return;
    }

    bool raw_trigger_data::has_run_id() const
    {
      return _run_id_ != INVALID_RUN_ID;
    }

    int32_t raw_trigger_data::get_run_id() const
    {
      return _run_id_;
    }

    void raw_trigger_data::set_run_id(const int32_t rid_)
    {
      _run_id_ = rid_;
      return;
    }
 
    bool raw_trigger_data::has_trigger_id() const
    {
      return _trigger_id_ != INVALID_TRIGGER_ID;
    }

    int32_t raw_trigger_data::get_trigger_id() const
    {
      return _trigger_id_;
    }

    void raw_trigger_data::set_trigger_id(const int32_t tid_)
    {
      _trigger_id_ = tid_;
      return;
    }
  
    // virtual
    void raw_trigger_data::print_tree(std::ostream & out_,
                                      const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      
      out_ << popts.indent << tag
           << "Run ID : " << _run_id_ << std::endl;
      }
      
      out_ << popts.indent << tag
           << "Trigger ID : " << _trigger_id_ << std::endl;
       
      out_ << popts.indent << tag
           << "Trigger record : ";
      if (!_trig_) {
        out_ << "<none>";
      } else if (!_trig_->is_complete()) {
        out_ << "<not complete>";
      }
      out_ << std::endl;
      if (_trig_ and _trig_->is_complete()) {
        std::ostringstream indent2;
        indent2 << popts.indent;
        indent2 << skip_tag;
        boost::property_tree::ptree trig_opts;
        trig_opts.put("indent", indent2.str());
        _trig_->print_tree(out_, trig_opts);
      }
     
      out_ << popts.indent << tag
           << "Calorimeter hit records : " << _calo_hits_.size() << std::endl;
      for (std::size_t i = 0; i < _calo_hits_.size(); i++) {
        const calo_hit_record & chit = *_calo_hits_[i];
        std::ostringstream indent2;
        indent2 << popts.indent << skip_tag;
        out_ << popts.indent << skip_tag;
        if (i + 1 == _calo_hits_.size()) {
          out_ << last_tag;
          indent2 << last_skip_tag;
        } else {
          out_ << tag;
          indent2 << skip_tag;
        }
        out_ << "Calo hit record #" << i << " : " << std::endl;
        boost::property_tree::ptree chit_opts;
        chit_opts.put("indent", indent2.str());
        chit.print_tree(out_, chit_opts);
      }
      
      out_ << popts.indent << tag
           << "Tracker hit records     : " << _tracker_hits_.size() << std::endl;
      for (std::size_t i = 0; i < _tracker_hits_.size(); i++) {
        const tracker_hit_record & thit = *_tracker_hits_[i];
        std::ostringstream indent2;
        indent2 << popts.indent << skip_tag;
        out_ << popts.indent << skip_tag;
        if (i + 1 == _tracker_hits_.size()) {
          out_ << last_tag;
          indent2 << last_skip_tag;
        } else {
          out_ << tag;
          indent2 << skip_tag;
        }
        out_ << "Tracker hit record #" << i << " : " << std::endl;
        boost::property_tree::ptree thit_opts;
        thit_opts.put("indent", indent2.str());
        thit.print_tree(out_, thit_opts);
      }
 
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete : " << std::boolalpha << is_complete() << std::endl;
                                                                    
      return;
    }

    bool raw_trigger_data::has_trig() const
    {
      return _trig_.get() != nullptr;
    }

    void raw_trigger_data::set_trig(const const_trigger_record_ptr & trig_)
    {
      DT_THROW_IF(!trig_ or !trig_->is_complete(), std::logic_error, "Trigger record is not complete!");
      if (!has_trigger_id()) {
        set_trigger_id(trig_->get_trigger_id());
      } else {
        DT_THROW_IF(trig_->get_trigger_id() != _trigger_id_,
                    std::logic_error,
                    "Set trigger ID [" <<_trigger_id_ << "] does not match trigger record's trigger ID [" << trig_->get_trigger_id() << "]!");
      }
      _trig_ = trig_;
      return;
    }

    const const_trigger_record_ptr & raw_trigger_data::get_trig() const
    {
      return _trig_;
    }

    const trigger_record & raw_trigger_data::get_trig_cref() const
    {
      DT_THROW_IF(!has_trig(), std::logic_error, "RTD has no trigger record!");
      return *_trig_.get();
    }

    void raw_trigger_data::append_calo_hit(const const_calo_hit_record_ptr & chrp_)
    {
      DT_THROW_IF(!chrp_ or !chrp_->is_complete(), std::logic_error, "Calo hit record is not complete!");
      _calo_hits_.push_back(chrp_);
      return;
    }

    const std::vector<const_calo_hit_record_ptr> & raw_trigger_data::get_calo_hits() const
    {
      return _calo_hits_;
    }

    void raw_trigger_data::append_tracker_hit(const const_tracker_hit_record_ptr & thrp_)
    {
      DT_THROW_IF(!thrp_ or !thrp_->is_complete(), std::logic_error, "Tracker hit record is not complete!");
      _tracker_hits_.push_back(thrp_);
      return;
    }
   
    const std::vector<const_tracker_hit_record_ptr> &
    raw_trigger_data::get_tracker_hits() const 
    {
      return _tracker_hits_;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const raw_trigger_data & rtd_)
    {
      std::ostringstream obuffer;
      obuffer << "{run-id=";
      if (rtd_.has_run_id()) {
        obuffer << rtd_.get_run_id();
      } else {
        obuffer << "none";
      }
      obuffer << ";trigger-id=";
      if (rtd_.has_trigger_id()) {
        obuffer << rtd_.get_trigger_id();
      } else {
        obuffer << "none";
      }
      obuffer << ";trig=";
      if (rtd_._trig_) {
        obuffer << "yes";
      } else {
        obuffer << "none";
      }
      obuffer << ";calo-hits="
           << rtd_._calo_hits_.size()
           << ";tracker-hits="
           << rtd_._tracker_hits_.size() << "}";
      out_ << obuffer.str();
      return out_;
    }

  } // namespace data
} // namespace snfee
