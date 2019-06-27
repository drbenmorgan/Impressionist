// This project:
#include <snfee/data/calo_hit_record.h>

// Standard Library:
#include <random>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/feb_constants.h>

namespace snfee {
  namespace data {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(calo_hit_record,
                                           "snfee::data::calo_hit_record")

    calo_hit_record::two_channel_adc_record::two_channel_adc_record()
    {
      for (int ich = 0; ich < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS; ich++) {
        set_adc(ich, SAMPLE_ADC_DEFAULT);
      }
      return;
    }
    
    void calo_hit_record::two_channel_adc_record::set_adc(const uint16_t channel_index_,
                                                          const uint16_t adc_)
    {
      DT_THROW_IF(channel_index_ >= snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS,
                  std::logic_error,
                  "Invalid SAMLONG channel index [" << channel_index_ << "]!");
      DT_THROW_IF(adc_ > SAMPLE_ADC_MAX,
                  std::logic_error,
                  "Invalid ADC value [" << adc_ << "] for SAMLONG channel index [" << channel_index_ << "]!");
      // std::cerr << "[devel] calo_hit_record::two_channel_adc_record::set_adc: channel #"
      //        << channel_index_ << " adc=" << adc_ << std::endl; 
      _adc_[channel_index_] = adc_;
      return;
    }

    uint16_t calo_hit_record::two_channel_adc_record::get_adc(const uint16_t channel_index_) const
    {
      DT_THROW_IF(channel_index_ >= snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS,
                  std::logic_error,
                  "Invalid SAMLONG channel index [" << channel_index_ << "]!");
      return _adc_[channel_index_];
    }

    
    calo_hit_record::waveforms_record::waveforms_record(const uint16_t nb_samples_)
    {
      _samples_.reserve(snfee::model::feb_constants::SAMLONG_MAX_NUMBER_OF_SAMPLES);
      two_channel_adc_record default_adc_record;
      _samples_.assign(nb_samples_, default_adc_record);
      return;
    }

    void calo_hit_record::waveforms_record::reset(const uint16_t nb_samples_)
    {
      _samples_.reserve(snfee::model::feb_constants::SAMLONG_MAX_NUMBER_OF_SAMPLES);
      two_channel_adc_record default_adc_record;
      _samples_.assign(nb_samples_, default_adc_record);
      return;
    }

    const std::vector<calo_hit_record::two_channel_adc_record> &
    calo_hit_record::waveforms_record::get_samples() const
    {
      return _samples_;
    }

    void calo_hit_record::waveforms_record::set_adc(const uint16_t sample_index_, 
                                                    const uint16_t channel_,
                                                    const uint16_t adc_)
    {
      DT_THROW_IF(sample_index_ >= _samples_.size(),
                  std::logic_error,
                  "Invalid SAMLONG sample index [" << sample_index_ << "]!");
      _samples_[sample_index_].set_adc(channel_, adc_);
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "==> Write waveform ADC[" << sample_index_ << "] = " << adc_);
      // if (datatools::logger::is_debug(datatools::logger::PRIO_DEBUG)) {
      //   uint16_t adc = get_adc(sample_index_, channel_);
      //   DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "==> Read  waveform ADC[" << sample_index_ << "] = " << adc);
      // }
      return;
    }

    uint16_t calo_hit_record::waveforms_record::get_adc(const uint16_t sample_index_, const uint16_t channel_) const
    {
      DT_THROW_IF(sample_index_ >= _samples_.size(),
                  std::logic_error,
                  "Invalid SAMLONG sample index [" << sample_index_ << "]!");
      return _samples_[sample_index_].get_adc(channel_);
    }

    void calo_hit_record::waveforms_record::invalidate()
    {
      _samples_.clear();
      return;
    }

    calo_hit_record::calo_hit_record()
    {
      for (int ich = 0; ich < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS; ich++) {
        _channel_data_[ich].invalidate();
      }
      return;
    }

    calo_hit_record::~calo_hit_record()
    {
      return;
    }
  
