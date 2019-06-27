// snfee/io/rtd2root_converter.cc

// Ourselves:
#include "rtd2root_converter.h"
#include "rtd2root_data.h"

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>
// - Root:
#include <TFile.h>
#include <TTree.h>

// This project
#include <snfee/data/raw_trigger_data.h>
#include <snfee/io/multifile_data_reader.h>

namespace snfee {
  namespace io {

    struct rtd2root_converter::pimpl_type {
      std::unique_ptr<multifile_data_reader> reader;
      TFile* rfile = nullptr;
      TTree* rtree = nullptr;
      snfee::data::rtd2root_data rtd2Root;
      std::size_t nb_processed_counter = 0;
      std::size_t nb_saved_counter = 0;
    };

    rtd2root_converter::rtd2root_converter()
    {
      _pimpl_.reset(new pimpl_type);
      return;
    }

    rtd2root_converter::~rtd2root_converter()
    {
      _pimpl_.reset();
      return;
    }

    void
    rtd2root_converter::set_logging(const datatools::logger::priority l_)
    {
      _logging_ = l_;
      return;
    }

    void
    rtd2root_converter::set_config(const config_type& cfg_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Converter is already initialized!");
      _config_ = cfg_;
      return;
    }

    bool
    rtd2root_converter::is_initialized() const
    {
      return _initialized_;
    }

    void
    rtd2root_converter::initialize()
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Converter is already initialized!");
      // RTD reader:
      multifile_data_reader::config_type reader_cfg;
      if (!_config_.input_rtd_listname.empty()) {
        // Read a file containing a list of RTD input filenames:
        std::string listname = _config_.input_rtd_listname;
        datatools::fetch_path_with_env(listname);
        std::ifstream finput_list(listname.c_str());
        DT_THROW_IF(!finput_list,
                    std::logic_error,
                    "Cannot open input files list filename!");
        while (finput_list and !finput_list.eof()) {
          std::string line;
          std::getline(finput_list, line);
          boost::trim(line);
          if (line.empty()) {
            continue;
          }
          std::istringstream ins(line);
          std::string filename;
          ins >> filename >> std::ws;
          if (!filename.empty() and filename[0] != '#') {
            reader_cfg.filenames.push_back(filename);
          }
        }
      }
      // Add any explicit RTD input filenames:
      for (int ifile = 0; ifile < (int)_config_.input_rtd_filenames.size();
           ifile++) {
        reader_cfg.filenames.push_back(_config_.input_rtd_filenames[ifile]);
      }
      _pimpl_->reader.reset(new multifile_data_reader(reader_cfg));

      // Root output:
      std::string rfilename = _config_.output_root_filename;
      datatools::fetch_path_with_env(rfilename);
      _pimpl_->rfile = new TFile(rfilename.c_str(), "RECREATE");
      _pimpl_->rtree = new TTree("RTD", "SuperNEMO RTD data");

      // Root tree:
      TTree* tree = _pimpl_->rtree;

      /*
        URL: https://root.cern.ch/root/html/tutorials/tree/tree3.C.html
        ROOT currently supported types:
        C : a character string terminated by the 0 character
        B : an 8 bit signed integer   (Char_t)
        b : an 8 bit unsigned integer (UChar_t)
        S : a 16 bit signed integer   (Short_t)
        s : a 16 bit unsigned integer (UShort_t)
        I : a 32 bit signed integer   (Int_t)
        i : a 32 bit unsigned integer (UInt_t)
        F : a 32 bit floating point   (Float_t)
        D : a 64 bit floating point   (Double_t)
        L : a 64 bit signed integer   (Long64_t)
        l : a 64 bit unsigned integer (ULong64_t)
        O : a boolean (Bool_t)
      */

      // General:
      tree->Branch("run_id", &_pimpl_->rtd2Root.run_id, "run_id/I");
      tree->Branch("trigger_id", &_pimpl_->rtd2Root.trigger_id, "trigger_id/I");

      // Trig record:
      tree->Branch("has_trig", &_pimpl_->rtd2Root.has_trig, "has_trig/O");

