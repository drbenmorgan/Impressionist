// snfee/io/multifile_data_reader.cc

// Ourselves:
#include <snfee/io/multifile_data_reader.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/io_factory.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

namespace snfee {
  namespace io {

    /// \brief Private implementation
    struct multifile_data_reader::pimpl_type
    {
      pimpl_type(multifile_data_reader & master_)
        : master(master_)
      {
        return;
      }
      multifile_data_reader & master;
      std::unique_ptr<datatools::data_reader> reader;
      int  _current_file_index_ = -1;
      //std::string record_tag;
      void _next_reader_();
      void _destroy_reader_();
    };

    multifile_data_reader::multifile_data_reader(const config_type & cfg_)
      : _config_(cfg_)
    {
      _pimpl_.reset(new pimpl_type(*this));
      DT_THROW_IF(_config_.filenames.size() == 0,
                  std::logic_error,
                  "Missing input filenames for the multiple data reader!");
      _pimpl_->_next_reader_();
      return;
    }

    // virtual
    multifile_data_reader::~multifile_data_reader()
    {
      if (_pimpl_) {
        _pimpl_->_destroy_reader_();
        _pimpl_.reset();
      }
      return;
    }

    void multifile_data_reader::pimpl_type::_destroy_reader_()
    {
      if (reader) {
        reader.reset();
      }
      return;
    }

    void multifile_data_reader::pimpl_type::_next_reader_()
    {
      _destroy_reader_();
      _current_file_index_++;
      DT_THROW_IF(_current_file_index_ == (int) master._config_.filenames.size(),
                   std::logic_error,
                  "Multiple data reader has no more input file!");
      std::string in_filename = master._config_.filenames[_current_file_index_];
      datatools::fetch_path_with_env(in_filename);
      reader.reset(new datatools::data_reader(in_filename, datatools::using_multi_archives));
      DT_THROW_IF(!reader->is_initialized(),
                  std::logic_error,
                  "Multiple data reader is not initialized from '"
                  << in_filename << "'!");
      DT_THROW_IF(reader->is_error(),
                  std::logic_error,
                  "Multiple data reader error!");   
      return;
    }

    void multifile_data_reader::terminate()
    {
      _terminated_ = true;
      return;
    }

    bool multifile_data_reader::has_record_tag() const
    {
      if (is_terminated()) {
        return false;
      }
      bool found_tag = false;
      while (!found_tag) {
        if (_pimpl_->reader->has_record_tag()) {
          found_tag = true;
          // record_tag = _pimpl_->reader->get_record_tag();
          break;
        } else {
          if ((_pimpl_->_current_file_index_ + 1) == (int) _config_.filenames.size()) {
            // No more input file:
            break;
          } else {
            multifile_data_reader * mutable_this = const_cast<multifile_data_reader *>(this);
            mutable_this->_pimpl_->_next_reader_();
          }
        }
      }
      return found_tag;
    }

    bool multifile_data_reader::record_tag_is(const std::string & tag_) const
    {
      if (is_terminated()) {
        return false;
      }
      if (_pimpl_->reader) {
        return _pimpl_->reader->record_tag_is(tag_);
      }
      return false;
      // multifile_data_reader * mutable_this = const_cast<multifile_data_reader *>(this);
      // return mutable_this->_reader_().record_tag_is(tag_);
    }

    std::string multifile_data_reader::get_record_tag() const
    {
      if (_pimpl_->reader) {
        return _pimpl_->reader->get_record_tag();
      }
      return "";
    }

    std::size_t multifile_data_reader::get_counter() const
    {
      return _counter_;
    }

    bool multifile_data_reader::is_terminated() const
    {
      if (_terminated_) return true;
      if ((_pimpl_->_current_file_index_) >= (int)_config_.filenames.size()) {
        return true;
      }
      return false;
    }

    datatools::data_reader & multifile_data_reader::_reader_()
    {
      DT_THROW_IF(is_terminated(), std::logic_error, "No reader!");
      return *_pimpl_->reader;
    }

    void multifile_data_reader::_at_load_()
    {
      _counter_++;
      return;
    }

  } // namespace io
} // namespace snfee
