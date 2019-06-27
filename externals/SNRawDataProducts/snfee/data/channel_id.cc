// snfee/data/channel_id.cc
// Ourselves:
#include <snfee/data/channel_id.h>

// Third Party:
#include <boost/algorithm/string.hpp>

namespace snfee {
  namespace data {

    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(channel_id,
    // "snfee::data::channel_id")

    channel_id::channel_id() { return; }

    channel_id::channel_id(const int16_t crate_num_,
                           const int16_t board_num_,
                           const int16_t channel_num_)
    {
      this->set(DEFAULT_MODULE_NUMBER, crate_num_, board_num_, channel_num_);
      return;
    }

    channel_id::channel_id(const int16_t module_num_,
                           const int16_t crate_num_,
                           const int16_t board_num_,
                           const int16_t channel_num_)
    {
      this->set(module_num_, crate_num_, board_num_, channel_num_);
      return;
    }

    channel_id::~channel_id() { return; }

    bool
    channel_id::is_complete() const
    {
      if (_module_number_ == INVALID_NUMBER)
        return false;
      if (_crate_number_ == INVALID_NUMBER)
        return false;
      if (_board_number_ == INVALID_NUMBER)
        return false;
      if (_channel_number_ == INVALID_NUMBER)
        return false;
      return true;
    }

    void
    channel_id::reset()
    {
      _module_number_ = INVALID_NUMBER;
      _crate_number_ = INVALID_NUMBER;
      _board_number_ = INVALID_NUMBER;
      _channel_number_ = INVALID_NUMBER;
      return;
    }

    void
    channel_id::set(const int16_t module_num_,
                    const int16_t crate_num_,
                    const int16_t board_num_,
                    const int16_t channel_num_)
    {
      _module_number_ =
        (module_num_ <= INVALID_NUMBER ? INVALID_NUMBER : module_num_);
      _crate_number_ =
        (crate_num_ <= INVALID_NUMBER ? INVALID_NUMBER : crate_num_);
      _board_number_ =
        (board_num_ <= INVALID_NUMBER ? INVALID_NUMBER : board_num_);
      _channel_number_ =
        (channel_num_ <= INVALID_NUMBER ? INVALID_NUMBER : channel_num_);
      return;
    }

    int16_t
    channel_id::get_module_number() const
    {
      return _module_number_;
    }

    int16_t
    channel_id::get_crate_number() const
    {
      return _crate_number_;
    }

    int16_t
    channel_id::get_board_number() const
    {
      return _board_number_;
    }

    int16_t
    channel_id::get_channel_number() const
    {
      return _channel_number_;
    }

    // virtual
    void
    channel_id::print_tree(std::ostream& out_,
                           const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Complete : " << std::boolalpha
           << is_complete() << std::endl;

      out_ << popts.indent << tag << "Module  : [" << _module_number_ << "]"
           << std::endl;

      out_ << popts.indent << tag << "Crate   : [" << _crate_number_ << "]"
           << std::endl;

      out_ << popts.indent << tag << "Board   : [" << _board_number_ << "]"
           << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit) << "Channel : ["
           << _channel_number_ << "]" << std::endl;

      return;
    }

    // friend
    bool
    operator<(const channel_id& id0_, const channel_id& id1_)
    {
      if (id0_._module_number_ < id1_._module_number_)
        return true;
      if (id0_._module_number_ > id1_._module_number_)
        return false;
      if (id0_._crate_number_ < id1_._crate_number_)
        return true;
      if (id0_._crate_number_ > id1_._crate_number_)
        return false;
      if (id0_._board_number_ < id1_._board_number_)
        return true;
      if (id0_._board_number_ > id1_._board_number_)
        return false;
      if (id0_._channel_number_ < id1_._channel_number_)
        return true;
      return false;
    }

    // friend
    bool
    operator>(const channel_id& id0_, const channel_id& id1_)
    {
      if (id0_ == id1_)
        return false;
      if (id0_ < id1_)
        return false;
      return true;
    }

    // friend
    bool
    operator==(const channel_id& id0_, const channel_id& id1_)
    {
      if (id0_._module_number_ != id1_._module_number_)
        return false;
      if (id0_._crate_number_ != id1_._crate_number_)
        return false;
      if (id0_._board_number_ != id1_._board_number_)
        return false;
      if (id0_._channel_number_ != id1_._channel_number_)
        return false;
      return true;
    }

    // friend
    bool
    operator!=(const channel_id& id0_, const channel_id& id1_)
    {
      return !(id0_ == id1_);
    }

    std::string
    channel_id::to_string(const char symbol_, bool with_module_) const
    {
      std::ostringstream outs;
      if (symbol_ != 0) {
        outs << '[' << symbol_ << ':';
      }
      if (with_module_) {
        outs << _module_number_ << '.';
      }
      outs << _crate_number_;
      outs << '.' << _board_number_;
      outs << '.' << _channel_number_;
      if (symbol_ != 0) {
        outs << ']';
      }
      return outs.str();
    }

    bool
    channel_id::from_string(const std::string& token_)
    {
      this->reset();
      std::vector<std::string> tokens;
      boost::split(tokens, token_, boost::is_any_of("."));
      // std::cerr << "tokens = " << tokens.size() << std::endl;
      if (tokens.size() < 3 and tokens.size() > 4) {
        return false;
      }
      int16_t mn = 0;
      int16_t cn = INVALID_NUMBER;
      int16_t bn = INVALID_NUMBER;
      int16_t hn = INVALID_NUMBER;
      try {
        // std::cerr << "tokens[0] = " << tokens[0] << std::endl;
        // std::cerr << "tokens[1] = " << tokens[1] << std::endl;
        // std::cerr << "tokens[2] = " << tokens[2] << std::endl;
        int itok = -1;
        if (tokens.size() == 4) {
          itok++;
          mn = std::stoi(tokens[itok]);
        }
        // std::cerr << "mn = " << mn << std::endl;
        itok++;
        // std::cerr << "tokens[+] = " << tokens[itok] << std::endl;
        cn = std::stoi(tokens[itok]);
        itok++;
        // std::cerr << "tokens[+] = " << tokens[itok] << std::endl;
        bn = std::stoi(tokens[itok]);
        itok++;
        // std::cerr << "tokens[+] = " << tokens[itok] << std::endl;
        hn = std::stoi(tokens[itok]);
      }
      catch (std::exception& x) {
        return false;
      }
      _module_number_ = mn;
      _crate_number_ = cn;
      _board_number_ = bn;
      _channel_number_ = hn;
      return true;
    }

    // bool channel_id::export_to(sncabling::calo_signal_id & id_) const
    //{
    //  if (!this->is_complete()) {
    //    id_.invalidate();
    //    return false;
    //  }
    //  id_ = sncabling::calo_signal_id(sncabling::CALOSIGNAL_CHANNEL,
    //                                  _crate_number_,
    //                                  _board_number_,
    //                                  _channel_number_);
    //  return true;
    //}

  } // namespace data
} // namespace snfee
