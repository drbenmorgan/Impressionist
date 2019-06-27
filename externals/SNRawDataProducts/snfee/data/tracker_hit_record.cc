// This project:
#include <snfee/data/tracker_hit_record.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/feb_constants.h>
#include <snfee/model/utils.h>

namespace snfee {
  namespace data {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(tracker_hit_record,
                                           "snfee::data::tracker_hit_record")

    // static
    std::string
    tracker_hit_record::channel_category_label(const channel_category_type cc_)
    {
      if (cc_ == CHANNEL_ANODE)
        return std::string("anode");
      if (cc_ == CHANNEL_CATHODE)
        return std::string("cathode");
      return std::string("");
    }

    // static
    std::string
    tracker_hit_record::timestamp_category_label(
      const timestamp_category_type tc_)
    {
      if (tc_ == TIMESTAMP_ANODE_R0)
        return std::string("R0");
      if (tc_ == TIMESTAMP_ANODE_R1)
        return std::string("R1");
      if (tc_ == TIMESTAMP_ANODE_R2)
        return std::string("R2");
      if (tc_ == TIMESTAMP_ANODE_R3)
        return std::string("R3");
      if (tc_ == TIMESTAMP_ANODE_R4)
        return std::string("R4");
      if (tc_ == TIMESTAMP_CATHODE_R5)
        return std::string("R5");
      if (tc_ == TIMESTAMP_CATHODE_R6)
        return std::string("R6");
      return std::string("");
    }

    tracker_hit_record::tracker_hit_record() { return; }

    tracker_hit_record::~tracker_hit_record() { return; }

    bool
    tracker_hit_record::is_complete() const
    {
      if (_hit_num_ == INVALID_NUMBER)
        return false;
      if (_trigger_id_ == INVALID_TRIGGER_ID)
        return false;
      // if (_module_num_ == INVALID_NUMBER_16) return false;
      if (_crate_num_ == INVALID_NUMBER_16)
        return false;
      if (_board_num_ == INVALID_NUMBER_16)
        return false;
      if (_chip_num_ == INVALID_NUMBER_16)
        return false;
      if (_channel_num_ == INVALID_NUMBER_16)
        return false;
      if (_channel_category_ == CHANNEL_UNDEF)
        return false;
      if (_timestamp_category_ == TIMESTAMP_UNDEF)
        return false;
      if (_timestamp_ == INVALID_TIMESTAMP)
        return false;
      return true;
    }

    // virtual
    void
    tracker_hit_record::print_tree(
      std::ostream& out_,
      const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Hit number : " << _hit_num_ << std::endl;

      out_ << popts.indent << tag << "Trigger ID : " << _trigger_id_
           << std::endl;

      // out_ << popts.indent << tag
      //      << "Module number : " << _module_num_ << std::endl;

      out_ << popts.indent << tag << "Crate number : " << _crate_num_
           << std::endl;

      out_ << popts.indent << tag << "Board number : " << _board_num_
           << std::endl;

      out_ << popts.indent << tag << "FEAST chip number : " << _chip_num_
           << std::endl;

      out_ << popts.indent << tag << "FEAST channel number : " << _channel_num_
           << std::endl;

      out_ << popts.indent << tag << "FEAST channel category : '"
           << channel_category_label(_channel_category_) << "'" << std::endl;

      out_ << popts.indent << tag << "Timestamp category : '"
           << timestamp_category_label(_timestamp_category_) << "'"
           << std::endl;

      out_ << popts.indent << tag << "Timestamp : " << _timestamp_ << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete : " << std::boolalpha << is_complete() << std::endl;

      return;
    }

    void
    tracker_hit_record::set_hit_num(const int32_t hit_num_)
    {
      _hit_num_ = hit_num_;
      return;
    }

    void
    tracker_hit_record::set_trigger_id(const int32_t trig_id_)
    {
      _trigger_id_ = trig_id_;
      return;
    }

    int32_t
    tracker_hit_record::get_hit_num() const
    {
      return _hit_num_;
    }

    int32_t
    tracker_hit_record::get_trigger_id() const
    {
      return _trigger_id_;
    }

    // int16_t tracker_hit_record::get_module_num() const
    // {
    //   return _module_num_;
    // }

    int16_t
    tracker_hit_record::get_crate_num() const
    {
      return _crate_num_;
    }

