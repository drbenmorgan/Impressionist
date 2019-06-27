// This project:
#include <snfee/data/calo_waveform_drawer.h>

// Standard library:
#include <iomanip>

// Third party:
// - Bayeux:
#include <bayeux/datatools/temporary_files.h>
#include <bayeux/geomtools/geomtools_config.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <bayeux/geomtools/gnuplot_i.h>
#include <bayeux/geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

namespace snfee {
  namespace data {

    calo_waveform_drawer::calo_waveform_drawer(const std::string & tmp_dir_)
    {
      _tmp_dir_ = tmp_dir_;
      return;
    }
  
    calo_waveform_drawer::calo_waveform_drawer(const double time_min_ns_,
                                               const double time_max_ns_,
                                               const double amp_min_mV_,
                                               const double amp_max_mV_,
                                               const bool auto_range_time_,
                                               const bool auto_range_voltage_,
                                               const std::string & tmp_dir_)
    {
      _tmp_dir_ = tmp_dir_;
      _time_min_ns_ = time_min_ns_;
      _time_max_ns_ = time_max_ns_;
      _amp_min_mV_ = amp_min_mV_;
      _amp_max_mV_ = amp_max_mV_;
      _auto_range_time_ = auto_range_time_;
      _auto_range_voltage_ = auto_range_voltage_;
      return;
    }

    void calo_waveform_drawer::set_range_time(const double time_min_ns_,
                                              const double time_max_ns_)
    {
      _time_min_ns_ = time_min_ns_;
      _time_max_ns_ = time_max_ns_;
      return;
    }

    void calo_waveform_drawer::set_range_voltage(const double amp_min_mV_,
                                                 const double amp_max_mV_)
    {
      _amp_min_mV_ = amp_min_mV_;
      _amp_max_mV_ = amp_max_mV_;
      return;
    }

    void calo_waveform_drawer::set_auto_range_time(const bool auto_range_time_)
    {
      _auto_range_time_ = auto_range_time_;
      return;
    }
      
    void calo_waveform_drawer::set_auto_range_voltage(const bool auto_range_voltage_)
    {
      _auto_range_voltage_ = auto_range_voltage_;
      return;
    }

    void calo_waveform_drawer::set_tmp_dir(const std::string & tmp_dir_)
    {
      _tmp_dir_ = tmp_dir_;
      return;
    }

