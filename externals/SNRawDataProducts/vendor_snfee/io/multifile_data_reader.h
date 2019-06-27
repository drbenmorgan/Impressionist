//! \file snfee/io/multifile_data_reader.h
//! \brief Multifile data reader
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_MULTIFILE_DATA_READER_H
#define SNFEE_IO_MULTIFILE_DATA_READER_H

// Standard library:
#include <string>
#include <memory>

// Third party:
// - Boost:
#include <boost/utility.hpp>
// - Bayeux:
#include <bayeux/datatools/io_factory.h>

namespace snfee {
  namespace io {

    //! \brief Multifile data reader
    class multifile_data_reader
      : private boost::noncopyable
    {
    public:

      /// \brief Configuration data:
      struct config_type
      {
        std::vector<std::string> filenames; ///< Sequence of input filenames
      };
  
      //! Default constructor
      multifile_data_reader(const config_type &);

      //! Destructor
      virtual ~multifile_data_reader();
 
      //! Check if the reader is terminated
      bool is_terminated() const;

      //! Check if the reader has a record tag associated to a next record
      bool has_record_tag() const;

      //! Check if the tag associated to a next record is of a certain type
      bool record_tag_is(const std::string &) const;

      //! Return the current record tag
      std::string get_record_tag() const;

      //! Load an arbitrary serialization records
      template <typename Data>
      void load(Data & data_)
      {
        _reader_().load(data_);
        _at_load_();
        return;
      }
 
      /// Force termination of the reader
      void terminate();

      /// Return the number of loaded records
      std::size_t get_counter() const;
     
    private:

      void _at_load_(); //!< At load action
      
      datatools::data_reader & _reader_(); //!< Return a ref to the current reader
     
    private:

      // Configuration::
      config_type _config_;      ///< Configuration

      // Management:
      bool _terminated_ = false; ///< Forced termination flag
      std::size_t _counter_ = 0; ///< Record counter
      
      struct pimpl_type;
      std::unique_ptr<pimpl_type> _pimpl_; ///< Private working data
      
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_MULTIFILE_DATA_READER_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