    bool calo_hit_record::is_complete() const
    {
      if (_hit_num_ == INVALID_NUMBER) return false;
      if (_trigger_id_ == INVALID_TRIGGER_ID) return false;
      if (_tdc_ == TDC_INVALID) return false;
      if (_crate_num_ == INVALID_NUMBER_16) return false;
      if (_board_num_ == INVALID_NUMBER_16) return false;
      if (_chip_num_ == INVALID_NUMBER) return false;
      if (_l2_id_ == INVALID_NUMBER_16U) return false;
      if (_fcr_ == snfee::model::feb_constants::SAMLONG_INVALID_SAMPLE) return false;
      if (_has_waveforms_) {
        if (_waveform_start_sample_ == INVALID_WAVEFORM_START_SAMPLE) return false;
        if (_waveform_number_of_samples_ == INVALID_WAVEFORM_NUMBER_OF_SAMPLES) return false;
        if (_waveforms_.get_samples().size() != _waveform_number_of_samples_) return false;
      }
      return true;
    }

    void calo_hit_record::invalidate()
    {
      _hit_num_      = INVALID_NUMBER;
      _trigger_id_   = INVALID_TRIGGER_ID;
      _tdc_          = TDC_INVALID; 
      _crate_num_    = INVALID_NUMBER_16;
      _board_num_    = INVALID_NUMBER_16;
      _chip_num_     = INVALID_NUMBER_16;
      _event_id_     = 0;
      _l2_id_        = INVALID_NUMBER_16U;
      _fcr_          = snfee::model::feb_constants::SAMLONG_INVALID_SAMPLE;
      _has_waveforms_ = false;
      _waveform_start_sample_ = INVALID_WAVEFORM_START_SAMPLE;
      _waveform_number_of_samples_ = INVALID_WAVEFORM_NUMBER_OF_SAMPLES;
      _waveforms_.invalidate();
      for (int ich = 0; ich < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS; ich++) { 
        _channel_data_[ich].invalidate();
      }
       return;
    }

    void calo_hit_record::channel_data_record::invalidate()
    {
      _lt_          = false;  
      _ht_           = false; 
      _underflow_    = false; 
      _overflow_     = false; 
      _baseline_     = 0;     
      _peak_         = 0;   
      _peak_cell_    = 0;    
      _charge_       = 0;    
      _rising_cell_  = 0; 
      _falling_cell_ = 0; 
      // for (int icell = 0;
      //      icell < snfee::model::feb_constants::SAMLONG_MAX_NUMBER_OF_SAMPLES;
      //      icell++) {
      //   samples.clear(); 
      // }
      return;
    }

    bool calo_hit_record::channel_data_record::is_lt() const
    {
      return _lt_;
    }
    
    bool calo_hit_record::channel_data_record::is_ht() const
    {
      return _ht_;
    }

    bool calo_hit_record::channel_data_record::is_lto() const
    {
      return _lt_ &&  ! _ht_;
    }
    
    bool calo_hit_record::channel_data_record::is_underflow() const
    {
      return _underflow_;
    }
    
    bool calo_hit_record::channel_data_record::is_overflow() const
    {
      return _overflow_;
    }
    
    int16_t calo_hit_record::channel_data_record::get_baseline() const
    {
      return _baseline_;
    }
    
    int16_t calo_hit_record::channel_data_record::get_peak() const
    {
      return _peak_;
    }
    
    int16_t calo_hit_record::channel_data_record::get_peak_cell() const
    {
      return _peak_cell_;
    }
    
    int32_t calo_hit_record::channel_data_record::get_charge() const
    {
      return _charge_;
    }
    
    int32_t calo_hit_record::channel_data_record::get_rising_cell() const
    {
      return _rising_cell_;
    }
    
    int32_t calo_hit_record::channel_data_record::get_falling_cell() const
    {
      return _falling_cell_;
    }
    
    void calo_hit_record::channel_data_record::make(const bool    lt_, 
                                                    const bool    ht_,        
                                                    const bool    underflow_,  
                                                    const bool    overflow_,    
                                                    const int16_t baseline_,   
                                                    const int16_t peak_,      
                                                    const int16_t peak_cell_,   
                                                    const int32_t charge_,     
                                                    const int32_t rising_cell_,   
                                                    const int32_t falling_cell_)
    {
      _lt_ = lt_;
      _ht_ = ht_;
      _underflow_ = underflow_;
      _overflow_ = overflow_;
      _baseline_ = baseline_;
      _peak_ = peak_;
      _peak_cell_ = peak_cell_;
      _charge_ = charge_;
      _rising_cell_ = rising_cell_;
      _falling_cell_ = falling_cell_;
      return;
    }
    
