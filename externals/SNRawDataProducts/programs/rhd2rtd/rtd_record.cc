// This project:
#include "rtd_record.h"

namespace snfee {
  namespace io {

    rtd_record::rtd_record() { return; }

    void
    rtd_record::make_record(const int32_t run_id_, const int32_t trigger_id_)
    {
      _trigger_id_ = trigger_id_;
      _rtd_ = std::make_shared<snfee::data::raw_trigger_data>();
      _rtd_->set_run_id(run_id_);
      _rtd_->set_trigger_id(_trigger_id_);
      return;
    }

    void
    rtd_record::reset()
    {
      _trigger_id_ = snfee::data::INVALID_TRIGGER_ID;
      _rtd_.reset();
      return;
    }

    bool
    rtd_record::has_record() const
    {
      return _rtd_.get() != nullptr;
    }

    int32_t
    rtd_record::get_trigger_id() const
    {
      return _trigger_id_;
    }

    snfee::data::raw_trigger_data&
    rtd_record::grab_rtd()
    {
      DT_THROW_IF(!has_record(), std::logic_error, "No RTD record!");
      return *_rtd_.get();
    }

    const snfee::data::raw_trigger_data&
    rtd_record::get_rtd() const
    {
      DT_THROW_IF(!has_record(), std::logic_error, "No RTD record!");
      return *_rtd_.get();
    }

    const std::shared_ptr<snfee::data::raw_trigger_data>&
    rtd_record::get_rtd_ptr() const
    {
      return _rtd_;
    }

    void
    rtd_record::install_rhd(const snfee::io::rhd_record& rhd_rec_)
    {
      DT_THROW_IF(rhd_rec_.get_trigger_id() != _trigger_id_,
                  std::logic_error,
                  "Unmatching trigger ID [" << rhd_rec_.get_trigger_id()
                                            << "] != [" << _trigger_id_
                                            << "]!");
      if (rhd_rec_.is_trig()) {
        DT_THROW_IF(_rtd_->has_trig(),
                    std::logic_error,
                    "Trigger record is already set in RTD data!");
        _rtd_->set_trig(rhd_rec_.get_trig_rec());
      } else if (rhd_rec_.is_calo_hit()) {
        _rtd_->append_calo_hit(rhd_rec_.get_calo_hit_rec());
      } else if (rhd_rec_.is_tracker_hit()) {
        _rtd_->append_tracker_hit(rhd_rec_.get_tracker_hit_rec());
      }
      return;
    }

    void
    rtd_record::print(std::ostream& out_) const
    {
      std::ostringstream obuffer;
      obuffer << "RTD output record: " << std::endl;
      obuffer << "|-- "
              << "RTD record : ";
      if (_rtd_) {
        obuffer << *_rtd_;
      } else {
        obuffer << "<none>";
      }
      obuffer << std::endl;
      obuffer << "`-- "
              << "Trigger ID : ";
      int32_t trigid = _trigger_id_;
      if (trigid == snfee::data::INVALID_TRIGGER_ID) {
        obuffer << "<none>";
      } else {
        obuffer << trigid;
      }
      obuffer << std::endl;
      obuffer << std::endl;
      out_ << obuffer.str();
      return;
    }

    // friend
    std::ostream&
    operator<<(std::ostream& out_, const rtd_record& rec_)
    {
      std::ostringstream out;
      out << "RTD={payload=";
      if (!rec_._rtd_) {
        out << "none";
      } else {
        out << *rec_._rtd_;
      }
      out << ";trigger-id=" << rec_._trigger_id_;
      out << '}';
      out_ << out.str();
      return out_;
    }

  } // namespace io
} // namespace snfee
