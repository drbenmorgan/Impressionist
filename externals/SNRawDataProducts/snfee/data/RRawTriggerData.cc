// This project:
#include "RRawTriggerData.h"

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/data/utils.h>
#include <snfee/model/feb_constants.h>
#include <snfee/utils.h>

namespace snfee {
  namespace data {
    DATATOOLS_SERIALIZATION_IMPLEMENTATION(RRawTriggerData,
                                           "snfee::data::RRawTriggerData")

    bool
    RRawTriggerData::isComplete() const
    {
      if (!hasRunID()) {
        return false;
      }
      if (!hasTriggerID()) {
        return false;
      }
      if (!trigger.is_complete()) {
        return false;
      }
      if (caloRecords.size() + trackerRecords.size() == 0) {
        return false;
      }
      for (const auto& chit : caloRecords) {
        if (!chit.is_complete()) {
          return false;
        }
      }
      for (const auto& thit : trackerRecords) {
        if (!thit.is_complete()) {
          return false;
        }
      }
      return true;
    }

    bool
    RRawTriggerData::hasRunID() const
    {
      return runID != INVALID_RUN_ID;
    }

    int32_t
    RRawTriggerData::getRunID() const
    {
      return runID;
    }

    bool
    RRawTriggerData::hasTriggerID() const
    {
      return triggerID != INVALID_TRIGGER_ID;
    }

    int32_t
    RRawTriggerData::getTriggerID() const
    {
      return triggerID;
    }

    const TriggerRecord&
    RRawTriggerData::getTriggerRecord() const
    {
      return trigger;
    }

    const CaloRecordCollection&
    RRawTriggerData::getCaloRecords() const
    {
      return caloRecords;
    }

    const TrackerRecordCollection&
    RRawTriggerData::getTrackerRecords() const
    {
      return trackerRecords;
    }

    // virtual
    void
    RRawTriggerData::print_tree(
      std::ostream& out_,
      const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Run ID : " << runID << std::endl;
      out_ << popts.indent << tag << "Trigger ID : " << triggerID << std::endl;

      // Trigger
      out_ << popts.indent << tag << "Trigger record : ";
      if (!trigger.is_complete()) {
        out_ << "<not complete>";
      }
      out_ << std::endl;

      std::ostringstream indent2;
      indent2 << popts.indent;
      indent2 << skip_tag;
      boost::property_tree::ptree trig_opts;
      trig_opts.put("indent", indent2.str());
      trigger.print_tree(out_, trig_opts);

      // Calorimeter records
      out_ << popts.indent << tag
           << "Calorimeter hit records : " << caloRecords.size() << std::endl;
      for (std::size_t i{0}; i < caloRecords.size(); ++i) {
        std::ostringstream indent2;
        indent2 << popts.indent << skip_tag;
        out_ << popts.indent << skip_tag;
        if (i + 1 == caloRecords.size()) {
          out_ << last_tag;
          indent2 << last_skip_tag;
        } else {
          out_ << tag;
          indent2 << skip_tag;
        }
        out_ << "Calo hit record #" << i << " : " << std::endl;
        boost::property_tree::ptree chit_opts;
        chit_opts.put("indent", indent2.str());
        caloRecords[i].print_tree(out_, chit_opts);
      }

      // Tracker records
      out_ << popts.indent << tag
           << "Tracker hit records : " << trackerRecords.size() << std::endl;
      for (std::size_t i{0}; i < trackerRecords.size(); ++i) {
        std::ostringstream indent2;
        indent2 << popts.indent << skip_tag;
        out_ << popts.indent << skip_tag;
        if (i + 1 == trackerRecords.size()) {
          out_ << last_tag;
          indent2 << last_skip_tag;
        } else {
          out_ << tag;
          indent2 << skip_tag;
        }
        out_ << "Tracker hit record #" << i << " : " << std::endl;
        boost::property_tree::ptree chit_opts;
        chit_opts.put("indent", indent2.str());
        trackerRecords[i].print_tree(out_, chit_opts);
      }

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete : " << std::boolalpha << isComplete() << std::endl;
    }
  } // namespace data
} // namespace snfee