    // virtual
    void calo_hit_record::channel_data_record::print_tree(std::ostream & out_,
                                                          const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
  
      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "LT           : " << std::boolalpha << _lt_ << std::endl;

      out_ << popts.indent << tag
           << "HT           : " << std::boolalpha << _ht_ << std::endl;

      out_ << popts.indent << tag;
      out_ << "Underflow    : " << std::boolalpha << _underflow_
           << std::endl;
        
      out_ << popts.indent << tag;
      out_ << "Overflow     : " << std::boolalpha << _overflow_
           << std::endl;
        
      out_ << popts.indent << tag
           << "Baseline     : " << _baseline_ << std::endl;

      out_ << popts.indent << tag
           << "Peak         : " << _peak_ << std::endl;

      out_ << popts.indent << tag
           << "Peak cell    : " << _peak_cell_ << std::endl;

      out_ << popts.indent << tag
           << "Charge       : " << _charge_ << std::endl;
 
      out_ << popts.indent << tag
           << "Rising cell  : " << _rising_cell_ << std::endl;
 
      out_ << popts.indent << last_tag
           << "Falling cell : " << _falling_cell_ << std::endl;

      return;
    }
    
    // virtual
    void calo_hit_record::print_tree(std::ostream & out_,
                                     const boost::property_tree::ptree & options_) const
    {
      // bool interactive = false;
      bool print_waveform_samples = false;
      print_waveform_samples = options_.get("with_waveform_samples", false);
      // interactive = options_.get("interactive", false);
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
  
      out_ << popts.indent << tag
           << "Hit number    : " << _hit_num_ << std::endl;
   
      out_ << popts.indent << tag
           << "Trigger ID    : " << _trigger_id_ << std::endl;
   
      out_ << popts.indent << tag
           << "TDC           : " << _tdc_ << std::endl;
     
      out_ << popts.indent << tag
           << "Crate number  : " << _crate_num_ << std::endl;
    
      out_ << popts.indent << tag
           << "Board number  : " << _board_num_ << " (Wavecatcher)" << std::endl;
    
      out_ << popts.indent << tag
           << "Chip number   : " << _chip_num_ << " (SAMLONG)" << std::endl;
 
      out_ << popts.indent << tag
           << "Event ID      : " << _event_id_ << std::endl;
 
      out_ << popts.indent << tag
           << "L2 ID         : " << _l2_id_ << std::endl;
 
      out_ << popts.indent << tag
           << "FCR           : " << _fcr_ << std::endl;
  
      out_ << popts.indent << tag
           << "Has waveforms : " << std::boolalpha << _has_waveforms_ << std::endl;

      if (_has_waveforms_) {
      
        out_ << popts.indent << tag
             << "Waveform start sample      : " << get_waveform_start_sample()
             << std::endl;
      
        out_ << popts.indent << tag
             << "Waveform number of samples : " << get_waveform_number_of_samples()
             << std::endl;
     
        out_ << popts.indent << tag
             << "Waveforms size : " << get_waveforms().get_samples().size()
             << std::endl;

        if (print_waveform_samples) {
          out_ << popts.indent << tag
               << "Waveforms @ : " << &get_waveforms()
               << std::endl;
          out_ << popts.indent << skip_tag << tag << "Channel 0 samples : " << std::endl;
          out_ << popts.indent << skip_tag << skip_tag << "+ ";
          for (int isample = 0; isample < (int) get_waveforms().get_samples().size(); isample++) {
            out_ << get_waveforms().get_adc(isample, 0) << ' ';
            if ((isample + 1) % 16 == 0) {
              out_ << std::endl << popts.indent << skip_tag << skip_tag << "+ ";
            }
          }
          out_ << std::endl;
          out_ << popts.indent << skip_tag << last_tag << "Channel 1 samples : " << std::endl;
          out_ << popts.indent << skip_tag << last_skip_tag << "+ ";
          for (int isample = 0; isample < (int) get_waveforms().get_samples().size(); isample++) {
            out_ << get_waveforms().get_adc(isample, 1) << ' ';
            if ((isample + 1) % 16 == 0) {
              out_ << std::endl << popts.indent << skip_tag <<last_skip_tag << "+ ";
            }
          }
          out_ << std::endl;
        }
        
      }

      for (int ich = 0; ich < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS; ich++) {
        const channel_data_record & chRec = _channel_data_[ich];
      
        out_ << popts.indent << tag
             << "Channel #" << ich << " record : " << std::endl;
        {
          boost::property_tree::ptree popts2;
          std::ostringstream oss;
          oss << popts.indent << skip_tag;
          popts2.put("indent", oss.str());
          chRec.print_tree(out_, popts2);
        }
      }
     
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete : " << std::boolalpha << is_complete() << std::endl;

      // if (interactive) {
      //   std::string resp;
      //   std::getline(std::cin,resp);
      // }
      return;
    }

