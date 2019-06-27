// snfee/io/multifile_data_writer.cc

// Ourselves:
#include <snfee/io/multifile_data_writer.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/io_factory.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

namespace snfee {
  namespace io {

    /// \brief Private implementation
    struct multifile_data_writer::pimpl_type
    {
      pimpl_type(multifile_data_writer & master_)
        : master(master_)
      {
        return;
      }
      multifile_data_writer & master;
      std::unique_ptr<datatools::data_writer> writer;
      int         current_file_index = -1;
      std::size_t _nrecords_in_file_ = 0;
      //std::string record_tag;
      void _next_writer_();
      void _destroy_writer_();
    };

    std::size_t multifile_data_writer::get_file_counter() const
    {
      return _pimpl_->current_file_index;
    }

    bool multifile_data_writer::is_last_file() const
    {
      if ((_pimpl_->current_file_index + 1) == (int) _config_.filenames.size()) return true;
      return false;
    }

    void multifile_data_writer::add_filename(const std::string & filename_)
    {
      _config_.filenames.push_back(filename_);
      return;
    }

    multifile_data_writer::multifile_data_writer(const config_type & cfg_,
                                                 const datatools::logger::priority logging_)
      : _logging_(logging_)
      , _config_(cfg_)
    {
      _pimpl_.reset(new pimpl_type(*this));
      DT_THROW_IF(_config_.filenames.size() == 0,
                  std::logic_error,
                  "Missing output filenames for the multiple data writer!");
      _pimpl_->_next_writer_();
      return;
    }

    // virtual
    multifile_data_writer::~multifile_data_writer()
    {
      if (_pimpl_) {
        _pimpl_->_destroy_writer_();
        _pimpl_.reset();
      }
      return;
    }

    void multifile_data_writer::pimpl_type::_destroy_writer_()
    {
      if (writer) {
        writer.reset();
      }
      return;
    }

    void multifile_data_writer::pimpl_type::_next_writer_()
    {
      DT_LOG_TRACE_ENTERING(master._logging_);
      _destroy_writer_();
      DT_LOG_DEBUG(master._logging_, "Current file index        = " << current_file_index);
      DT_LOG_DEBUG(master._logging_, "Number of available files = " << master._config_.filenames.size());
      if ((current_file_index + 1) == (int) master._config_.filenames.size()) {
        DT_LOG_DEBUG(master._logging_, "Detected overrun!");
        // Detect an overrun:
        if (master._config_.terminate_on_overrun) {
          master.terminate();
        } else {
          DT_THROW(std::logic_error,
                   "Overrunning multiple data writer has no more output file!");
        }
      }
      if (!master._terminated_) {
        current_file_index++;
        DT_LOG_DEBUG(master._logging_, "Open a new file with index = " << current_file_index);
        std::string out_filename = master._config_.filenames[current_file_index];
        datatools::fetch_path_with_env(out_filename);
        writer.reset(new datatools::data_writer(out_filename,
                                                datatools::using_multi_archives));
        DT_THROW_IF(!writer->is_initialized(),
                    std::logic_error,
                    "Multiple data writer is not initialized from '"
                    << out_filename << "'!");
      }
      DT_LOG_TRACE_EXITING(master._logging_);
      return;
    }

    void multifile_data_writer::terminate()
    {
      _terminated_ = true;
      return;
    }

    std::size_t multifile_data_writer::get_counter() const
    {
      return _counter_;
    }

    bool multifile_data_writer::is_terminated() const
    {
      if (_terminated_) return true;
      if (_pimpl_->current_file_index >= (int) _config_.filenames.size()) {
        return true;
      }
      return false;
    }

    datatools::data_writer & multifile_data_writer::_writer_()
    {
      DT_THROW_IF(is_terminated(), std::logic_error, "No writer!");
      return *_pimpl_->writer;
    }

    void multifile_data_writer::_pre_store_()
    {
      DT_LOG_DEBUG(_logging_, "Number of records = " << _counter_);
      DT_THROW_IF(is_terminated(), std::logic_error, "Multifile writer is terminated!");
      DT_LOG_DEBUG(_logging_, "Number of records stored in the current output file = "
                   << _pimpl_->_nrecords_in_file_);
      DT_LOG_DEBUG(_logging_, "Max. number of records per output file = " << _config_.max_records_per_file);
      if (_config_.max_records_per_file > 0 and _pimpl_->_nrecords_in_file_ == _config_.max_records_per_file) {
        DT_LOG_DEBUG(_logging_, "Request to open a new output file");
        _pimpl_->_next_writer_();
        _pimpl_->_nrecords_in_file_ = 0;
      }
      return;
    }

    void multifile_data_writer::_post_store_()
    {
      _counter_++;
      _pimpl_->_nrecords_in_file_++;
      if (_config_.max_total_records > 0) {
        if (_counter_ == _config_.max_total_records) {
          terminate();
        }
      }
      return;
    }

  } // namespace io
} // namespace snfee
