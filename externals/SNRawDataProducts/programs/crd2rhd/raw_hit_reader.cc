#include "raw_hit_reader.h"

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

// This project:

namespace snfee {
  namespace io {

    // static
    const std::size_t raw_hit_reader::HEADER_NBLINES;

    raw_hit_reader::raw_hit_reader() { return; }

    bool
    raw_hit_reader::has_next_hit() const
    {
      DT_THROW_IF(
        !_initialized_, std::logic_error, "Reader is not initialized!");
      if (!*_fin_ || _fin_->eof())
        return false;
      return true;
    }

    bool
    raw_hit_reader::has_run_header() const
    {
      return _header_.get() != nullptr && _header_->is_complete();
    }

    void
    raw_hit_reader::load_run_header(raw_run_header& header_)
    {
      DT_THROW_IF(
        !_initialized_, std::logic_error, "Reader is not initialized!");
      header_ = *_header_.get();
      return;
    }

    raw_record_parser::record_type
    raw_hit_reader::load_next_hit(snfee::data::calo_hit_record& calo_hit_,
                                  snfee::data::tracker_hit_record& tracker_hit_)
    {
      DT_THROW_IF(
        !_initialized_, std::logic_error, "Reader is not initialized!");
      calo_hit_.invalidate();
      tracker_hit_.invalidate();
      raw_record_parser::record_type rec_type =
        _record_parser_->parse(*_fin_, calo_hit_, tracker_hit_);
      if (rec_type == raw_record_parser::RECORD_UNDEF) {
        DT_THROW(std::logic_error, "Parsing failed!");
      }
      *_fin_ >> std::ws;
      return rec_type;
    }

    datatools::logger::priority
    raw_hit_reader::get_logging() const
    {
      return _logging_;
    }

    void
    raw_hit_reader::set_logging(datatools::logger::priority logging_)
    {
      _logging_ = logging_;
      return;
    }

    const raw_hit_reader::config_type&
    raw_hit_reader::get_config() const
    {
      return _config_;
    }

    void
    raw_hit_reader::set_config(const config_type& cfg_)
    {
      DT_THROW_IF(
        _initialized_, std::logic_error, "Reader is already initialized!");
      _config_ = cfg_;
      return;
    }

    bool
    raw_hit_reader::is_initialized() const
    {
      return _initialized_;
    }

    void
    raw_hit_reader::initialize()
    {
      DT_THROW_IF(
        _initialized_, std::logic_error, "Reader is already initialized!");
      _init_input_file_();
      _init_header_();
      _init_parser_();
      *_fin_ >> std::ws;
      _initialized_ = true;
      return;
    }

    void
    raw_hit_reader::reset()
    {
      DT_THROW_IF(
        !_initialized_, std::logic_error, "Reader is not initialized!");
      _initialized_ = false;
      _reset_parser_();
      _reset_header_();
      _reset_input_file_();
      return;
    }

    void
    raw_hit_reader::print(std::ostream& out_) const
    {
      out_ << "Raw hit reader:" << std::endl;
      out_ << "|-- "
           << "Config : " << std::endl;
      out_ << "|   "
           << "|-- "
           << "Input filename : " << _config_.input_filename << std::endl;
      out_ << "|   "
           << "|-- "
           << "Crate number   : " << _config_.crate_num << std::endl;
      out_ << "|   "
           << "|-- "
           << "With calo   : " << std::boolalpha << _config_.with_calo
           << std::endl;
      out_ << "|   "
           << "|-- "
           << "With tracker   : " << std::boolalpha << _config_.with_tracker
           << std::endl;
      out_ << "|   "
           << "`-- "
           << "With calo waveforms : " << std::boolalpha
           << _config_.with_calo_waveforms << std::endl;
      out_ << "`-- "
           << "Initialized  : " << std::boolalpha << _initialized_ << std::endl;
      return;
    }

