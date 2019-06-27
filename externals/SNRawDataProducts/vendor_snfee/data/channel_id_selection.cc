// Ourselves:
#include <snfee/data/channel_id_selection.h>

// This project:

namespace snfee {
  namespace data {

    channel_id_selection::channel_id_selection()
    {
      return;
    }
 
    channel_id_selection::channel_id_selection(const config_type & cfg_)
    {
      configure(cfg_);
      return;
    }

    void channel_id_selection::configure(const config_type & cfg_)
    {
      for (const auto n : cfg_.selected_crates) {
        _selected_crates_.insert(n);
      }
      for (const auto n : cfg_.selected_boards) {
        _selected_boards_.insert(n);
      }
      for (const auto n : cfg_.selected_channels) {
        _selected_channels_.insert(n);
      }
      for (const auto & id : cfg_.selected_ids) {
        _selected_ids_.insert(id);
      }
      _reverse_ = cfg_.reverse;
      return;
    }
 
    void channel_id_selection::add_crate(const int16_t num_)
    {
      _selected_crates_.insert(num_);
      return;
    }

    void channel_id_selection::add_board(const int16_t num_)
    {
      _selected_boards_.insert(num_);
      return;
    }

    void channel_id_selection::add_channel(const int16_t num_)
    {
      _selected_channels_.insert(num_);
      return;
    }

    void channel_id_selection::add_id(const channel_id & id_)
    {
      _selected_ids_.insert(id_);
      return;
    }

    void channel_id_selection::set_reverse(const bool r_)
    {
      _reverse_ = r_;
      return;
    }

    bool channel_id_selection::operator()(const snfee::data::channel_id & chid_) const
    {
      bool select = true;
      
      if (!_selected_crates_.empty()) {
        if (!_selected_crates_.count(chid_.get_crate_number())) {
          select = false;
        }
      }
      if (!_selected_boards_.empty()) {
        if (!_selected_boards_.count(chid_.get_board_number())) {
          select = false;
        }
      }
      if (!_selected_channels_.empty()) {
        if (!_selected_channels_.count(chid_.get_channel_number())) {
          select = false;
        }
      }
      if (!_selected_ids_.empty()) {
        if (!_selected_ids_.count(chid_)) {
          select = false;
        }
      }
      if (_reverse_) {
        select = !select;
      }
      return select;
    }

  } // namespace data
} // namespace snfee