    void calo_hit_record::set_hit_num(const int32_t hit_num_)
    {
      _hit_num_ = hit_num_;
      return;
    }

    void calo_hit_record::set_trigger_id(const int32_t trig_id_)
    {
      _trigger_id_ = trig_id_;
      return;
    }

    uint64_t calo_hit_record::get_tdc() const
    {
      return _tdc_;
    }
  
    int32_t calo_hit_record::get_hit_num() const
    {
      return _hit_num_;
    }
  
    int32_t calo_hit_record::get_trigger_id() const
    {
      return _trigger_id_;
    }
   
    int16_t calo_hit_record::get_crate_num() const  
    {
      return _crate_num_;
    }
 
    int16_t calo_hit_record::get_board_num() const  
    {
      return _board_num_;
    }
  
    int16_t calo_hit_record::get_chip_num() const
    {
      return _chip_num_;
    }
  
    bool calo_hit_record::has_waveforms() const
    {
      return _has_waveforms_;
    }

    uint16_t calo_hit_record::get_waveform_start_sample() const
    {
      DT_THROW_IF(!_has_waveforms_, std::logic_error, "No waveforms!");
      return _waveform_start_sample_;
    }
    
    uint16_t calo_hit_record::get_waveform_number_of_samples() const
    {
      DT_THROW_IF(!_has_waveforms_, std::logic_error, "No waveforms!");
      return _waveform_number_of_samples_;
    }

    const calo_hit_record::waveforms_record & calo_hit_record::get_waveforms() const
    {
      return _waveforms_;
    }
 
    const calo_hit_record::channel_data_record &
    calo_hit_record::get_channel_data(const uint16_t channel_num_) const
    {
      DT_THROW_IF(channel_num_ >= snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS,
                  std::logic_error,
                  "Invalid SAMLONG channel number!");
      return _channel_data_[channel_num_];
    }

    calo_hit_record::channel_data_record &
    calo_hit_record::grab_channel_data(const uint16_t channel_num_)
    {
      DT_THROW_IF(channel_num_ >= snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS,
                  std::logic_error,
                  "Invalid SAMLONG channel number!");
      return _channel_data_[channel_num_];
    }

    uint16_t calo_hit_record::get_event_id() const
    {
      return _event_id_;
    }

    uint16_t calo_hit_record::get_l2_id() const
    {
      return _l2_id_;
    }

    uint16_t calo_hit_record::get_fcr() const
    {
      return _fcr_;
    }

