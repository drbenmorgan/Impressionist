//! \file snfee/io/multifile_data_writer.h
//! \brief Multifile data writer
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_MULTIFILE_DATA_WRITER_H
#define SNFEE_IO_MULTIFILE_DATA_WRITER_H

// Standard library:
#include <string>
#include <memory>

// Third party:
// - Boost:
#include <boost/utility.hpp>
// - Bayeux:
#include <bayeux/datatools/io_factory.h>
#include <bayeux/datatools/logger.h>

namespace snfee {
  namespace io {

    //! \brief Multifile data writer
    class multifile_data_writer
      : private boost::noncopyable
    {
    public:

      /// \brief Configuration data:
      struct config_type
      {
        std::vector<std::string> filenames;       ///< Sequence of output filenames
        std::size_t max_records_per_file = 0;     ///< Maximum number of records per file
        std::size_t max_total_records    = 0;     ///< Maximum total number of records
        bool        terminate_on_overrun = false; ///< Soft terminate at file overrun (to many records w/r to the file capacity)
      };

      //! Default constructor
      multifile_data_writer(const config_type & config_params_,
                            const datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);

      //! Destructor
      virtual ~multifile_data_writer();
 
      //! Check if the writer is terminated
      bool is_terminated() const;

      //! Check if the writer reach the last file
      bool is_last_file() const;

      /// Force termination of the writer
      void terminate();

      /// Return the number of loaded records
      std::size_t get_counter() const;

      /// Return the file counter
      std::size_t get_file_counter() const;

      /// Add an output filename
      void add_filename(const std::string & filename_);
      
      //! Store an arbitrary serialization record
      template <typename Data>
      void store(Data & data_)
      {
        _pre_store_();
        if (!_terminated_) {
          _writer_().store(data_);
          _post_store_();
        }
        return;
      }
      
    private:

      void _pre_store_(); //!< Pre-store action
      
      void _post_store_(); //!< Post-store action
      
      datatools::data_writer & _writer_(); //!< Return a ref to the current writer
  
    private:

      // Configuration:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      config_type _config_;      ///< Configuration

      // Management:
      bool _terminated_ = false; ///< Forced termination flag
      std::size_t _counter_ = 0; ///< Record counter
      
      struct pimpl_type;
      std::unique_ptr<pimpl_type> _pimpl_; ///< Private working data
      
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_MULTIFILE_DATA_WRITER_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
