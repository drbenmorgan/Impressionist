//! \file snfee/io/rhd_record.h
//! \brief RHD record
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RHD_RECORD_H
#define SNFEE_IO_RHD_RECORD_H

// Standard Library:
#include <memory>

// This project:
#include <snfee/data/calo_hit_record.h>
#include <snfee/data/tracker_hit_record.h>
#include <snfee/data/trigger_record.h>

namespace snfee {
  namespace io {

    /// \brief RHD record wrapper for a calo hit record, a tracker hit record or
    /// a trigger record
    class rhd_record {
    public:
      /// Constructor
      rhd_record();

      /// Constructor from a trigger record
      rhd_record(const std::shared_ptr<snfee::data::trigger_record>& trig_rec_);

      /// Constructor from a calo hit record
      rhd_record(
        const std::shared_ptr<snfee::data::calo_hit_record>& calo_hit_rec_);

      /// Constructor from a tracker hit record
      rhd_record(const std::shared_ptr<snfee::data::tracker_hit_record>&
                   tracker_hit_rec_);

      /// Make this record a trigger record
      void make_trig();

      /// Make this record a calo hit record
      void make_calo_hit();

      /// Make this record a tracker hit record
      void make_tracker_hit();

      /// Check if this record is a trigger record
      bool is_trig() const;

      /// Check if this record is a calo hit record
      bool is_calo_hit() const;

      /// Check if this record is a tracker hit record
      bool is_tracker_hit() const;

      /// Check if this record is not set
      bool empty() const;

      /// Reset this record
      void reset();

      /// Get the trigger ID of the record
      int32_t get_trigger_id() const;

      /// Return the embedded trigger record
      const std::shared_ptr<snfee::data::trigger_record>& get_trig_rec() const;

      /// Return the embedded calo hit record
      const std::shared_ptr<snfee::data::tracker_hit_record>&
      get_tracker_hit_rec() const;

      /// Return the embedded tracker hit record
      const std::shared_ptr<snfee::data::calo_hit_record>& get_calo_hit_rec()
        const;

      /// Print
      void print(std::ostream& out_) const;

      /// Print
      friend std::ostream& operator<<(std::ostream& out_,
                                      const rhd_record& rec_);

    private:
      std::shared_ptr<snfee::data::trigger_record>
        _trig_rec_; ///< Embedded trigger record
      std::shared_ptr<snfee::data::calo_hit_record>
        _calo_hit_rec_; ///< Embedded calo hit record
      std::shared_ptr<snfee::data::tracker_hit_record>
        _tracker_hit_rec_; ///< Embedded tracker hit record
    };

    struct rhd_record_less {
      bool operator()(const rhd_record& rec1_, const rhd_record& rec2_);
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RHD_RECORD_H