      // Calo hit records:
      tree->Branch(
        "nb_calo_hits", &_pimpl_->rtd2Root.nb_calo_hits, "nb_calo_hits/i");
      tree->Branch(
        "calo_tdc", _pimpl_->rtd2Root.calo_tdc, "calo_tdc[nb_calo_hits]/S");
      tree->Branch("calo_crate_num",
                   _pimpl_->rtd2Root.calo_crate_num,
                   "calo_crate_num[nb_calo_hits]/S");
      tree->Branch("calo_board_num",
                   _pimpl_->rtd2Root.calo_board_num,
                   "calo_board_num[nb_calo_hits]/S");
      tree->Branch("calo_chip_num",
                   _pimpl_->rtd2Root.calo_chip_num,
                   "calo_chip_num[nb_calo_hits]/S");
      tree->Branch("calo_event_id",
                   _pimpl_->rtd2Root.calo_event_id,
                   "calo_event_id[nb_calo_hits]/s");
      tree->Branch("calo_l2_id",
                   _pimpl_->rtd2Root.calo_l2_id,
                   "calo_l2_id[nb_calo_hits]/s");
      tree->Branch(
        "calo_fcr", _pimpl_->rtd2Root.calo_fcr, "calo_fcr[nb_calo_hits]/s");
      tree->Branch("calo_has_waveforms",
                   _pimpl_->rtd2Root.calo_has_waveforms,
                   "calo_has_waveforms[nb_calo_hits]/O");
      tree->Branch("calo_waveform_start_sample",
                   _pimpl_->rtd2Root.calo_waveform_start_sample,
                   "calo_waveform_start_sample[nb_calo_hits]/s");
      tree->Branch("calo_waveform_number_of_samples",
                   _pimpl_->rtd2Root.calo_waveform_number_of_samples,
                   "calo_waveform_number_of_samples[nb_calo_hits]/s");

      tree->Branch("calo_ch0_lt",
                   _pimpl_->rtd2Root.calo_ch0_lt,
                   "calo_ch0_lt[nb_calo_hits]/O");
      tree->Branch("calo_ch0_ht",
                   _pimpl_->rtd2Root.calo_ch0_ht,
                   "calo_ch0_ht[nb_calo_hits]/O");
      tree->Branch("calo_ch0_underflow",
                   _pimpl_->rtd2Root.calo_ch0_underflow,
                   "calo_ch0_underflow[nb_calo_hits]/O");
      tree->Branch("calo_ch0_overflow",
                   _pimpl_->rtd2Root.calo_ch0_overflow,
                   "calo_ch0_overflow[nb_calo_hits]/O");
      tree->Branch("calo_ch0_baseline",
                   _pimpl_->rtd2Root.calo_ch0_baseline,
                   "calo_ch0_baseline[nb_calo_hits]/S");
      tree->Branch("calo_ch0_peak",
                   _pimpl_->rtd2Root.calo_ch0_peak,
                   "calo_ch0_peak[nb_calo_hits]/S");
      tree->Branch("calo_ch0_peak_cell",
                   _pimpl_->rtd2Root.calo_ch0_peak_cell,
                   "calo_ch0_peak_cell[nb_calo_hits]/S");
      tree->Branch("calo_ch0_charge",
                   _pimpl_->rtd2Root.calo_ch0_charge,
                   "calo_ch0_charge[nb_calo_hits]/I");
      tree->Branch("calo_ch0_rising_cell",
                   _pimpl_->rtd2Root.calo_ch0_rising_cell,
                   "calo_ch0_rising_cell[nb_calo_hits]/I");
      tree->Branch("calo_ch0_falling_cell",
                   _pimpl_->rtd2Root.calo_ch0_falling_cell,
                   "calo_ch0_rising_falling[nb_calo_hits]/I");

      tree->Branch("calo_ch1_lt",
                   _pimpl_->rtd2Root.calo_ch1_lt,
                   "calo_ch1_lt[nb_calo_hits]/O");
      tree->Branch("calo_ch1_ht",
                   _pimpl_->rtd2Root.calo_ch1_ht,
                   "calo_ch1_ht[nb_calo_hits]/O");
      tree->Branch("calo_ch1_underflow",
                   _pimpl_->rtd2Root.calo_ch1_underflow,
                   "calo_ch1_underflow[nb_calo_hits]/O");
      tree->Branch("calo_ch1_overflow",
                   _pimpl_->rtd2Root.calo_ch1_overflow,
                   "calo_ch1_overflow[nb_calo_hits]/O");
      tree->Branch("calo_ch1_baseline",
                   _pimpl_->rtd2Root.calo_ch1_baseline,
                   "calo_ch1_baseline[nb_calo_hits]/S");
      tree->Branch("calo_ch1_peak",
                   _pimpl_->rtd2Root.calo_ch1_peak,
                   "calo_ch1_peak[nb_calo_hits]/S");
      tree->Branch("calo_ch1_peak_cell",
                   _pimpl_->rtd2Root.calo_ch1_peak_cell,
                   "calo_ch1_peak_cell[nb_calo_hits]/S");
      tree->Branch("calo_ch1_charge",
                   _pimpl_->rtd2Root.calo_ch1_charge,
                   "calo_ch1_charge[nb_calo_hits]/I");
      tree->Branch("calo_ch1_rising_cell",
                   _pimpl_->rtd2Root.calo_ch1_rising_cell,
                   "calo_ch1_rising_cell[nb_calo_hits]/I");
      tree->Branch("calo_ch1_falling_cell",
                   _pimpl_->rtd2Root.calo_ch1_falling_cell,
                   "calo_ch1_rising_falling[nb_calo_hits]/I");

