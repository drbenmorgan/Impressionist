// snfee/io/raw_record_parser.cc

// Ourselves:
#include <snfee/io/raw_record_parser.h>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/phoenix/phoenix.hpp>
// #include <boost/fusion/include/at_c.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

namespace snfee {
  namespace io {

    // static
    const std::size_t raw_record_parser::NB_HIT_HEADER_LINES;

    raw_record_parser::raw_record_parser(const config_type & cfg_,
                                         const datatools::logger::priority l_)
    {
      set_logging(l_);
      set_config(cfg_);
      _record_type_ = RECORD_UNDEF;
      return;
    }
    
    datatools::logger::priority raw_record_parser::get_logging() const
    {
      return _logging_;
    }

    void raw_record_parser::set_logging(const datatools::logger::priority l_)
    {
      _logging_ = l_;
      return;
    }
  
    const raw_record_parser::config_type & raw_record_parser::get_config() const
    {
      return _config_;
    }
    
    void raw_record_parser::set_config(const config_type & cfg_)
    {
      _config_ = cfg_;

      DT_THROW_IF(! _config_.with_calo and ! _config_.with_tracker,
                  std::logic_error,
                  "At least one type of raw records must be allowed!");

      if (_config_.with_calo) {
        calo_hit_parser::config_type caloCfg;
        // caloCfg.module_num = _config_.module_num;
        caloCfg.crate_num = _config_.crate_num;
        caloCfg.firmware_version = _config_.firmware_version;
        caloCfg.with_waveforms = _config_.with_calo_waveforms;
        _calo_hit_parser_.reset(new calo_hit_parser(caloCfg, _logging_));
        if (datatools::logger::is_debug(_logging_)) {
          DT_LOG_DEBUG(_logging_, "Calo hit parser: ");
          _calo_hit_parser_->print(std::clog, "[debug] ");
        }
      }
 
      if (_config_.with_tracker) {
        tracker_hit_parser::config_type trackerCfg;
        // trackerCfg.module_num = _config_.module_num;
        trackerCfg.crate_num = _config_.crate_num;
        trackerCfg.firmware_version = _config_.firmware_version;
        _tracker_hit_parser_.reset(new tracker_hit_parser(trackerCfg, _logging_));
        // if (datatools::logger::is_debug(_logging_)) {
        //   DT_LOG_DEBUG(_logging_, "Tracker hit parser: ");
        //   _tracker_hit_parser_->print(std::clog, "[debug] ");
        // }
      }
      return;
    }
    
    raw_record_parser::record_type raw_record_parser::parse(std::istream & in_,
                                                            snfee::data::calo_hit_record & calo_hit_,
                                                            snfee::data::tracker_hit_record & tracker_hit_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      // bool success = false;
      record_type ret = RECORD_UNDEF;
      try {

        // Header:
        for (std::size_t ih = 0; ih < NB_HIT_HEADER_LINES; ih++) {
          std::string hline;
          std::getline(in_, hline);
          DT_LOG_DEBUG(_logging_, "Parsing header line number " << ih << " : {" << hline << "}");
          _parse_hit_header_(hline, ih);
          in_ >> std::ws;
        }

        DT_LOG_DEBUG(_logging_, "Current raw record type after header parsing = " << _record_type_ );
        DT_LOG_DEBUG(_logging_, "Current raw hit ID after header parsing      = " << _hit_id_);
        DT_LOG_DEBUG(_logging_, "Current raw trigger ID after header parsing  = " << _trigger_id_);

        // Calo or tracker parser:
        if (_record_type_ == RECORD_CALO) {
          DT_THROW_IF(!_config_.with_calo, std::logic_error,
                      "Unexpected calo hit record!");
          calo_hit_.set_hit_num(_hit_id_);
          calo_hit_.set_trigger_id(_trigger_id_);
          bool success = _calo_hit_parser_->parse(in_, calo_hit_);
          DT_THROW_IF(!success, std::logic_error, "Failed to parse a calo hit!");
          ret = _record_type_;
          DT_LOG_DEBUG(_logging_, "Parsed a calo hit record");
          // // XXX
          // {
          //   boost::property_tree::ptree options;
          //   options.put("title", "Calo hit record: ");
          //   options.put("indent", "[devel] ");
          //   options.put("with_waveform_samples", true);
          //   calo_hit_.print_tree(std::cerr, options);
          // }
          // // XXX
        } else if (_record_type_ == RECORD_TRACKER){
          DT_THROW_IF(!_config_.with_tracker, std::logic_error,
                      "Unexpected tracker hit record!");
          tracker_hit_.set_hit_num(_hit_id_);
          tracker_hit_.set_trigger_id(_trigger_id_);
          bool success = _tracker_hit_parser_->parse(in_, tracker_hit_);
          DT_THROW_IF(!success, std::logic_error, "Failed to parse a tracker hit!");
          ret = _record_type_;
          DT_LOG_DEBUG(_logging_, "Parsed a tracker hit record");
        } 
        in_ >> std::ws;
        // success = true;
      } catch (std::exception & error) {
        DT_LOG_ERROR(_logging_, error.what());
        // success = false;
        ret = RECORD_UNDEF;
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return ret;
    }

    void raw_record_parser::_parse_hit_header_(const std::string & header_line_,
                                               const int index_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      namespace qi = boost::spirit::qi;
      bool res = false;
      std::string hit_type;

      if (index_ == 0) {
        std::string::const_iterator str_iter = header_line_.begin();
        std::string::const_iterator end_iter = header_line_.end();
        res = qi::phrase_parse(str_iter,
                               end_iter,
                               //  Begin grammar
                               (
                                qi::lit("= HIT")
                                >> qi::ulong_long
                                >> "="
                                >> (+~qi::char_("="))
                                >> "="
                                >> qi::lit("TRIG_ID")
                                >> qi::ulong_long
                                >> "="
                                ),
                               //  End grammar
                               qi::space,
                               _hit_id_, hit_type, _trigger_id_);
        DT_THROW_IF(!res || str_iter != end_iter,
                    std::logic_error,
                    "Cannot parse file header line #" << index_);
        DT_LOG_DEBUG(_logging_, "_hit_id_ = " << _hit_id_);
        DT_LOG_DEBUG(_logging_, "hit_type = " << hit_type);
        DT_LOG_DEBUG(_logging_, "_trigger_id_ = " << _trigger_id_);
        DT_THROW_IF(hit_type != "CALO" && hit_type != "TRACKER", std::logic_error, "Invalid hit type label '" << hit_type << "'!");

        if (hit_type == "CALO") {
          _record_type_ = RECORD_CALO;
        } else if (hit_type == "TRACKER") {
          _record_type_ = RECORD_TRACKER;
        } else {
          DT_THROW(std::logic_error, "Unknow raw record type '" << hit_type << "'!");
        }
      }

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // namespace io
} // namespace snfee

