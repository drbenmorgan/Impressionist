// This project:
#include <snfee/data/trigger_record.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:

namespace snfee {
  namespace data {
  
    DATATOOLS_SERIALIZATION_IMPLEMENTATION(trigger_record,
                                           "snfecore::trigger_record")

    // static
    std::string trigger_record::trigger_mode_label(const trigger_mode_type trigger_mode_)
    {
      switch (trigger_mode_) {
      case TRIGGER_MODE_CALO_ONLY : return std::string("calo_only");
      case TRIGGER_MODE_CALO_TRACKER_TIME_COINC : return std::string("calo_tracker_time_coincidence");
      case TRIGGER_MODE_CARACO : return std::string("CARACO");
      case TRIGGER_MODE_OPEN_DELAYED : return std::string("open_delayed");
      case TRIGGER_MODE_APE : return std::string("APE");
      case TRIGGER_MODE_DAVE : return std::string("DAVE");
      case TRIGGER_MODE_SCREENING : return std::string("screening");
      default:;
      }
      return std::string("");
    }
 
    trigger_record::trigger_record()
    {
      return;
    }

    trigger_record::~trigger_record()
    {
      return;
    }

    bool trigger_record::is_complete() const
    {
      if (_trigger_id_ == INVALID_TRIGGER_ID) return false;
      if (_trigger_mode_ == TRIGGER_MODE_INVALID) return false;
      if (_l2_clocktick_1600ns_ == 0) return false;
      return true;
    }
  
    // virtual
    void trigger_record::print_tree(std::ostream & out_,
                                    const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
    
      out_ << popts.indent << tag
           << "Trigger ID : " << _trigger_id_ << std::endl;
    
      out_ << popts.indent << tag
           << "Trigger mode : '" << trigger_mode_label(_trigger_mode_) << "'" << std::endl;
    
      out_ << popts.indent << tag
           << "L2 clocktick (1600 ns) : " << _l2_clocktick_1600ns_ << std::endl;
    
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete : " << std::boolalpha << is_complete() << std::endl;
     
      return;
    }

    void trigger_record::set_trigger_id(const int32_t tid_)
    {
      _trigger_id_ = tid_;
      return;
    }

    int32_t trigger_record::get_trigger_id() const
    {
      return _trigger_id_;
    }

    trigger_record::trigger_mode_type
    trigger_record::get_trigger_mode() const
    {
      return _trigger_mode_;
    }
  
    uint32_t trigger_record::get_l2_clocktick_1600ns() const
    {
      return _l2_clocktick_1600ns_;
    }

    void trigger_record::make(const uint32_t trigger_id_,
                              const trigger_mode_type trigger_mode_,
                              const uint32_t l2_clocktick_1600ns_)
    {
      _trigger_id_ = trigger_id_;
      _trigger_mode_ = trigger_mode_;
      _l2_clocktick_1600ns_ = l2_clocktick_1600ns_;
      return;
    }

    void trigger_record::invalidate()
    {
      _trigger_id_          = INVALID_TRIGGER_ID;     
      _trigger_mode_        = TRIGGER_MODE_INVALID;
      _l2_clocktick_1600ns_ = 0;

      // _l1_calo_decision_       = false;
      // _l1_calo_clocktick_25ns_ = 0;
      // _l1_tracker_decision_         = false;
      // _l1_tracker_clocktick_1600ns_ = 0;
      return;
    }

  } // namespace data
} // namespace snfee