    void calo_waveform_drawer::draw(const calo_waveform_info & wfi_,
                                    const std::string & title_,
                                    const std::string & id_) const
    {
      datatools::temp_file ftmp;
      ftmp.set_remove_at_destroy(true);
      ftmp.create(_tmp_dir_, "snfee_data_calo_waveform_drawer-temp_" + id_ + "_");
      std::string ftmp_path = ftmp.get_filename();
      const mygsl::tabulated_function & tf = wfi_.waveform.get_tabfunc(); 
      tf.print_points(ftmp.out());
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      std::string title = title_;
      if (title_.empty()) {
        title = "Waveform for calo channel [" + id_ + "]"; 
      }
      double time_min_ns = _time_min_ns_;
      double time_max_ns = _time_max_ns_;
      double amp_min_mV  = _amp_min_mV_;
      double amp_max_mV  = _amp_max_mV_;
      if (datatools::logger::is_debug(logging)) {
        wfi_.print(std::cerr, "WFI : ", "[debug] ");
      }
      if (_auto_range_time_) {
        DT_LOG_DEBUG(logging, "Auto range time");
        if (wfi_.has_peak) {      
          DT_LOG_DEBUG(logging, " -> using peak window");
          time_min_ns = std::max(_time_min_ns_, wfi_.baseline.baseline_duration_ns - 25.0);
          time_max_ns = std::min(_time_max_ns_, wfi_.peak.time_tail_10_percent_ns + 40.0);
        }
      }
      if (_auto_range_voltage_) {
        DT_LOG_DEBUG(logging, "Auto range voltage");
        if (wfi_.has_peak) {
          DT_LOG_DEBUG(logging, " -> using peak window");
          double margin_mV = 30 * wfi_.baseline.noise_mV;
          amp_max_mV = wfi_.baseline.baseline_mV + margin_mV;
          amp_min_mV = wfi_.baseline.baseline_mV + wfi_.peak.amplitude_mV - margin_mV;
        }
      }
      std::string terminal_id = "wxt";
      terminal_id = "x11";
      g1.cmd("set terminal " + terminal_id);
      g1.cmd("set encoding utf8");
      g1.cmd("set title '" + title + "'");
      g1.cmd("set grid");
      g1.cmd("set xlabel 'Time (ns)'");
      g1.cmd("set ylabel 'Amplitude (mV)'");
      g1.cmd("set xrange [" + std::to_string(time_min_ns) + ":" + std::to_string(time_max_ns) + "]");
      g1.cmd("set yrange [" + std::to_string(amp_min_mV)  + ":" + std::to_string(amp_max_mV) + "]");
      {
        double sigma_mv = 0.0;
        if (wfi_.has_baseline) {
          sigma_mv = wfi_.baseline.noise_mV;
        }

        std::string sample_color = "rgb '#0090ff'";
        std::string sample_error_color = "rgb '#00b0ff'";
        
        std::ostringstream plot_cmd;
        double sample_point_size = 0.5;
        plot_cmd << "plot ";
        plot_cmd << "'" << ftmp.get_filename()
                 << "' notitle with lines lt " << sample_color << " lw 1 ";
        plot_cmd << ", '" << ftmp.get_filename()
                 << "' notitle with points pt 4 ps " << sample_point_size << " lt " << sample_color << " ";
        plot_cmd << ", '" << ftmp.get_filename()
                 << "' using 1:2:(" << sigma_mv << ") notitle with errorlines pt 0 lt " << sample_error_color << " ";
        
        if (wfi_.has_baseline) {

          std::string baseline_color = "rgb '#008000'";
          double bl  = wfi_.baseline.baseline_mV;
          double fw_bl = datatools::invalid_real();
          if (wfi_.has_fw_data) {
            fw_bl = wfi_.fw_data.baseline_mV;
          }
          {
            double tr1 = wfi_.baseline.baseline_start_ns;
            double tr2 = tr1 + wfi_.baseline.baseline_duration_ns;
            double vert_factor_top = 5.0;
            double fr1 = bl - vert_factor_top * wfi_.baseline.noise_mV;
            double fr2 = bl + vert_factor_top * wfi_.baseline.noise_mV;

            g1.cmd("bl(x)=" + std::to_string(bl));
            plot_cmd << ", bl(x) notitle with lines lt " << baseline_color << " lw 2";

            if (datatools::is_valid(fw_bl)) {
              g1.cmd("fwbl(x)=" + std::to_string(fw_bl));
              plot_cmd << ", fwbl(x) notitle with lines lt " << baseline_color << " lw 1";
            }
            
            std::ostringstream baseline_outs;
            baseline_outs << "set object 1 rectangle "
                          << "from " << tr1 << "," << fr1
                          << " to " << tr2 << "," << fr2
                          << " fs empty border " << baseline_color << " dt 4"
                          << std::endl;

            baseline_outs << "set label 'b = " << std::setprecision(4) << bl;
            // baseline_outs << " +/- " << std::setprecision(4) << wfi_.baseline.sigma_baseline_mV;
            baseline_outs << " mV ";
            baseline_outs << "' "
                          << "at " << tr2 << "," << bl + vert_factor_top * wfi_.baseline.noise_mV
                          << " right front textcolor " << baseline_color << " offset -1,+1 "
                          << std::endl;
            if (datatools::is_valid(fw_bl)) {
              baseline_outs << "set label '[FW : b = " << std::setprecision(4) << fw_bl;
              baseline_outs << " mV]";
              baseline_outs << "' "
                            << "at " << tr2 << "," << bl + vert_factor_top * wfi_.baseline.noise_mV
                            << " right front textcolor " << baseline_color << " offset -1,+2 "
                            << std::endl;
            }
   
            g1.cmd(baseline_outs.str());
          }
          
          if (wfi_.has_peak) {
            double hskip_ns = 5.0;
            std::string peak_color = "rgb '#ff4000'";
            if (wfi_.peak.time_head_10_percent_cell >= 0) {
              std::ostringstream vline_outs;
              
              vline_outs << "set arrow from " << wfi_.peak.time_head_10_percent_ns << ',' << bl
                         << " to " << wfi_.peak.time_head_10_percent_ns << ','
                         << wfi_.peak.amplitude_mV * 0.1
                         << " nohead lw 1 linecolor " << peak_color << " dt 2 "
                         << std::endl;
              
              vline_outs << "set arrow from " << (wfi_.peak.time_head_10_percent_ns - hskip_ns)
                         << ',' << (wfi_.peak.amplitude_mV * 0.1 + bl)
                         << " to " << (wfi_.peak.time_tail_10_percent_ns + 1.0) << ','
                         << wfi_.peak.amplitude_mV * 0.1 + bl
                         << " nohead lw 1 linecolor " << peak_color << " dt 2 "
                         << std::endl;

              vline_outs << "set label '(10 %)' at " << (wfi_.peak.time_head_10_percent_ns - hskip_ns) << ","
                         << wfi_.peak.amplitude_mV * 0.1 + bl << " right front textcolor " << peak_color << " offset -1,0 "
                         << std::endl;

              g1.cmd(vline_outs.str());
            }
            
            if (wfi_.peak.time_head_90_percent_cell >= 0) {
              std::ostringstream vline_outs;
              
              vline_outs << "set arrow from " << wfi_.peak.time_head_90_percent_ns << ',' << bl
                         << " to " << wfi_.peak.time_head_90_percent_ns << ','
                         << wfi_.peak.amplitude_mV * 0.9
                         << " nohead lw 1 linecolor " << peak_color << " dt 2 "
                         << std::endl;
              
              vline_outs << "set arrow from " << (wfi_.peak.time_head_90_percent_ns - hskip_ns)
                         << ',' << (wfi_.peak.amplitude_mV * 0.9 + bl)
                         << " to " << (wfi_.peak.time_tail_90_percent_ns + 1.0) << ','
                         << wfi_.peak.amplitude_mV * 0.9 + bl
                         << " nohead lw 1 linecolor " << peak_color << " dt 2"
                         << std::endl;
              
              vline_outs << "set label '(90 %)' at " << (wfi_.peak.time_head_90_percent_ns - hskip_ns) << ","
                         << wfi_.peak.amplitude_mV * 0.9 + bl << " right front textcolor " << peak_color << " offset -1,0 "
                         << std::endl;
              g1.cmd(vline_outs.str());
            }
          
            if (wfi_.peak.time_tail_10_percent_cell >= 0) {
              std::ostringstream vline_outs;
              vline_outs << "set arrow from " << wfi_.peak.time_tail_10_percent_ns << ',' << bl
                         << " to " << wfi_.peak.time_tail_10_percent_ns << ','
                         << wfi_.peak.amplitude_mV * 0.1
                         << " nohead lw 1 linecolor " << peak_color << " dt 2";
              g1.cmd(vline_outs.str());
            }
            
            if (wfi_.peak.time_tail_90_percent_cell >= 0) {
              
              std::ostringstream vline_outs;

              vline_outs << "set arrow from " << wfi_.peak.time_tail_90_percent_ns << ',' << bl
                         << " to " << wfi_.peak.time_tail_90_percent_ns << ','
                         << wfi_.peak.amplitude_mV * 0.9
                         << " nohead lw 1 linecolor " << peak_color << " dt 2";

              g1.cmd(vline_outs.str());
            }
            
            if (wfi_.peak.time_cell >= 0) {
              std::ostringstream vline_outs;

              double lhskip_ns = 10.0;
              vline_outs << "set arrow from "
                         << wfi_.peak.time_ns << ',' << bl << " to "
                         << wfi_.peak.time_ns << ',' << wfi_.peak.amplitude_mV
                         << " nohead lw 1 linecolor " << peak_color << " dt 2"
                         << std::endl;
              
              vline_outs << "set arrow from "
                         << (wfi_.peak.time_ns - 1.0)            << ',' << (wfi_.peak.amplitude_mV + bl) << " to " 
                         << (wfi_.peak.time_ns + lhskip_ns)      << ',' << (wfi_.peak.amplitude_mV + bl)
                         << " nohead lw 1 linecolor " << peak_color << " dt 2 "
                         << std::endl;

              vline_outs << "set label 'A_{peak} = " << std::setprecision(5) << wfi_.peak.amplitude_mV << " mV"
                         << ", t_{peak} = " << std::setprecision(6) << wfi_.peak.time_ns  << " ns"
                         << "' at "
                         << (wfi_.peak.time_ns + lhskip_ns) << ',' << (wfi_.peak.amplitude_mV + bl)
                         << " left front textcolor " << peak_color << " offset +1,0 "
                         << std::endl;

              if (wfi_.has_fw_data and datatools::is_valid(wfi_.fw_data.peak_mV)) {
                vline_outs << "set label '[FW : A_{peak} = " << std::setprecision(5) << wfi_.fw_data.peak_mV << " mV"
                           << ", t_{peak} = " << std::setprecision(6) << wfi_.peak.time_ns  << " ns]"
                           << "' at "
                           << (wfi_.peak.time_ns + lhskip_ns) << ',' << (wfi_.peak.amplitude_mV + bl)
                           << " left front textcolor " << peak_color << " offset +1,-2 "
                           << std::endl;
              }
              
              g1.cmd(vline_outs.str());
            }
            
            if (wfi_.has_cfd) {
              std::ostringstream cfd_amp_fraction_outs;
              std::string cfd_color = "rgb '#af00af'";
              double cfd_amp = wfi_.cfd.fraction * wfi_.peak.amplitude_mV;
              // double hskip_ns = 5.0;
              cfd_amp_fraction_outs << "set label 't@^{" << std::setprecision(2) << 100*wfi_.cfd.fraction << " %}_{CFD} = "
                                    << std::setprecision(6) << wfi_.cfd.time_ns << " ns' at "
                                    << (wfi_.cfd.time_ns - hskip_ns) << "," << cfd_amp + wfi_.baseline.baseline_mV
                                    << " right front textcolor " << cfd_color << " offset -1,0 "
                                    << std::endl;
              
              cfd_amp_fraction_outs << "set arrow from " << (wfi_.cfd.time_ns - hskip_ns) << ','
                                    << cfd_amp + wfi_.baseline.baseline_mV
                                    << " to " << (wfi_.cfd.time_ns + 1.0) << ','
                                    << cfd_amp + wfi_.baseline.baseline_mV
                                    << " nohead lt " << cfd_color << " lw 1 dt 2"
                                    << std::endl;

              cfd_amp_fraction_outs << "set arrow from "
                                    << wfi_.cfd.time_ns << ',' << wfi_.baseline.baseline_mV << " to "
                                    << wfi_.cfd.time_ns << ',' << cfd_amp + wfi_.baseline.baseline_mV
                                    << " nohead lt " << cfd_color << " lw 1 dt 1"
                                    << std::endl;

              g1.cmd(cfd_amp_fraction_outs.str());
           
               if (wfi_.has_charge) { 
                 std::ostringstream charge_outs;
                 std::string charge_color = "rgb '#00408f'";
                 double vert_factor_top = 15.0;
                 double vert_factor_bot =  5.0;
                 charge_outs << "set arrow from " << (wfi_.charge.start_time_ns) << ','
                             << wfi_.baseline.baseline_mV + vert_factor_top * wfi_.baseline.noise_mV
                             << " to " << (wfi_.charge.start_time_ns) << ','
                             << wfi_.baseline.baseline_mV + vert_factor_bot * wfi_.baseline.noise_mV
                             << " head lt " << charge_color << " lw 1"
                             << std::endl;
                 charge_outs << "set arrow from " << (wfi_.charge.stop_time_ns) << ','
                             << wfi_.baseline.baseline_mV + vert_factor_top * wfi_.baseline.noise_mV
                             << " to " << (wfi_.charge.stop_time_ns) << ','
                             << wfi_.baseline.baseline_mV + vert_factor_bot * wfi_.baseline.noise_mV
                             << " head lt " << charge_color << " lw 1"
                             << std::endl;
                 charge_outs << "set arrow from " << (wfi_.charge.start_time_ns) << ','
                             << wfi_.baseline.baseline_mV + vert_factor_top * wfi_.baseline.noise_mV
                             << " to " << (wfi_.charge.stop_time_ns) << ','
                             << wfi_.baseline.baseline_mV + vert_factor_top * wfi_.baseline.noise_mV
                             << " nohead lt " << charge_color << " lw 1"
                             << std::endl;
                 double charge_time_pos = wfi_.charge.start_time_ns;
                 charge_outs << "set label 'Q = " << wfi_.charge.charge_nVs << " nV.s' "
                             << "at " << (charge_time_pos) << "," << wfi_.baseline.baseline_mV + vert_factor_top * wfi_.baseline.noise_mV
                             << " left front textcolor " << charge_color << " offset +1,+1 "
                             << std::endl;
                 if (wfi_.has_fw_data and datatools::is_valid(wfi_.fw_data.charge_nVs)) {
                   charge_outs << "set label '[FW : Q = " << wfi_.fw_data.charge_nVs << " nV.s]' "
                               << "at " << (charge_time_pos) << "," << wfi_.baseline.baseline_mV + vert_factor_top * wfi_.baseline.noise_mV
                               << " left front textcolor " << charge_color << " offset +1,+2 "
                               << std::endl;
                 }
                 g1.cmd(charge_outs.str());
               } // wfi_.has_charge
               
            } // wfi_.has_cfd
           
          } // wfi_.has_peak
          
        } // wfi_.has_baseline
        
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      return;
    }

  } // namespace data
} // namespace snfee