    void
    raw_hit_reader::_init_input_file_()
    {
      _fin_.reset(new std::ifstream);
      std::string ifn = _config_.input_filename;
      datatools::fetch_path_with_env(ifn);
      _fin_->open(ifn.c_str());
      DT_THROW_IF(!*_fin_,
                  std::runtime_error,
                  "Cannot open input file '" << _config_.input_filename << "'");
      return;
    }

    void
    raw_hit_reader::_reset_input_file_()
    {
      if (_fin_) {
        _fin_->close();
        _fin_.reset();
      }
      return;
    }

    void
    raw_hit_reader::_init_header_()
    {
      _header_.reset(new raw_run_header);
      for (std::size_t ih = 0; ih < HEADER_NBLINES; ih++) {
        std::string hlines;
        std::getline(*_fin_, hlines);
        DT_LOG_DEBUG(_logging_, "Header[#" << ih << "] : " << hlines);
        _decode_header_(hlines, ih);
      }
      return;
    }

    void
    raw_hit_reader::_reset_header_()
    {
      if (_header_) {
        _header_.reset();
      }
      return;
    }

    void
    raw_hit_reader::_init_parser_()
    {
      raw_record_parser::config_type parser_config;
      // parser_config.module_num = _config_.module_num;
      parser_config.crate_num = _config_.crate_num;
      // Extract the firmware version from the run header:
      parser_config.firmware_version = datatools::version_id(
        _header_->software_major_version, _header_->software_minor_version);
      parser_config.with_calo = _config_.with_calo;
      parser_config.with_tracker = _config_.with_tracker;
      parser_config.with_calo_waveforms = _config_.with_calo_waveforms;
      _record_parser_.reset(new raw_record_parser(parser_config, _logging_));

      return;
    }

    void
    raw_hit_reader::_reset_parser_()
    {
      if (_record_parser_) {
        _record_parser_.reset();
      }
      return;
    }

    void
    raw_hit_reader::_decode_header_(const std::string& hline_, const int index_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      namespace qi = boost::spirit::qi;
      std::string::const_iterator str_iter = hline_.begin();
      std::string::const_iterator end_iter = hline_.end();
      bool res = false;

      std::string sw_version;
      double unix_time = datatools::invalid_real();
      std::string date;
      std::string time;
      // std::string data_type;

      if (index_ == 0) {
        std::string data_type;
        res = qi::phrase_parse(
          str_iter,
          end_iter,
          //  Begin grammar
          ("===" >>
           qi::lit("DATA FILE SAVED WITH SN CRATE SOFTWARE VERSION:") >>
           (+~qi::char_("=")) >> "==" >> qi::lit("DATE OF RUN:") >>
           qi::lit("UnixTime =")
           // >> (+~qi::char_("="))
           // >> "="
           >> qi::double_ >> qi::lit("date =") >> (+~qi::char_("t")) >>
           qi::lit("time =") >> (+~qi::char_("=")) >> "==="),
          //  End grammar
          qi::space,
          sw_version,
          unix_time,
          date,
          time);
        DT_THROW_IF(!res || str_iter != end_iter,
                    std::logic_error,
                    "Cannot parse file header line #" << index_);
        DT_LOG_DEBUG(_logging_, "sw_version = " << sw_version);
        DT_LOG_DEBUG(_logging_, "unix_time = " << unix_time);
        DT_LOG_DEBUG(_logging_, "date = " << date);
        DT_LOG_DEBUG(_logging_, "time = " << time);
        _header_->make(sw_version, unix_time, date, time, data_type);
      }

      if (index_ == 2) {
        std::string data_type;
        res = qi::phrase_parse(
          str_iter,
          end_iter,
          //  Begin grammar
          ("===" >> qi::lit("DATA TYPE :") >> (+~qi::char_("=")) >> "==="),
          //  End grammar
          qi::space,
          data_type);
        DT_THROW_IF(!res || str_iter != end_iter,
                    std::logic_error,
                    "Cannot parse file header line #" << index_);
        DT_LOG_DEBUG(_logging_, "data_type = " << data_type);
        _header_->data_type = data_type;
      }

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // namespace io
} // namespace snfee