      tree->Branch("calo_ch0_waveform",
                   _pimpl_->rtd2Root.calo_ch0_waveform,
                   "calo_ch0_waveform[nb_calo_hits][1024]/S");
      tree->Branch("calo_ch1_waveform",
                   _pimpl_->rtd2Root.calo_ch1_waveform,
                   "calo_ch1_waveform[nb_calo_hits][1024]/S");

      // Tracker hit records:
      tree->Branch("nb_tracker_hits",
                   &_pimpl_->rtd2Root.nb_tracker_hits,
                   "nb_tracker_hits/i");
      tree->Branch("tracker_crate_num",
                   _pimpl_->rtd2Root.tracker_crate_num,
                   "tracker_crate_num[nb_tracker_hits]/S");
      tree->Branch("tracker_board_num",
                   _pimpl_->rtd2Root.tracker_board_num,
                   "tracker_board_num[nb_tracker_hits]/S");
      tree->Branch("tracker_chip_num",
                   _pimpl_->rtd2Root.tracker_chip_num,
                   "tracker_chip_num[nb_tracker_hits]/S");
      tree->Branch("tracker_channel_num",
                   _pimpl_->rtd2Root.tracker_channel_num,
                   "tracker_channel_num[nb_tracker_hits]/S");
      tree->Branch("tracker_channel_category",
                   _pimpl_->rtd2Root.tracker_channel_category,
                   "tracker_channel_category[nb_tracker_hits]/S");
      tree->Branch("tracker_timestamp_category",
                   _pimpl_->rtd2Root.tracker_timestamp_category,
                   "tracker_timestamp_category[nb_tracker_hits]/S");
      tree->Branch("tracker_timestamp",
                   _pimpl_->rtd2Root.tracker_timestamp,
                   "tracker_timestamp[nb_tracker_hits]/l");

      _initialized_ = true;
      return;
    }

    void
    rtd2root_converter::run()
    {
      DT_THROW_IF(
        !is_initialized(), std::logic_error, "Converter is not initialized!");

      // Working RTD object:
      snfee::data::raw_trigger_data rtd;

      // Working RTD->ROOT conversion structure:
      snfee::data::rtd2root_data rtd2Root;

      // Main loop on RTD input:
      _pimpl_->nb_processed_counter = 0;
      _pimpl_->nb_saved_counter = 0;
      while (_pimpl_->reader->has_record_tag()) {
        if (_pimpl_->reader->record_tag_is(
              snfee::data::raw_trigger_data::SERIAL_TAG)) {
          _pimpl_->reader->load(rtd);
          if (datatools::logger::is_debug(_logging_)) {
            boost::property_tree::ptree options;
            options.put("title", "Raw trigger data (RTD) record: ");
            options.put("indent", "[debug] ");
            rtd.print_tree(std::clog, options);
          }
          _pimpl_->nb_processed_counter++;
          bool export_rtd = true;
          if (export_rtd) {
            // ROOT export:
            snfee::data::rtd2root_data::export_to_root(rtd, _pimpl_->rtd2Root);
            _pimpl_->rtree->Fill();
            _pimpl_->nb_saved_counter++;
          }
        } else {
          DT_THROW(std::logic_error, "Unexpected record tag!");
        }
        if (_pimpl_->nb_saved_counter == _config_.max_total_records) {
          break;
        }
      }
      if (datatools::logger::is_debug(_logging_)) {
        _pimpl_->rtree->Print();
      }
      return;
    }

    void
    rtd2root_converter::terminate()
    {
      DT_THROW_IF(
        !is_initialized(), std::logic_error, "Converter is not initialized!");
      _initialized_ = false;
      _pimpl_->rfile->Write();
      _pimpl_->rfile->Close();
      _pimpl_->reader.reset();
      return;
    }

  } // namespace io
} // namespace snfee
