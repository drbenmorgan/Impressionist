// Ourselves:
#include <snfee/data/rtd_selection.h>

// This project:
#include <snfee/data/trigger_record.h>
#include <snfee/data/calo_hit_record.h>
#include <snfee/data/tracker_hit_record.h>

namespace snfee {
  namespace data {

    calo_selection::calo_selection(const config_type & cfg_)
      : _crate_num_(cfg_.crate_num)
      , _board_num_(cfg_.board_num)
      , _chip_num_(cfg_.chip_num)
      , _reverse_(cfg_.reverse)
    {
      return;
    }
 
    calo_selection::calo_selection(const int16_t crate_num_,
                                   const int16_t board_num_,
                                   const int16_t chip_num_,
                                   const bool reverse_)
      : _crate_num_(crate_num_)
      , _board_num_(board_num_)
      , _chip_num_(chip_num_)
      , _reverse_(reverse_)
    {
      return;
    }

    bool calo_selection::is_activated() const
    {
      if (_crate_num_ == -1 and
          _board_num_ == -1 and
          _chip_num_ == -1) return false;
      return true;
    }
                     
    bool calo_selection::operator()(const snfee::data::raw_trigger_data & rtd_) const
    {
      if (!is_activated()) return true;
      uint32_t nb_matching_hits = 0;
      for (const auto & pchit : rtd_.get_calo_hits()) {
        const calo_hit_record & chit = *pchit;
        bool this_hit_matches = true;
        if (this_hit_matches and _crate_num_ != -1) {
          if (chit.get_crate_num() != _crate_num_) {
            this_hit_matches = false;
          }
        }
        if (this_hit_matches and _board_num_ != -1) {
          if (chit.get_board_num() != _board_num_) {
            this_hit_matches = false;
          }
        }
        if (this_hit_matches and _chip_num_ != -1) {
          if (chit.get_chip_num() != _chip_num_) {
            this_hit_matches = false;
          }
        }
        if (this_hit_matches) {
          nb_matching_hits = 0;
        }
      }
      bool select = nb_matching_hits > 0;
      if (_reverse_) {
        select = !select;
      }
      return select;
    }

  } // namespace data
} // namespace snfee
