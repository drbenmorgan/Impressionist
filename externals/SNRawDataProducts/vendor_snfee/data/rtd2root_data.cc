// This project:
#include <snfee/data/rtd2root_data.h>


// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snfee {
  namespace data {

    void rtd2root_data::clear()
    {
      run_id     = INVALID_RUN_ID;
      trigger_id = INVALID_TRIGGER_ID;
      has_trig   = false;
      nb_calo_hits    = 0;
      nb_tracker_hits = 0; 
      return;
    }

    // static
    void rtd2root_data::export_to_root(const raw_trigger_data & in_,
                                       rtd2root_data & out_)
    {
      out_.clear();
      
      out_.run_id     = in_.get_run_id();
      out_.trigger_id = in_.get_trigger_id();

      // Trigger record:
      if (in_.has_trig()) {
        out_.has_trig = true;
      }

      // Calorimeter hit records:
      out_.nb_calo_hits = in_.get_calo_hits().size();
      int calo_count = 0;
      for (const auto & hchit : in_.get_calo_hits()) {
        const calo_hit_record & chit = *hchit;
        
        out_.calo_tdc[calo_count]       = chit.get_tdc();
        out_.calo_crate_num[calo_count] = chit.get_crate_num();
        out_.calo_board_num[calo_count] = chit.get_board_num();
        out_.calo_chip_num[calo_count]  = chit.get_chip_num();
        out_.calo_event_id[calo_count]  = chit.get_event_id();
        out_.calo_l2_id[calo_count]     = chit.get_l2_id();
        out_.calo_fcr[calo_count]       = chit.get_fcr();
        out_.calo_has_waveforms[calo_count]              = chit.has_waveforms();
        out_.calo_waveform_start_sample[calo_count]      = chit.get_waveform_start_sample();
        out_.calo_waveform_number_of_samples[calo_count] = chit.get_waveform_number_of_samples();
        
        out_.calo_ch0_lt[calo_count]           = chit.get_channel_data(0).is_lt();
        out_.calo_ch0_ht[calo_count]           = chit.get_channel_data(0).is_ht();
        out_.calo_ch0_underflow[calo_count]    = chit.get_channel_data(0).is_underflow();
        out_.calo_ch0_overflow[calo_count]     = chit.get_channel_data(0).is_overflow();
        out_.calo_ch0_baseline[calo_count]     = chit.get_channel_data(0).get_baseline();
        out_.calo_ch0_peak[calo_count]         = chit.get_channel_data(0).get_peak();
        out_.calo_ch0_peak_cell[calo_count]    = chit.get_channel_data(0).get_peak_cell();
        out_.calo_ch0_charge[calo_count]       = chit.get_channel_data(0).get_charge();
        out_.calo_ch0_rising_cell[calo_count]  = chit.get_channel_data(0).get_rising_cell();
        out_.calo_ch0_falling_cell[calo_count] = chit.get_channel_data(0).get_falling_cell();
       
        out_.calo_ch1_lt[calo_count]           = chit.get_channel_data(1).is_lt();
        out_.calo_ch1_ht[calo_count]           = chit.get_channel_data(1).is_ht();
        out_.calo_ch1_underflow[calo_count]    = chit.get_channel_data(1).is_underflow();
        out_.calo_ch1_overflow[calo_count]     = chit.get_channel_data(1).is_overflow();
        out_.calo_ch1_baseline[calo_count]     = chit.get_channel_data(1).get_baseline();
        out_.calo_ch1_peak[calo_count]         = chit.get_channel_data(1).get_peak();
        out_.calo_ch1_peak_cell[calo_count]    = chit.get_channel_data(1).get_peak_cell();
        out_.calo_ch1_charge[calo_count]       = chit.get_channel_data(1).get_charge();
        out_.calo_ch1_rising_cell[calo_count]  = chit.get_channel_data(1).get_rising_cell();
        out_.calo_ch1_falling_cell[calo_count] = chit.get_channel_data(1).get_falling_cell();

        const snfee::data::calo_hit_record::waveforms_record & waveforms = chit.get_waveforms();
        for (int isample = 0; isample < chit.get_waveform_number_of_samples(); isample++) {
          out_.calo_ch0_waveform[calo_count][isample] = waveforms.get_adc(isample, 0);
          out_.calo_ch1_waveform[calo_count][isample] = waveforms.get_adc(isample, 1);
        }
         
        calo_count++;
      } 
      
      // Tracker hit records:
      out_.nb_tracker_hits = in_.get_tracker_hits().size();
      int tracker_count = 0;
      for (const auto & hthit : in_.get_tracker_hits()) {
        const tracker_hit_record & thit = *hthit;

        out_.tracker_crate_num[tracker_count] = thit.get_crate_num();
        out_.tracker_board_num[tracker_count] = thit.get_board_num();
        out_.tracker_chip_num[tracker_count] = thit.get_chip_num();
        out_.tracker_channel_num[tracker_count] = thit.get_channel_num();
        out_.tracker_channel_category[tracker_count] = (int16_t) thit.get_channel_category();
        out_.tracker_timestamp_category[tracker_count] = (int16_t) thit.get_timestamp_category();
        out_.tracker_timestamp[tracker_count] = thit.get_timestamp();
        
        tracker_count++;
      } 
      
      return;
    }

  } // namespace data
} // namespace snfee
