//! \file snfee/io/rtd_record.h
//! \brief RTD record
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RTD_RECORD_H
#define SNFEE_IO_RTD_RECORD_H

// Standard Library:
#include <memory>

// This project:
#include <snfee/data/raw_trigger_data.h>
#include <snfee/io/rhd_record.h>

namespace snfee {
  namespace io {

    /// \brief RTD record wrapper for raw trigger data object
    class rtd_record
    {
    public:

      rtd_record();
      
      void make_record(const int32_t run_id_, const int32_t trigger_id_);
     
      void reset();
      
      bool has_record() const;
      
      int32_t get_trigger_id() const;

      snfee::data::raw_trigger_data & grab_rtd();

      const snfee::data::raw_trigger_data & get_rtd() const;

      const std::shared_ptr<snfee::data::raw_trigger_data> & get_rtd_ptr() const;

      void install_rhd(const snfee::io::rhd_record & rhd_rec_);
    
      void print(std::ostream & out_) const;

      friend std::ostream & operator<<(std::ostream & out_, const rtd_record & rec_);
   
    private:

      int32_t _trigger_id_ = snfee::data::INVALID_TRIGGER_ID; ///< Trigger ID
      std::shared_ptr<snfee::data::raw_trigger_data> _rtd_;   ///< Pointer to a raw trigger data (RTD)
      
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RTD_RECORD_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
