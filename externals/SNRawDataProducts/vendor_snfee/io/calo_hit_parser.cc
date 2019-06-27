// snfee/io/calo_hit_parser.cc

// Ourselves:
#include <snfee/io/calo_hit_parser.h>

// Standard library:
// #include <limits>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
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

    // static
    const std::size_t calo_hit_parser::NB_CALO_HEADER_LINES;
    const std::size_t calo_hit_parser::NB_CALO_CHANNELS;

    void calo_hit_parser::header_type::print(std::ostream & out_, const std::string & indent_) const
    {
      out_ << indent_ << "slot_id         = " << slot_id << std::endl;
      out_ << indent_ << "channel_id      = " << channel_id << std::endl;
      out_ << indent_ << "lto_flag        = " << lto_flag << std::endl;
      out_ << indent_ << "ht_flag         = " << ht_flag << std::endl;
      out_ << indent_ << "event_id        = " << event_id << std::endl;
      out_ << indent_ << "raw_tdc         = " << raw_tdc << std::endl;
      out_ << indent_ << "raw_tdc_ns      = " << raw_tdc_ns << std::endl;
      out_ << indent_ << "lt_trig_count   = " << lt_trig_count << std::endl;
      out_ << indent_ << "lt_time_count   = " << lt_time_count << std::endl;
      out_ << indent_ << "raw_baseline    = " << raw_baseline << std::endl;
      out_ << indent_ << "baseline_volt   = " << baseline_volt << std::endl;
      out_ << indent_ << "raw_peak        = " << raw_peak << std::endl;
      out_ << indent_ << "peak_volt       = " << peak_volt << std::endl;
      out_ << indent_ << "peak_cell       = " << peak_cell << std::endl;
      out_ << indent_ << "raw_charge      = " << raw_charge << std::endl;
      out_ << indent_ << "charge_picocoulomb = " << charge_picocoulomb << std::endl;
      out_ << indent_ << "charge_overflow = " << charge_overflow << std::endl;
      out_ << indent_ << "rising_cell     = " << rising_cell << std::endl;
      out_ << indent_ << "rising_offset   = " << rising_offset << std::endl;
      out_ << indent_ << "rising_ns       = " << rising_ns << std::endl;
      out_ << indent_ << "falling_cell    = " << falling_cell << std::endl;
      out_ << indent_ << "falling_offset  = " << falling_offset << std::endl;
      out_ << indent_ << "falling_ns      = " << falling_ns << std::endl;
      return;
    } 

    void calo_hit_parser::print(std::ostream & out_, const std::string & indent_) const
    {
      out_ << indent_ << "|-- Logging: " << datatools::logger::get_priority_label(_logging_) << std::endl;
      out_ << indent_ << "|-- Config: " << std::endl;
      out_ << indent_ << "|   |-- Crate num      : " << _config_.crate_num << std::endl;
      out_ << indent_ << "|   |-- Firmware ver.  : " << _config_.firmware_version.to_string() << std::endl;
      out_ << indent_ << "|   `-- With waveforms : " << std::boolalpha << _config_.with_waveforms << std::endl;
      out_ << indent_ << "`-- Format         : " << _format_ << std::endl;
      return;
    }

    calo_hit_parser::calo_hit_parser(const config_type & cfg_,
                                     const datatools::logger::priority logging_)
    {
      set_logging(logging_);
      set_config(cfg_);
      return;
    }

    datatools::logger::priority calo_hit_parser::get_logging() const
    {
      return _logging_;
    }

    void calo_hit_parser::set_logging(const datatools::logger::priority l_)
    {
      _logging_ = l_;
      return;
    }
    
    const calo_hit_parser::config_type & calo_hit_parser::get_config() const
    {
      return _config_;
    }
    
    void calo_hit_parser::set_config(const config_type & cfg_)
    {
      _config_ = cfg_;
      static const datatools::version_id version_2_3(2, 3);
      static const datatools::version_id version_2_4(2, 4);
      if (_config_.firmware_version.compare(version_2_4) >= 0) {
        _format_ = FORMAT_FROM_2_4;
      } else if (_config_.firmware_version.compare(version_2_3) >= 0) {
        _format_ = FORMAT_FROM_2_3;
      } else {
        _format_ = FORMAT_BEFORE_2_3;
      }
      return;
    }

    bool calo_hit_parser::parse(std::istream & in_, snfee::data::calo_hit_record & hit_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      bool success = false;
      namespace qi = boost::spirit::qi;
      try {
        // Backup hit number and trigger ID from the hit:
        int32_t hit_num = hit_.get_hit_num();
        int32_t trigger_id = hit_.get_trigger_id();
        hit_.invalidate();
        hit_.set_hit_num(hit_num);
        hit_.set_trigger_id(trigger_id);
        header_type headers[2];
        // Loop on both channels:
        for (int ichannel = 0;
             ichannel < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS;
             ichannel++) {      
          // Header line(s):
          for (std::size_t ihline = 0; ihline < NB_CALO_HEADER_LINES; ihline++) {
            std::string hline;
            std::getline(in_, hline);
            DT_LOG_DEBUG(_logging_, "Calo hit parsing header line number " << ihline << " : '" << hline << "'");
            _parse_header_(hline, ihline, headers[ichannel]);
            in_ >> std::ws;
          }
          if (_config_.with_waveforms) {
            // Waveforms:
            snfee::data::calo_hit_record::waveforms_record & waveforms
              = const_cast<snfee::data::calo_hit_record::waveforms_record &>(hit_.get_waveforms());
            std::string raw_waveform_data_line;
            std::getline(in_, raw_waveform_data_line);
            DT_LOG_DEBUG(_logging_, "Parsing raw waveform data line : '" << raw_waveform_data_line << "'");
            _parse_waveform_(raw_waveform_data_line, ichannel, waveforms);
            in_ >> std::ws;
            DT_LOG_DEBUG(_logging_, "Raw waveforms size             : " << waveforms.get_samples().size());
            /// >>> XXX
            // std::cerr << "==============================================" << std::endl;
            // DT_LOG_DEBUG(_logging_, "[DEVEL] >>>>> TEST 0");
            // std::cerr << "[DEVEL] Waveforms @ " << &hit_.get_waveforms() << std::endl;
            // std::cerr << "[DEVEL] Channel 0 samples : " << std::endl;
            // for (int isample = 0; isample < 8; isample++) {
            //   std::cerr  << hit_.get_waveforms().get_adc(isample, 0) << ' ';
            // }
            // std::cerr << "..." <<  std::endl;
            // std::cerr << "[DEVEL] Channel 1 samples : " << std::endl;
            // for (int isample = 0; isample < 8; isample++) {
            //   std::cerr  << hit_.get_waveforms().get_adc(isample, 1) << ' ';
            // }
            // std::cerr << "..." <<  std::endl;
            // std::cerr << "==============================================" << std::endl;
            /// <<< XXX
          }
          if (ichannel == 0) {
            // Parse intermediate line between 2 associated calorimeter channel hits (same SAMLONG):
            std::string hitline;
            std::getline(in_, hitline);
            DT_LOG_DEBUG(_logging_, "hitline = '" << hitline << "'");
            std::string::const_iterator str_iter = hitline.begin();
            std::string::const_iterator end_iter = hitline.end();
            int32_t next_hit_number;
            int32_t next_trigger_id;
            bool res = false;
            res = qi::phrase_parse(str_iter,
                                   end_iter,
                                   //  Begin grammar
                                   (
                                    qi::lit("=")
                                    >> qi::lit("HIT")
                                    >> qi::int_[boost::phoenix::ref(next_hit_number) = boost::spirit::qi::_1]
                                    >> qi::lit("=")
                                    >> qi::lit("CALO")
                                    >> qi::lit("=")
                                    >> qi::lit("TRIG_ID")
                                    >> qi::int_[boost::phoenix::ref(next_trigger_id) = boost::spirit::qi::_1]
                                    >> qi::lit("=")
                                    ),
                                   //  End grammar
                                   qi::space);
            DT_THROW_IF(!res || str_iter != end_iter,
                        std::logic_error,
                        "Cannot parse file calo intermediate hit line; failed at '" << *str_iter << "'!");
            DT_THROW_IF(next_hit_number != hit_.get_hit_num() + 1, std::logic_error,
                        "Hit numbers (" << next_hit_number << " vs " << hit_.get_hit_num() << " do not match (ch0 vs ch1)!");
            DT_THROW_IF(next_trigger_id != hit_.get_trigger_id(), std::logic_error,
                        "Trigger IDs do not match (ch0 vs ch1)!");
          } // End of parse intermediate
        } // End of channel loop
        // Checks:
        DT_THROW_IF(headers[0].slot_id != headers[1].slot_id,
                    std::logic_error,
                    "Board slot IDs do not match (ch0 vs ch1)!");
        DT_THROW_IF(headers[1].channel_id != headers[0].channel_id + 1,
                    std::logic_error,
                    "Board channel IDs do not pair (ch0 vs ch1)!");
        DT_THROW_IF(headers[0].event_id != headers[1].event_id,
                    std::logic_error,
                    "Event IDs do not pair (ch0 vs ch1)!");
        DT_THROW_IF(headers[0].raw_tdc != headers[1].raw_tdc,
                    std::logic_error,
                    "Raw TDCs do not pair (ch0 vs ch1)!");
        // DT_THROW_IF(headers[0].lt_time_count != headers[1].lt_time_count,
        //             std::logic_error,
        //             "Time counts do not pair (ch0 vs ch1)!");
        // DT_THROW_IF(headers[0].lt_trig_count != 0 and headers[1].lt_trig_count != 1,
        //             std::logic_error,
        //             "Trig counts do not pair (ch0 vs ch1)!");
        DT_THROW_IF(headers[0].fcr != headers[1].fcr,
                    std::logic_error,
                    "FCRs do not pair (ch0 vs ch1)!");
        // Populate the calorimeter hit record:
        uint64_t  tdc = headers[0].raw_tdc;
        int16_t   crate_num =  _config_.crate_num;
        int16_t   board_num = headers[0].slot_id;
        int16_t   chip_num = headers[0].channel_id / 2;
        // uint16_t  l2_id = headers[0].l2_id;
        uint16_t  event_id = trigger_id % 0xFF;
        uint16_t  l2_id = trigger_id % 0x1F;
        uint16_t  fcr = headers[0].fcr;
        if (fcr >= 1024) {
          DT_LOG_WARNING(_logging_, "FCR=[" << fcr << "] is out of range.");
          fcr = fcr % 1024;
        } 
        bool      has_waveforms = _config_.with_waveforms;
        DT_LOG_DEBUG(_logging_, "has_waveforms = " << std::boolalpha << has_waveforms);
        uint16_t  waveform_start_sample = snfee::data::calo_hit_record::INVALID_WAVEFORM_START_SAMPLE;
        uint16_t  waveform_number_of_samples = snfee::data::calo_hit_record::INVALID_WAVEFORM_NUMBER_OF_SAMPLES;
        if (has_waveforms) {
          waveform_start_sample = 0;
          DT_LOG_DEBUG(_logging_,
                       "# samples      = " << hit_.get_waveforms().get_samples().size());
          waveform_number_of_samples = hit_.get_waveforms().get_samples().size();
        }
        DT_LOG_DEBUG(_logging_, "waveform_start_sample      = " << waveform_start_sample);
        DT_LOG_DEBUG(_logging_, "waveform_number_of_samples = " << waveform_number_of_samples);
        hit_.make(hit_num,
                  trigger_id,
                  tdc,
                  crate_num,
                  board_num,
                  chip_num,
                  event_id,
                  l2_id,
                  fcr,
                  has_waveforms,
                  waveform_start_sample,
                  waveform_number_of_samples,
                  true);
        /// >>> XXX
        // std::cerr << "==============================================" << std::endl;
        // DT_LOG_DEBUG(_logging_, "[DEVEL] >>>>> TEST 1");
        // std::cerr << "[DEVEL] Waveforms @ " << &hit_.get_waveforms() << std::endl;
        // std::cerr << "[DEVEL] Channel 0 samples : " << std::endl;
        // for (int isample = 0; isample < 8; isample++) {
        //   std::cerr  << hit_.get_waveforms().get_adc(isample, 0) << ' ';
        // }
        // std::cerr << "..." <<  std::endl;
        // std::cerr << "[DEVEL] Channel 1 samples : " << std::endl;
        // for (int isample = 0; isample < 8; isample++) {
        //   std::cerr  << hit_.get_waveforms().get_adc(isample, 1) << ' ';
        // }
        // std::cerr << "..." <<  std::endl;
        // std::cerr << "==============================================" << std::endl;
        /// <<< XXX
        // Loop on both channels:
        for (int ichannel = 0;
             ichannel < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS;
             ichannel++) {
          bool    lto       = headers[ichannel].lto_flag;
          bool    ht        = headers[ichannel].ht_flag;
          bool    lt        = (ht || lto);
          bool    underflow = false;
          bool    overflow  = headers[ichannel].charge_overflow;
          int32_t baseline  = headers[ichannel].raw_baseline;
          int16_t peak      = headers[ichannel].raw_peak;
          int16_t peak_cell = headers[ichannel].peak_cell;
          int16_t charge    = headers[ichannel].raw_charge;
          int32_t rising_cell  = headers[ichannel].rising_cell * 256 + headers[ichannel].rising_offset; 
          int32_t falling_cell = headers[ichannel].falling_cell * 256 + headers[ichannel].falling_offset; 
          hit_.make_channel(ichannel,
                            lt,
                            ht,
                            underflow,
                            overflow,
                            baseline,
                            peak,
                            peak_cell,
                            charge,
                            rising_cell,
                            falling_cell);
        }
        // /// >>> XXX
        // std::cerr << "==============================================" << std::endl;
        // std::cerr << "[DEVEL] Waveforms @ " << &hit_.get_waveforms() << std::endl;
        // std::cerr << "[DEVEL] Channel 0 samples : " << std::endl;
        // for (int isample = 0; isample < 8; isample++) {
        //   std::cerr  << hit_.get_waveforms().get_adc(isample, 0) << ' ';
        // }
        // std::cerr << "..." <<  std::endl;
        // std::cerr << "[DEVEL] Channel 1 samples : " << std::endl;
        // for (int isample = 0; isample < 8; isample++) {
        //   std::cerr  << hit_.get_waveforms().get_adc(isample, 1) << ' ';
        // }
        // std::cerr << "..." <<  std::endl;
        // std::cerr << "==============================================" << std::endl;
        // /// <<< XXX
        success = true;
      } catch (std::exception & error) {
        DT_LOG_ERROR(_logging_, error.what());
        success = false;
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return success;
    }
    
    void calo_hit_parser::_parse_header_(const std::string & header_line_,
                                         const int index_,
                                         header_type & header_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (_format_ == FORMAT_FROM_2_4) {
        _parse_header_from_2_4_(header_line_, index_, header_);
      } else if (_format_ == FORMAT_FROM_2_3) {
        _parse_header_from_2_3_(header_line_, index_, header_);
      } else {
        _parse_header_legacy_(header_line_, index_, header_);
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }
    
    void calo_hit_parser::_parse_header_from_2_4_(const std::string & header_line_,
                                                  const int index_,
                                                  header_type & header_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      namespace qi = boost::spirit::qi;
      bool res = false;
      if (index_ == 0) {
        std::string header_line = header_line_;
        DT_LOG_DEBUG(_logging_, "header_line = '" << header_line << "'");
        std::string::const_iterator str_iter = header_line.begin();
        std::string::const_iterator end_iter = header_line.end();
        res = qi::phrase_parse(str_iter,
                               end_iter,
                               //  Begin grammar
                               (
                                qi::lit("Slot")             >> qi::uint_[boost::phoenix::ref(header_.slot_id) = boost::spirit::qi::_1]
                                >> qi::lit("Ch")            >> qi::uint_[boost::phoenix::ref(header_.channel_id) = boost::spirit::qi::_1]
                                >> qi::lit("LTO")           >> qi::uint_[boost::phoenix::ref(header_.lto_flag) = boost::spirit::qi::_1]
                                >> qi::lit("HT")            >> qi::uint_[boost::phoenix::ref(header_.ht_flag) = boost::spirit::qi::_1]
                                >> qi::lit("EvtID")         >> qi::uint_[boost::phoenix::ref(header_.event_id) = boost::spirit::qi::_1]
                                >> qi::lit("RawTDC")        >> qi::ulong_long[boost::phoenix::ref(header_.raw_tdc) = boost::spirit::qi::_1]
                                >> qi::lit("TDC")           >> qi::double_[boost::phoenix::ref(header_.raw_tdc_ns) = boost::spirit::qi::_1]
                                >> qi::lit("TrigCount")     >> qi::uint_[boost::phoenix::ref(header_.lt_trig_count) = boost::spirit::qi::_1]
                                >> qi::lit("Timecount")     >> qi::uint_[boost::phoenix::ref(header_.lt_time_count) = boost::spirit::qi::_1]
                                >> qi::lit("RawBaseline")   >> qi::int_[boost::phoenix::ref(header_.raw_baseline) = boost::spirit::qi::_1]
                                >> qi::lit("Baseline")      >> qi::double_[boost::phoenix::ref(header_.baseline_volt) = boost::spirit::qi::_1]
                                >> qi::lit("RawPeak")       >> qi::int_[boost::phoenix::ref(header_.raw_peak) = boost::spirit::qi::_1]
                                >> qi::lit("Peak")          >> qi::double_[boost::phoenix::ref(header_.peak_volt) = boost::spirit::qi::_1]
                                >> qi::lit("PeakCell")      >> qi::uint_[boost::phoenix::ref(header_.peak_cell) = boost::spirit::qi::_1]
                                >> qi::lit("RawCharge")     >> qi::int_[boost::phoenix::ref(header_.raw_charge) = boost::spirit::qi::_1]
                                >> qi::lit("Charge")        >> qi::double_[boost::phoenix::ref(header_.charge_picocoulomb) = boost::spirit::qi::_1]
                                >> qi::lit("Overflow")      >> qi::uint_[boost::phoenix::ref(header_.charge_overflow) = boost::spirit::qi::_1]
                                >> qi::lit("RisingCell")    >> qi::uint_[boost::phoenix::ref(header_.rising_cell) = boost::spirit::qi::_1]
                                >> qi::lit("RisingOffset")  >> qi::uint_[boost::phoenix::ref(header_.rising_offset) = boost::spirit::qi::_1]
                                >> qi::lit("RisingTime")    >> qi::double_[boost::phoenix::ref(header_.rising_ns) = boost::spirit::qi::_1]
                                >> qi::lit("FallingCell")   >> qi::uint_[boost::phoenix::ref(header_.falling_cell) = boost::spirit::qi::_1]
                                >> qi::lit("FallingOffset") >> qi::uint_[boost::phoenix::ref(header_.falling_offset) = boost::spirit::qi::_1]
                                >> qi::lit("FallingTime")   >> qi::double_[boost::phoenix::ref(header_.falling_ns) = boost::spirit::qi::_1]
                                >> qi::lit("FCR")           >> qi::uint_[boost::phoenix::ref(header_.fcr) = boost::spirit::qi::_1]
                                >> qi::lit("UnixTime")      >> qi::double_[boost::phoenix::ref(header_.unix_time) = boost::spirit::qi::_1]
                                ),
                               //  End grammar
                               qi::space);
        DT_THROW_IF(!res || str_iter != end_iter,
                    std::logic_error,
                    "Cannot parse file header line #" << index_ << "; failed at '" << *str_iter << "'!");
      }
     
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }
      
    /// Header parsing
    void calo_hit_parser::_parse_header_from_2_3_(const std::string & header_line_,
                                                  const int index_,
                                                  header_type & header_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      namespace qi = boost::spirit::qi;
      bool res = false;
      if (index_ == 0) {
        std::string header_line = header_line_;
        DT_LOG_DEBUG(_logging_, "header_line = '" << header_line << "'");
        std::string::const_iterator str_iter = header_line.begin();
        std::string::const_iterator end_iter = header_line.end();
        res = qi::phrase_parse(str_iter,
                               end_iter,
                               //  Begin grammar
                               (
                                qi::lit("Slot")             >> qi::uint_[boost::phoenix::ref(header_.slot_id) = boost::spirit::qi::_1]
                                >> qi::lit("Ch")            >> qi::uint_[boost::phoenix::ref(header_.channel_id) = boost::spirit::qi::_1]
                                >> qi::lit("LTO")           >> qi::uint_[boost::phoenix::ref(header_.lto_flag) = boost::spirit::qi::_1]
                                >> qi::lit("HT")            >> qi::uint_[boost::phoenix::ref(header_.ht_flag) = boost::spirit::qi::_1]
                                >> qi::lit("EvtID")         >> qi::uint_[boost::phoenix::ref(header_.event_id) = boost::spirit::qi::_1]
                                >> qi::lit("RawTDC")        >> qi::ulong_long[boost::phoenix::ref(header_.raw_tdc) = boost::spirit::qi::_1]
                                >> qi::lit("TDC")           >> qi::double_[boost::phoenix::ref(header_.raw_tdc_ns) = boost::spirit::qi::_1]
                                >> qi::lit("TrigCount")     >> qi::uint_[boost::phoenix::ref(header_.lt_trig_count) = boost::spirit::qi::_1]
                                >> qi::lit("Timecount")     >> qi::uint_[boost::phoenix::ref(header_.lt_time_count) = boost::spirit::qi::_1]
                                >> qi::lit("RawBaseline")   >> qi::int_[boost::phoenix::ref(header_.raw_baseline) = boost::spirit::qi::_1]
                                >> qi::lit("Baseline")      >> qi::double_[boost::phoenix::ref(header_.baseline_volt) = boost::spirit::qi::_1]
                                >> qi::lit("RawPeak")       >> qi::int_[boost::phoenix::ref(header_.raw_peak) = boost::spirit::qi::_1]
                                >> qi::lit("Peak")          >> qi::double_[boost::phoenix::ref(header_.peak_volt) = boost::spirit::qi::_1]
                                >> qi::lit("PeakCell")      >> qi::uint_[boost::phoenix::ref(header_.peak_cell) = boost::spirit::qi::_1]
                                >> qi::lit("RawCharge")     >> qi::int_[boost::phoenix::ref(header_.raw_charge) = boost::spirit::qi::_1]
                                >> qi::lit("Charge")        >> qi::double_[boost::phoenix::ref(header_.charge_picocoulomb) = boost::spirit::qi::_1]
                                >> qi::lit("Overflow")      >> qi::uint_[boost::phoenix::ref(header_.charge_overflow) = boost::spirit::qi::_1]
                                >> qi::lit("RisingCell")    >> qi::uint_[boost::phoenix::ref(header_.rising_cell) = boost::spirit::qi::_1]
                                >> qi::lit("RisingOffset")  >> qi::uint_[boost::phoenix::ref(header_.rising_offset) = boost::spirit::qi::_1]
                                >> qi::lit("RisingTime")    >> qi::double_[boost::phoenix::ref(header_.rising_ns) = boost::spirit::qi::_1]
                                >> qi::lit("FallingCell")   >> qi::uint_[boost::phoenix::ref(header_.falling_cell) = boost::spirit::qi::_1]
                                >> qi::lit("FallingOffset") >> qi::uint_[boost::phoenix::ref(header_.falling_offset) = boost::spirit::qi::_1]
                                >> qi::lit("FallingTime")   >> qi::double_[boost::phoenix::ref(header_.falling_ns) = boost::spirit::qi::_1]
                                >> qi::lit("FCR")           >> qi::uint_[boost::phoenix::ref(header_.fcr) = boost::spirit::qi::_1]
                                ),
                               //  End grammar
                               qi::space);
        DT_THROW_IF(!res || str_iter != end_iter,
                    std::logic_error,
                    "Cannot parse file header line #" << index_ << "; failed at '" << *str_iter << "'!");
      }
     
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    /// Header parsing
    void calo_hit_parser::_parse_header_legacy_(const std::string & header_line_,
                                                const int index_,
                                                header_type & header_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      namespace qi = boost::spirit::qi;
      bool res = false;
      if (index_ == 0) {
        std::string header_line = header_line_;
        DT_LOG_DEBUG(_logging_, "header_line = '" << header_line << "'");
        std::string::const_iterator str_iter = header_line.begin();
        std::string::const_iterator end_iter = header_line.end();
        res = qi::phrase_parse(str_iter,
                               end_iter,
                               //  Begin grammar
                               (
                               qi::lit("Slot")             >> qi::uint_[boost::phoenix::ref(header_.slot_id) = boost::spirit::qi::_1]
                               >> qi::lit("Ch")            >> qi::uint_[boost::phoenix::ref(header_.channel_id) = boost::spirit::qi::_1]
                               >> qi::lit("EvtID")         >> qi::uint_[boost::phoenix::ref(header_.event_id) = boost::spirit::qi::_1]
                               >> qi::lit("RawTDC")        >> qi::ulong_long[boost::phoenix::ref(header_.raw_tdc) = boost::spirit::qi::_1]
                               >> qi::lit("TDC")           >> qi::double_[boost::phoenix::ref(header_.raw_tdc_ns) = boost::spirit::qi::_1]
                               >> qi::lit("TrigCount")     >> qi::uint_[boost::phoenix::ref(header_.lt_trig_count) = boost::spirit::qi::_1]
                               >> qi::lit("Timecount")     >> qi::uint_[boost::phoenix::ref(header_.lt_time_count) = boost::spirit::qi::_1]
                               >> qi::lit("RawBaseline")   >> qi::int_[boost::phoenix::ref(header_.raw_baseline) = boost::spirit::qi::_1]
                               >> qi::lit("Baseline")      >> qi::double_[boost::phoenix::ref(header_.baseline_volt) = boost::spirit::qi::_1]
                               >> qi::lit("RawPeak")       >> qi::int_[boost::phoenix::ref(header_.raw_peak) = boost::spirit::qi::_1]
                               >> qi::lit("Peak")          >> qi::double_[boost::phoenix::ref(header_.peak_volt) = boost::spirit::qi::_1]
                               >> qi::lit("RawCharge")     >> qi::int_[boost::phoenix::ref(header_.raw_charge) = boost::spirit::qi::_1]
                               >> qi::lit("Charge")        >> qi::double_[boost::phoenix::ref(header_.charge_picocoulomb) = boost::spirit::qi::_1]
                               >> qi::lit("Overflow")      >> qi::uint_[boost::phoenix::ref(header_.charge_overflow) = boost::spirit::qi::_1]
                               >> qi::lit("RisingCell")    >> qi::uint_[boost::phoenix::ref(header_.rising_cell) = boost::spirit::qi::_1]
                               >> qi::lit("RisingOffset")  >> qi::uint_[boost::phoenix::ref(header_.rising_offset) = boost::spirit::qi::_1]
                               >> qi::lit("RisingTime")    >> qi::double_[boost::phoenix::ref(header_.rising_ns) = boost::spirit::qi::_1]
                               >> qi::lit("FallingCell")   >> qi::uint_[boost::phoenix::ref(header_.falling_cell) = boost::spirit::qi::_1]
                               >> qi::lit("FallingOffset") >> qi::uint_[boost::phoenix::ref(header_.falling_offset) = boost::spirit::qi::_1]
                               >> qi::lit("FallingTime")   >> qi::double_[boost::phoenix::ref(header_.falling_ns) = boost::spirit::qi::_1]
                               >> qi::lit("FCR")           >> qi::uint_[boost::phoenix::ref(header_.fcr) = boost::spirit::qi::_1]
                               ),
                               //  End grammar
                               qi::space);
        DT_THROW_IF(!res || str_iter != end_iter,
                    std::logic_error,
                    "Cannot parse file header line #" << index_ << "; failed at '" << *str_iter << "'!");
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }
    
    void calo_hit_parser::_parse_waveform_(const std::string & data_line_,
                                           const uint16_t channel_index_,
                                           snfee::data::calo_hit_record::waveforms_record & waveforms_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      namespace qi = boost::spirit::qi;

      std::string::const_iterator str_iter = data_line_.begin();
      std::string::const_iterator end_iter = data_line_.end();
      std::vector<int16_t> channel_waveform_data;
      //     std::size_t waveform_data_size = XXX; // unknown from the header !!! so we let Spirit guess it...
      bool res = false;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              // qi::repeat(hit_.waveform_data_size)[qi::int_]
                              +qi::int_
                              ),
                             //  End grammar
                             qi::space,
                             channel_waveform_data);
      DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                  "Cannot parse hit waveform samples for channel [" << channel_index_ << "!");
      DT_LOG_DEBUG(_logging_, "Number of parsed samples : " << channel_waveform_data.size());
      for (int i = 0; i < 10 /*channel_waveform_data.size()*/; i++) {
        DT_LOG_DEBUG(_logging_, "Channel waveform sample[" << i << "] = " << channel_waveform_data[i]);
      }
 
      // Populate the waveform for this channel:
      if (waveforms_.get_samples().size() == 0) {
        waveforms_.reset(channel_waveform_data.size());
      } else {
        DT_THROW_IF(waveforms_.get_samples().size() != channel_waveform_data.size(),
                    std::logic_error,
                    "Waveforms number of samples does not match the parsed waveform data for channel [" << channel_index_ << "!"); 
      }
      for (uint16_t icell = 0; icell < channel_waveform_data.size(); icell++) {
        waveforms_.set_adc(icell, channel_index_, channel_waveform_data[icell]);
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // namespace io
} // namespace snfee