    int16_t
    tracker_hit_record::get_board_num() const
    {
      return _board_num_;
    }

    int16_t
    tracker_hit_record::get_chip_num() const
    {
      return _chip_num_;
    }

    int16_t
    tracker_hit_record::get_channel_num() const
    {
      return _channel_num_;
    }

    tracker_hit_record::channel_category_type
    tracker_hit_record::get_channel_category() const
    {
      return _channel_category_;
    }

    tracker_hit_record::timestamp_category_type
    tracker_hit_record::get_timestamp_category() const
    {
      return _timestamp_category_;
    }

    uint64_t
    tracker_hit_record::get_timestamp() const
    {
      return _timestamp_;
    }

    void
    tracker_hit_record::invalidate()
    {
      _hit_num_ = INVALID_NUMBER;
      _trigger_id_ = INVALID_TRIGGER_ID;
      // _module_num_ = INVALID_NUMBER_16;
      _crate_num_ = INVALID_NUMBER_16;
      _board_num_ = INVALID_NUMBER_16;
      _chip_num_ = INVALID_NUMBER_16;
      _channel_num_ = INVALID_NUMBER_16;
      _channel_category_ = CHANNEL_UNDEF;
      _timestamp_category_ = TIMESTAMP_UNDEF;
      _timestamp_ = INVALID_TIMESTAMP;
      return;
    }

    void
    tracker_hit_record::make(const int32_t hit_num_,
                             const int32_t trigger_id_,
                             // const int16_t module_num_,
                             const int16_t crate_num_,
                             const int16_t board_num_,
                             const int16_t chip_num_,
                             const int16_t channel_num_,
                             const channel_category_type channel_category_,
                             const timestamp_category_type timestamp_category_,
                             const uint64_t timestamp_)

    {
      DT_THROW_IF(
        hit_num_ <= INVALID_NUMBER, std::logic_error, "Invalid hit number!");
      _hit_num_ = hit_num_;
      DT_THROW_IF(trigger_id_ <= INVALID_TRIGGER_ID,
                  std::logic_error,
                  "Invalid trigger ID!");
      _trigger_id_ = trigger_id_;
      // DT_THROW_IF(module_num_ != 0, std::logic_error, "Invalid module
      // number!"); _module_num_ = module_num_;
      DT_THROW_IF(crate_num_ < 0 or crate_num_ >= 3,
                  std::logic_error,
                  "Invalid crate number!");
      _crate_num_ = crate_num_;
      DT_THROW_IF(board_num_ < 0 and board_num_ > 19,
                  std::logic_error,
                  "Invalid board number!");
      _board_num_ = board_num_;
      DT_THROW_IF(chip_num_ != 0 and chip_num_ != 1,
                  std::logic_error,
                  "Invalid chip number!");
      _chip_num_ = chip_num_;
      DT_THROW_IF(channel_num_ < 0 and
                    channel_num_ >=
                      snfee::model::feb_constants::FEAST_NUMBER_OF_CHANNELS,
                  std::logic_error,
                  "Invalid FEAST channel number [" << channel_num_ << "]!");
      _channel_num_ = channel_num_;
      DT_THROW_IF(channel_category_ == CHANNEL_UNDEF,
                  std::logic_error,
                  "Invalid channel category [" << channel_category_ << "]!");
      _channel_category_ = channel_category_;
      DT_THROW_IF(timestamp_category_ == TIMESTAMP_UNDEF,
                  std::logic_error,
                  "Invalid timestamp category [" << timestamp_category_
                                                 << "]!");
      if (_channel_category_ == CHANNEL_ANODE) {
        if (timestamp_category_ == TIMESTAMP_CATHODE_R5 or
            timestamp_category_ == TIMESTAMP_CATHODE_R6) {
          DT_THROW(std::logic_error,
                   "Invalid anode timestamp category [" << timestamp_category_
                                                        << "]!");
        }
      }
      if (_channel_category_ == CHANNEL_CATHODE) {
        if (timestamp_category_ != TIMESTAMP_CATHODE_R5 and
            timestamp_category_ != TIMESTAMP_CATHODE_R6) {
          DT_THROW(std::logic_error,
                   "Invalid cathode timestamp category [" << timestamp_category_
                                                          << "]!");
        }
      }
      _timestamp_category_ = timestamp_category_;
      _timestamp_ = timestamp_;
      return;
    }

  } // namespace data
} // namespace snfee
