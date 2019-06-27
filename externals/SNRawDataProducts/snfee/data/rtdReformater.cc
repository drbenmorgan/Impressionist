#include "snfee/data/rtdReformater.h"

namespace snfee {
  namespace data {

    snfee::data::RRawTriggerData
    rtdOnlineToOffline(const snfee::data::raw_trigger_data& rawRTD)
    {
      int32_t runID{rawRTD.get_run_id()};
      int32_t triggerID{rawRTD.get_trigger_id()};

      snfee::data::TriggerRecord triggerRec{};
      if (rawRTD.has_trig()) {
        triggerRec = *(rawRTD.get_trig());
      }

      const auto& inputCaloRec = rawRTD.get_calo_hits();
      snfee::data::CaloRecordCollection outputCaloRec{};

      for (const auto& crec : inputCaloRec) {
        outputCaloRec.push_back(*crec);
      }

      const auto& inputTrackerRec = rawRTD.get_tracker_hits();
      snfee::data::TrackerRecordCollection outputTrackerRec;

      for (const auto& crec : inputTrackerRec) {
        outputTrackerRec.push_back(*crec);
      }

      return snfee::data::RRawTriggerData{
        runID, triggerID, triggerRec, outputCaloRec, outputTrackerRec};
    }

  } // namespace data
} // namespace snfee
