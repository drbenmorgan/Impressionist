// This project:
#include "rhd_sorter.h"

// Standard Library:
#include <deque>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snfee {
  namespace io {

    struct trigger_id_buffer {
      trigger_id_buffer(const int32_t trigger_id_) : trigger_id(trigger_id_)
      {
        return;
      }

      int32_t trigger_id = snfee::data::INVALID_TRIGGER_ID;
      std::deque<rhd_record> queue;
    };

    struct rhd_sorter::pimpl_type {
      pimpl_type(rhd_sorter&);

      rhd_sorter& master;
    };

    rhd_sorter::pimpl_type::pimpl_type(rhd_sorter& master_) : master(master_)
    {
      return;
    }

    rhd_sorter::rhd_sorter()
    {
      _pimpl_.reset(new pimpl_type(*this));
      return;
    }

    rhd_sorter::~rhd_sorter()
    {
      _pimpl_.reset();
      return;
    }

    datatools::logger::priority
    rhd_sorter::get_logging() const
    {
      return _logging_;
    }

    void
    rhd_sorter::set_logging(const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    void
    rhd_sorter::push_record(const snfee::io::rhd_record& rhd_rec_)
    {
      int32_t trigid = rhd_rec_.get_trigger_id();
      DT_THROW_IF(trigid == snfee::data::INVALID_TRIGGER_ID,
                  std::logic_error,
                  "Invalid RHD record!");

      return;
    }

    // snfee::io::rhd_record rhd_sorter::pop_next_record()
    // {
    //   return;
    // }

  } // namespace io
} // namespace snfee