    // static
    void calo_hit_record::populate_mock_hit(calo_hit_record & hit_rec_,
                                            const bool     signal_on_first_channel,
                                            const bool     signal_on_second_channel,
                                            const int32_t  hit_num_,
                                            const int32_t  trigger_id_,
                                            const uint64_t tdc_,
                                            const int16_t  crate_num_,
                                            const int16_t  board_num_,
                                            const int16_t  chip_num_,
                                            const uint16_t event_id_,
                                            const uint16_t l2_id_,
                                            const uint16_t fcr_,
                                            const bool     has_waveforms_,
                                            const uint16_t waveform_start_sample_,
                                            const uint16_t waveform_number_of_samples_)
    {
      // std::clog << "[devel] calo_hit_record::populate_mock_hit: has_waveform = "
      //           << std::boolalpha << has_waveform_ << std::endl;
      std::default_random_engine generator;
      std::uniform_int_distribution<int> distribution(-4,4);
      hit_rec_.make(hit_num_,
                    trigger_id_,
                    tdc_,
                    crate_num_,
                    board_num_,
                    chip_num_,
                    event_id_,
                    l2_id_,
                    fcr_,
                    has_waveforms_,
                    waveform_start_sample_,
                    waveform_number_of_samples_);
      for (int ich = 0; ich < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS; ich++) {
        int channel_num = ich;
        bool    lt = false;
        bool    ht = false;
        bool    underflow = false;
        bool    overflow = false;
        int16_t baseline = 3;
        int16_t peak = -41;
        int16_t peak_cell = 2;
        int32_t charge = 259;
        int32_t rising_cell = 110;
        int32_t falling_cell = 109;
        bool signal = false;
        if ((signal_on_first_channel and ich == 0) or
            (signal_on_second_channel and ich == 1)) {
          signal = true;
        }
        if (signal) {
          lt = true;
          ht = true;
          underflow = false;
          overflow = false;
          baseline = 8582;
          peak = -7891;
          peak_cell = 8514;
          charge = -27704;
          rising_cell = 937;
          falling_cell = 909;
        }
        hit_rec_.make_channel(channel_num,
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
        if (signal) {
          const std::vector<int16_t> & adc_sample = mock_adc_samples();
          for (int icell = 0; icell < waveform_number_of_samples_; icell++) {
            int16_t adc = 2048;
            adc = adc_sample[icell];
            // std::cerr << "[devel] channel #" << channel_num << " adc[" << icell << "] = " << adc << std::endl; 
            hit_rec_.set_waveform_adc(channel_num, icell, adc);
          }
        } else {
          for (int icell = 0; icell < waveform_number_of_samples_; icell++) {
            int16_t adc = 2048;
            adc += distribution(generator) ;
            // std::cerr << "[devel] channel #" << channel_num << " adc[" << icell << "] = " << adc << std::endl; 
            hit_rec_.set_waveform_adc(channel_num, icell, adc);
          }
        }
      }
      
      return;
    }

    void calo_hit_record::set_waveform_adc(const uint16_t channel_index_,
                                           const uint16_t sample_index_,
                                           const uint16_t adc_)
    {
      _waveforms_.set_adc(sample_index_, channel_index_, adc_);
      return;
    }
    
    void calo_hit_record::make(const int32_t  hit_num_,
                               const int32_t  trigger_id_,
                               const uint64_t tdc_,
                               const int16_t  crate_num_,
                               const int16_t  board_num_,
                               const int16_t  chip_num_,
                               const uint16_t event_id_,
                               const uint16_t l2_id_,
                               const uint16_t fcr_,
                               const bool     has_waveforms_,
                               const uint16_t waveform_start_sample_,
                               const uint16_t waveform_number_of_samples_,
                               const bool     preserve_waveforms_)
    {
      // std::clog << "[devel] calo_hit_record::make: has_waveform_  = "
      //           << std::boolalpha << has_waveform_ << std::endl;
      DT_THROW_IF(hit_num_ <= INVALID_NUMBER, std::logic_error, "Invalid hit number!");
      _hit_num_ = hit_num_;    
      DT_THROW_IF(trigger_id_ <= INVALID_TRIGGER_ID, std::logic_error, "Invalid trigger ID!");
      _trigger_id_ = trigger_id_;  
      DT_THROW_IF(tdc_ > TDC_MAX, std::logic_error, "Overflow TDC!");
      _tdc_ = tdc_;  
      DT_THROW_IF(crate_num_ < 0 and crate_num_ >= snfee::model::feb_constants::MAX_NUMBER_OF_CALO_CRATES,
                  std::logic_error, "Invalid calorimeter crate number!");
      _crate_num_ = crate_num_;   
      DT_THROW_IF(board_num_ < 0 and board_num_ >= snfee::model::feb_constants::MAX_CALO_CRATE_NUMBER_OF_FEBS,
                  std::logic_error, "Invalid calorimeter FEB number!");
      _board_num_ = board_num_;   
      DT_THROW_IF(chip_num_ < 0 and chip_num_ >= snfee::model::feb_constants::CFEB_NUMBER_OF_SAMLONGS,
                  std::logic_error, "Invalid SAMLONG chip number!");
      _chip_num_ = chip_num_;
      _event_id_ = event_id_;
      DT_THROW_IF(l2_id_ > 0x1F, std::logic_error, "Invalid L2 ID!");
      _l2_id_ = l2_id_;
      DT_THROW_IF(fcr_ > 0x3FF, std::logic_error, "Invalid FCR!");
      _fcr_ = fcr_;
      _has_waveforms_ = has_waveforms_;
      if (_has_waveforms_) {
        DT_THROW_IF(waveform_start_sample_ == INVALID_WAVEFORM_START_SAMPLE,
                    std::logic_error, "Invalid waveform start sample!");
        DT_THROW_IF(waveform_start_sample_ > INVALID_WAVEFORM_START_SAMPLE,
                    std::logic_error, "Overflow waveform start sample!");
        _waveform_start_sample_ = waveform_start_sample_;
        DT_THROW_IF(waveform_number_of_samples_ == INVALID_WAVEFORM_NUMBER_OF_SAMPLES,
                    std::logic_error, "Invalid waveform number of samples!");
        DT_THROW_IF(waveform_number_of_samples_ > snfee::model::feb_constants::SAMLONG_MAX_NUMBER_OF_SAMPLES,
                    std::logic_error, "Overflow waveform number of samples!");
        _waveform_number_of_samples_ = waveform_number_of_samples_;
        if (! preserve_waveforms_ or _waveforms_.get_samples().size() == 0) {
          _waveforms_.reset(_waveform_number_of_samples_);
        } else {
          DT_THROW_IF(_waveforms_.get_samples().size() != _waveform_number_of_samples_,
                      std::logic_error,
                      "Waveforms current depth does not match the number of samples!");
        }
      }
      return;
    }
  
    void calo_hit_record::make_channel(const int channel_num_,
                                       const bool lt_,
                                       const bool ht_,
                                       const bool underflow_,
                                       const bool overflow_,
                                       const int16_t baseline_,
                                       const int16_t peak_,
                                       const int16_t peak_cell_,
                                       const int32_t charge_,
                                       const int32_t rising_cell_,
                                       const int32_t falling_cell_)
    {
      DT_THROW_IF(channel_num_ != 0 and channel_num_ != 1,
                  std::logic_error,
                  "Invalid SAMLONG channel number!");
      channel_data_record & chRec = _channel_data_[channel_num_];
      chRec.make(lt_,
                 ht_,
                 underflow_,
                 overflow_,
                 baseline_,
                 peak_,
                 peak_cell_,
                 charge_,
                 rising_cell_,
                 falling_cell_);
      return;
    }

    const std::vector<int16_t> & calo_hit_record::mock_adc_samples()
    {
      static std::vector<int16_t> _samples;
      if (_samples.size() == 0) {
        _samples.assign(1024, snfee::data::calo_hit_record::SAMPLE_ADC_DEFAULT);
        _samples = {
          2584, 2587, 2583, 2585, 2583, 2583, 2584, 2584, 2581, 2585, 2585, 2585, 2586, 2586, 2584, 2585, 2585, 2585, 2582, 2587, 2583, 2584, 2588, 2586, 2585, 2584, 2582, 2583, 2584, 2586, 2586, 2586, 2582, 2589, 2583, 2585, 2583, 2583, 2584, 2582, 2583, 2588, 2587, 2585, 2583, 2586, 2586, 2585, 2583, 2586, 2583, 2585, 2585, 2585, 2586, 2585, 2583, 2584, 2583, 2583, 2584, 2584, 2587, 2585, 2586, 2588, 2584, 2584, 2583, 2584, 2587, 2586, 2585, 2585, 2585, 2584, 2585, 2584, 2583, 2583, 2582, 2587, 2583, 2587, 2584, 2584, 2586, 2584, 2586, 2584, 2584, 2584, 2583, 2584, 2586, 2586, 2586, 2587, 2582, 2586, 2583, 2583, 2585, 2586, 2583, 2584, 2583, 2585, 2581, 2582, 2583, 2585, 2585, 2587, 2584, 2586, 2587, 2584, 2584, 2583, 2583, 2584, 2584, 2584, 2581, 2585, 2585, 2585, 2584, 2587, 2585, 2584, 2584, 2586, 2588, 2588, 2583, 2587, 2586, 2586, 2583, 2585, 2586, 2584, 2584, 2585, 2584, 2585, 2585, 2583, 2585, 2584, 2584, 2585, 2584, 2585, 2585, 2587, 2588, 2586, 2582, 2585, 2586, 2584, 2586, 2585, 2585, 2584, 2584, 2585, 2585, 2582, 2582, 2581, 2584, 2584, 2585, 2586, 2582, 2584, 2585, 2584, 2583, 2583, 2584, 2587, 2585, 2587, 2584, 2585, 2585, 2585, 2585, 2585, 2582, 2584, 2585, 2587, 2588, 2585, 2586, 2584, 2585, 2584, 2584, 2584, 2583, 2587, 2584, 2587, 2584, 2584, 2583, 2583, 2584, 2584, 2585, 2585, 2584, 2584, 2583, 2584, 2583, 2583, 2585, 2586, 2583, 2585, 2584, 2585, 2584, 2583, 2583, 2582, 2582, 2584, 2586, 2587, 2585, 2586, 2583, 2585, 2583, 2586, 2586, 2586, 2586, 2586, 2585, 2587, 2585, 2583, 2584, 2584, 2583, 2584, 2583, 2587, 2584, 2584, 2583, 2582, 2585, 2584, 2584, 2585, 2584, 2586, 2583, 2585, 2586, 2584, 2584, 2586, 2580, 2583, 2584, 2585, 2588, 2585, 2585, 2586, 2584, 2583, 2582, 2584, 2586, 2587, 2584, 2587, 2584, 2586, 2585, 2581, 2585, 2583, 2583, 2585, 2584, 2586, 2583, 2585, 2584, 2583, 2582, 2584, 2583, 2587, 2587, 2585, 2585, 2583, 2583, 2584, 2582, 2585, 2584, 2584, 2587, 2588, 2585, 2586, 2582, 2584, 2581, 2581, 2584, 2587, 2585, 2586, 2587, 2584, 2582, 2584, 2584, 2586, 2583, 2586, 2585, 2587, 2584, 2586, 2585, 2583, 2583, 2584, 2585, 2587, 2583, 2586, 2585, 2587, 2583, 2587, 2581, 2583, 2582, 2584, 2585, 2585, 2584, 2586, 2585, 2585, 2581, 2586, 2585, 2584, 2584, 2587, 2584, 2586, 2585, 2583, 2584, 2583, 2583, 2583, 2585, 2585, 2584, 2584, 2585, 2585, 2584, 2584, 2585, 2586, 2586, 2588, 2587, 2585, 2586, 2585, 2584, 2584, 2584, 2586, 2587, 2588, 2587, 2587, 2583, 2585, 2583, 2586, 2586, 2586, 2585, 2587, 2586, 2586, 2584, 2586, 2587, 2585, 2582, 2584, 2584, 2587, 2585, 2586, 2587, 2585, 2583, 2583, 2583, 2585, 2583, 2584, 2582, 2585, 2584, 2584, 2581, 2585, 2583, 2581, 2583, 2584, 2584, 2586, 2584, 2585, 2584, 2585, 2585, 2583, 2583, 2586, 2583, 2586, 2585, 2584, 2586, 2585, 2586, 2584, 2582, 2583, 2582, 2585, 2585, 2588, 2583, 2586, 2582, 2585, 2585, 2586, 2587, 2585, 2585, 2586, 2584, 2587, 2582, 2583, 2585, 2586, 2586, 2587, 2585, 2587, 2586, 2585, 2585, 2584, 2584, 2585, 2585, 2583, 2582, 2587, 2586, 2587, 2585, 2586, 2584, 2583, 2581, 2585, 2588, 2586, 2586, 2585, 2585, 2583, 2582, 2583, 2584, 2586, 2584, 2585, 2581, 2584, 2585, 2587, 2584, 2585, 2585, 2584, 2585, 2584, 2581, 2587, 2586, 2586, 2583, 2585, 2582, 2586, 2585, 2586, 2584, 2585, 2584, 2584, 2581, 2583, 2580, 2584, 2583, 2585, 2585, 2585, 2583, 2584, 2583, 2585, 2586, 2584, 2581, 2585, 2582, 2585, 2586, 2584, 2585, 2586, 2583, 2584, 2581, 2586, 2586, 2586, 2587, 2585, 2581, 2585, 2584, 2585, 2582, 2586, 2585, 2586, 2585, 2587, 2583, 2584, 2584, 2585, 2585, 2585, 2583, 2584, 2585, 2584, 2581, 2585, 2584, 2583, 2584, 2589, 2585, 2585, 2586, 2583, 2585, 2586, 2583, 2587, 2587, 2586, 2584, 2586, 2587, 2586, 2584, 2586, 2584, 2586, 2583, 2583, 2586, 2585, 2583, 2586, 2584, 2582, 2584, 2587, 2583, 2585, 2584, 2586, 2584, 2586, 2586, 2587, 2586, 2583, 2583, 2584, 2584, 2584, 2582, 2583, 2584, 2584, 2584, 2586, 2583, 2588, 2584, 2585, 2584, 2585, 2587, 2587, 2587, 2584, 2584, 2585, 2585, 2585, 2582, 2587, 2584, 2585, 2587, 2584, 2586, 2585, 2584, 2586, 2584, 2585, 2582, 2584, 2584, 2586, 2586, 2586, 2583, 2584, 2585, 2588, 2588, 2584, 2584, 2585, 2583, 2586, 2583, 2584, 2585, 2586, 2586, 2585, 2584, 2584, 2583, 2584, 2586, 2583, 2584, 2585, 2585, 2585, 2582, 2584, 2586, 2585, 2588, 2589, 2585, 2585, 2585, 2585, 2588, 2583, 2582, 2585, 2586, 2584, 2582, 2585, 2586, 2585, 2585, 2586, 2585, 2584, 2582, 2585, 2589, 2583, 2584, 2583, 2583, 2586, 2582, 2583, 2583, 2586, 2586, 2585, 2584, 2586, 2583, 2584, 2586, 2586, 2583, 2584, 2586, 2585, 2584, 2584, 2584, 2583, 2583, 2587, 2586, 2585, 2586, 2585, 2587, 2587, 2585, 2586, 2583, 2584, 2584, 2586, 2584, 2584, 2583, 2585, 2583, 2584, 2585, 2583, 2584, 2585, 2585, 2586, 2584, 2585, 2583, 2584, 2583, 2585, 2586, 2587, 2584, 2584, 2584, 2586, 2583, 2584, 2584, 2584, 2585, 2584, 2584, 2586, 2587, 2586, 2584, 2585, 2584, 2584, 2585, 2588, 2589, 2586, 2584, 2587, 2586, 2586, 2584, 2583, 2584, 2586, 2584, 2587, 2585, 2585, 2585, 2585, 2585, 2583, 2582, 2583, 2586, 2585, 2585, 2585, 2585, 2586, 2582, 2587, 2583, 2585, 2585, 2587, 2586, 2585, 2585, 2584, 2582, 2585, 2586, 2585, 2584, 2585, 2584, 2587, 2583, 2586, 2583, 2587, 2586, 2585, 2584, 2584, 2585, 2584, 2584, 2585, 2585, 2584, 2584, 2585, 2583, 2586, 2585, 2585, 2586, 2585, 2582, 2584, 2586, 2583, 2583, 2583, 2586, 2587, 2586, 2586, 2583, 2583, 2584, 2584, 2587, 2582, 2582, 2585, 2583, 2583, 2583, 2584, 2583, 2583, 2583, 2586, 2584, 2587, 2586, 2585, 2582, 2576, 2557, 2523, 2463, 2369, 2249, 2103, 1948, 1807, 1684, 1620, 1598, 1607, 1637, 1672, 1702, 1721, 1732, 1736, 1740, 1754, 1773, 1798, 1829, 1859, 1885, 1911, 1931, 1950, 1965, 1986, 2007, 2025, 2050, 2075, 2101, 2128, 2151, 2180, 2209, 2235, 2260, 2285, 2304, 2329, 2354, 2380, 2404, 2427, 2450, 2475, 2496, 2517, 2534, 2550, 2564, 2578, 2594, 2606, 2611, 2614, 2611, 2605, 2593, 2580, 2567, 2558, 2547, 2535, 2532, 2534, 2538, 2547, 2559, 2571, 2576, 2583, 2581, 2581, 2580, 2575, 2573, 2576, 2578, 2580, 2584, 2586, 2583, 2583, 2582, 2577, 2572, 2574, 2572, 2572, 2576, 2580, 2581, 2584, 2586, 2587, 2584, 2586, 2583, 2582, 2582, 2581, 2583, 2588, 2590, 2592, 2591, 2588, 2588, 2588, 2585, 2583, 2582, 2584, 2582, 2583};
      }
      return _samples;
    }

  } // namespace data
} // namespace snfee
