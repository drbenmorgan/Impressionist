//! \file snfee/io/rhd_sorter.h
//! \brief Sorter for RHD records
//
// Copyright (c) 2016-2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RHD_SORTER_H
#define SNFEE_IO_RHD_SORTER_H

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/io/rhd_record.h>

namespace snfee {
  namespace io {

    //! \brief Sorter algorithm for raw hit records
    class rhd_sorter
    {
    public:

      //! Constructor
      rhd_sorter();
 
      //! Destructor
      virtual ~rhd_sorter();
 
      //! Return logging priority
      datatools::logger::priority get_logging() const;

      //! Set logging priority
      void set_logging(const datatools::logger::priority);

      //! Push a new RHD record
      void push_record(const snfee::io::rhd_record & rhd_rec_);

      // //! Pop the next RHD record
      // const snfee::io::rhd_record & pop_next_record();
                       
    private:
    
      // Management:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

      struct pimpl_type;
      std::unique_ptr<pimpl_type> _pimpl_;
      
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RHD_SORTER_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
