// snfee/io/tracker_hit_parser.cc
// Ourselves:
#include <snfee/io/tracker_hit_parser.h>

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

// This project:
#include <snfee/model/feb_constants.h>

namespace snfee {
  namespace io {

    tracker_hit_parser::tracker_hit_parser(const config_type & cfg_,
                                           const datatools::logger::priority p_)
    {
      set_logging(p_);
      set_config(cfg_);
      return;
    }

    datatools::logger::priority tracker_hit_parser::get_logging() const
    {
      return _logging_;
    }

    void tracker_hit_parser::set_logging(const datatools::logger::priority l_)
    {
      _logging_ = l_;
      return;
    }
    
    const tracker_hit_parser::config_type & tracker_hit_parser::get_config() const
    {
      return _config_;
    }
   
    void tracker_hit_parser::set_config(const config_type & cfg_)
    {
      _config_ = cfg_;
      static const datatools::version_id version_2_4(2, 4);
      if (_config_.firmware_version.compare(version_2_4) >= 0) {
        _format_ = FORMAT_FROM_2_4;
      } else {
        _format_ = FORMAT_BEFORE_2_4;
      }
      return;
    }

    bool tracker_hit_parser::parse(std::istream & in_, snfee::data::tracker_hit_record & hit_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      bool success = false;
      try {
        // Backup hit number and trigger ID from the hit:
        int32_t hit_num = hit_.get_hit_num();
        int32_t trigger_id = hit_.get_trigger_id();
        hit_.invalidate();
        hit_.set_hit_num(hit_num);
        hit_.set_trigger_id(trigger_id);
        // No header for tracker channel hit
        // // Header:
        // for (std::size_t ih = 0; ih < NB_TRACKER_HEADER_LINES; ih++) {
        //   std::string hline;
        //   std::getline(in_, hline);
        //   DT_LOG_DEBUG(_logging_, "Parsing header line '" << hline << "'");
        //   _parse_header_(hline, ih, hit_);
        //   in_ >> std::ws;
        // }
        // Data:
        std::string data_line;
        std::getline(in_, data_line);
        DT_LOG_DEBUG(_logging_, "Parsing data line '" << data_line << "'");
        hit_data_type hit_data;
        _parse_timestamp_(data_line, hit_data);
        in_ >> std::ws;

        // Populate the tracker hit record:
        
        // int16_t   module_num  = _config_.module_num;
        int16_t   crate_num   = _config_.crate_num;
        int16_t   board_num   = hit_data.slot_id;
        int16_t   chip_num    = hit_data.feast_id;
        int16_t   channel_num = hit_data.channel_id;
        snfee::data::tracker_hit_record::channel_category_type chCat
          = snfee::data::tracker_hit_record::CHANNEL_UNDEF;
        if (hit_data.channel_type == "AN") {
          chCat = snfee::data::tracker_hit_record::CHANNEL_ANODE;
        } else if (hit_data.channel_type == "CA") {
          chCat = snfee::data::tracker_hit_record::CHANNEL_CATHODE;
        }
        snfee::data::tracker_hit_record::timestamp_category_type tsCat
          = snfee::data::tracker_hit_record::TIMESTAMP_UNDEF;
        if (hit_data.timestamp_type == "R0") {
          tsCat = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R0;
        } else if (hit_data.timestamp_type == "R1") {
          tsCat = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R1;
        } else if (hit_data.timestamp_type == "R2") {
          tsCat = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R2;
        } else if (hit_data.timestamp_type == "R3") {
          tsCat = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R3;
        } else if (hit_data.timestamp_type == "R4") {
          tsCat = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R4;
        } else if (hit_data.timestamp_type == "R5") {
          tsCat = snfee::data::tracker_hit_record::TIMESTAMP_CATHODE_R5;
        } else if (hit_data.timestamp_type == "R6") {
          tsCat = snfee::data::tracker_hit_record::TIMESTAMP_CATHODE_R6;
        }
        DT_LOG_DEBUG(_logging_, "Timestamp type label = '" << hit_data.timestamp_type << "'");
        DT_LOG_DEBUG(_logging_, "Timestamp type       = [" << tsCat << "]");
        DT_LOG_DEBUG(_logging_, "Crate number         = [" << crate_num << "]");
        uint64_t timestamp = hit_data.timestamp_value;
        hit_.make(hit_num,
                  trigger_id,
                  // module_num,
                  crate_num,
                  board_num,
                  chip_num,
                  channel_num,
                  chCat,
                  tsCat,
                  timestamp);
        success = true;
      } catch (std::exception & error) {
        DT_LOG_ERROR(_logging_, error.what());
        success = false;
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return success;
    }

    void tracker_hit_parser::_parse_timestamp_(const std::string & data_line_,
                                               hit_data_type & hit_data_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      std::string data_line = data_line_;
      namespace qi = boost::spirit::qi;
      bool res = false;
      // We use a trick because of nasty syntax from the DAQ ascii output:
      // boost::replace_all(data_line, "Slot", "Slot ");
      // boost::replace_all(data_line, " Feast", " Feast ");
      // boost::replace_all(data_line, " Ch", " Ch ");
      boost::replace_all(data_line, " CA R0 ", " CA R5 ");
      DT_LOG_DEBUG(_logging_, "data_line before parsing = " << data_line);

      std::string::const_iterator str_iter = data_line.begin();
      std::string::const_iterator end_iter = data_line.end();
 
      if (_format_ == FORMAT_FROM_2_4) {
        res = qi::phrase_parse(str_iter,
                               end_iter,
                               //  Begin grammar
                               (
                                qi::lit("Slot")     >> qi::uint_[boost::phoenix::ref(hit_data_.slot_id) = boost::spirit::qi::_1]
                                >> qi::lit("Feast") >> qi::uint_[boost::phoenix::ref(hit_data_.feast_id) = boost::spirit::qi::_1]
                                >> qi::lit("Ch")    >> qi::uint_[boost::phoenix::ref(hit_data_.channel_id) = boost::spirit::qi::_1]
                                >> (qi::string("AN") |
                                    qi::string("CA"))[boost::phoenix::ref(hit_data_.channel_type) = boost::spirit::qi::_1]
                                >> (qi::string("R0") |
                                    qi::string("R1") |
                                    qi::string("R2") |
                                    qi::string("R3") |
                                    qi::string("R4") |
                                    qi::string("R5") |
                                    qi::string("R6"))[boost::phoenix::ref(hit_data_.timestamp_type) = boost::spirit::qi::_1]
                                >> qi::ulong_long[boost::phoenix::ref(hit_data_.timestamp_value) = boost::spirit::qi::_1]
                                >> qi::double_[boost::phoenix::ref(hit_data_.timestamp_ns) = boost::spirit::qi::_1]
                                >> qi::lit("UnixTime")      >> qi::double_[boost::phoenix::ref(hit_data_.unix_time) = boost::spirit::qi::_1]
                                ),
                               //  End grammar
                               qi::space);
      } else {
          res = qi::phrase_parse(str_iter,
                                 end_iter,
                                 //  Begin grammar
                                 (
                                  qi::lit("Slot")     >> qi::uint_[boost::phoenix::ref(hit_data_.slot_id) = boost::spirit::qi::_1]
                                  >> qi::lit("Feast") >> qi::uint_[boost::phoenix::ref(hit_data_.feast_id) = boost::spirit::qi::_1]
                                  >> qi::lit("Ch")    >> qi::uint_[boost::phoenix::ref(hit_data_.channel_id) = boost::spirit::qi::_1]
                                  >> (qi::string("AN") |
                                      qi::string("CA"))[boost::phoenix::ref(hit_data_.channel_type) = boost::spirit::qi::_1]
                                  >> (qi::string("R0") |
                                      qi::string("R1") |
                                      qi::string("R2") |
                                      qi::string("R3") |
                                      qi::string("R4") |
                                      qi::string("R5") |
                                      qi::string("R6"))[boost::phoenix::ref(hit_data_.timestamp_type) = boost::spirit::qi::_1]
                                  >> qi::ulong_long[boost::phoenix::ref(hit_data_.timestamp_value) = boost::spirit::qi::_1]
                                  >> qi::double_[boost::phoenix::ref(hit_data_.timestamp_ns) = boost::spirit::qi::_1]
                                  ),
                                 //  End grammar
                                 qi::space);
        }
      DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                  "Cannot parse file timestamp : " << data_line << "; failed at '" << *str_iter << "'!");
      DT_LOG_DEBUG(_logging_, "slot_id         = " << hit_data_.slot_id);
      DT_LOG_DEBUG(_logging_, "feast_id        = " << hit_data_.feast_id);
      DT_LOG_DEBUG(_logging_, "channel_id      = " << hit_data_.channel_id);
      DT_LOG_DEBUG(_logging_, "channel_type    = " << hit_data_.channel_type);
      DT_LOG_DEBUG(_logging_, "timestamp_type  = " << hit_data_.timestamp_type);
      DT_LOG_DEBUG(_logging_, "timestamp_value = " << hit_data_.timestamp_value);
      DT_LOG_DEBUG(_logging_, "timestamp_ns    = " << hit_data_.timestamp_ns);
      DT_LOG_DEBUG(_logging_, "unixtime        = " << hit_data_.unix_time);

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // namespace io
} // namespace snfee

