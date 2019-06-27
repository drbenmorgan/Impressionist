// This project:
#include <snfee/io/rhd_record.h>

namespace snfee {
  namespace io {

    rhd_record::rhd_record()
    {
      return;
    }

    rhd_record::rhd_record(const std::shared_ptr<snfee::data::trigger_record> & trig_rec_)
    {
      _trig_rec_ = trig_rec_;
      return;
    }

    rhd_record::rhd_record(const std::shared_ptr<snfee::data::calo_hit_record> & calo_hit_rec_)
    {
      _calo_hit_rec_ = calo_hit_rec_;
      return;
    }
 
    rhd_record::rhd_record(const std::shared_ptr<snfee::data::tracker_hit_record> & tracker_hit_rec_)
    {
      _tracker_hit_rec_ = tracker_hit_rec_;
      return;
    }
     
    void rhd_record::make_trig()
    {
      reset();
      _trig_rec_ = std::make_shared<snfee::data::trigger_record>();
      return;
    }
    
    void rhd_record::make_calo_hit()
    {
      reset();
      _calo_hit_rec_ = std::make_shared<snfee::data::calo_hit_record>();
      return;
    }
      
    void rhd_record::make_tracker_hit()
    {
      reset();
      _tracker_hit_rec_ = std::make_shared<snfee::data::tracker_hit_record>();
      return;
    }
      
    bool rhd_record::is_calo_hit() const
    {
      return _calo_hit_rec_.get() != nullptr;
    }
      
    bool rhd_record::is_tracker_hit() const
    {
      return _tracker_hit_rec_.get() != nullptr;
    }
      
    bool rhd_record::is_trig() const
    {
      return _trig_rec_.get() != nullptr;
    }
      
    bool rhd_record::empty() const
    {
      if (_calo_hit_rec_) return false;
      if (_tracker_hit_rec_) return false;
      if (_trig_rec_) return false;
      return true;
    }
      
    void rhd_record::reset()
    {
      _calo_hit_rec_.reset();
      _tracker_hit_rec_.reset();
      _trig_rec_.reset();
      return;
    }
      
    int32_t rhd_record::get_trigger_id() const
    {
      int32_t tid = snfee::data::INVALID_TRIGGER_ID;
      if (_calo_hit_rec_) {
        tid = _calo_hit_rec_->get_trigger_id();
      } else if (_tracker_hit_rec_) {
        tid = _tracker_hit_rec_->get_trigger_id();
      } else if (_trig_rec_) {
        tid = _trig_rec_->get_trigger_id();
      }
      return tid;
    }
 
    const std::shared_ptr<snfee::data::trigger_record> & rhd_record::get_trig_rec() const
    {
      return _trig_rec_;
    }

    const std::shared_ptr<snfee::data::tracker_hit_record> & rhd_record::get_tracker_hit_rec() const
    {
      return _tracker_hit_rec_;
    }
 
    const std::shared_ptr<snfee::data::calo_hit_record> & rhd_record::get_calo_hit_rec() const
    {
      return _calo_hit_rec_;
    }
      
    void rhd_record::print(std::ostream & out_) const
    {
      std::ostringstream out;
      out << "RHD input record: " << std::endl;
      out << "|-- Record type: ";
      if (is_calo_hit()) {
        out << "'calo'";
      } else if (is_tracker_hit()) {
        out << "'tracker'";
      } else if (is_trig()) {
        out << "'trig'";
      } else {
        out << "<none>";
      }
      out << std::endl;
      out << "`-- " << "Trigger ID : ";
      int32_t trigid = get_trigger_id();
      if (trigid == snfee::data::INVALID_TRIGGER_ID) {
        out << "<none>";
      } else {
        out << trigid;
      }
      out << std::endl;
      out << std::endl;
      out_ << out.str();
      return;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const rhd_record & rec_)
    {
      out_ << "RHD={record-type=";
      if (rec_.is_calo_hit()) {
        out_ << "'calo'";
      } else if (rec_.is_tracker_hit()) {
        out_ << "'tracker'";
      } else if (rec_.is_trig()) {
        out_ << "'trig'";
      } else {
        out_ << "<none>";
      }
      out_ << ";trigger-id=" << rec_.get_trigger_id();
      out_ << '}';
      return out_;
    }

    bool rhd_record_less::operator()(const rhd_record & rec1_, const rhd_record & rec2_)
    {
      if (rec1_.get_trigger_id() < rec2_.get_trigger_id()) {
        return true;
      }
      return false;
    }
   
  } // namespace io
} // namespace snfee
