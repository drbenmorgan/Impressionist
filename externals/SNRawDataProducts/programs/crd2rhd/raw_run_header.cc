// snfee/io/raw_run_header.cc

// Ourselves:
#include "raw_run_header.h"

// Standard library:
#include <iomanip>
#include <iostream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/version_id.h>

// This project:

namespace snfee {
  namespace io {

    raw_run_header::raw_run_header()
    {
      _reset_();
      return;
    }

    raw_run_header::~raw_run_header() { return; }

    bool
    raw_run_header::is_complete() const
    {
      if (software_version.empty())
        return false;
      if (!datatools::is_valid(unix_time))
        return false;
      if (date.empty())
        return false;
      if (time.empty())
        return false;
      if (data_type.empty())
        return false;
      return true;
    }

    void
    raw_run_header::set_software_version(const std::string& sw_version_)
    {
      std::string ver = sw_version_;
      if (ver.length() > 0 and (ver[0] == 'v' or ver[0] == 'V')) {
        ver = ver.substr(1);
      }
      datatools::version_id ver_id;
      DT_THROW_IF(!ver_id.from_string(ver),
                  std::logic_error,
                  "Invalid software version ID '" << ver << "'!");
      DT_THROW_IF(!ver_id.has_major(),
                  std::logic_error,
                  "Missing major software version number in '" << ver << "'!");
      DT_THROW_IF(!ver_id.has_minor(),
                  std::logic_error,
                  "Missing minor software version number in '" << ver << "'!");
      software_major_version = ver_id.get_major();
      software_minor_version = ver_id.get_minor();
      software_version = sw_version_;
      return;
    }

    void
    raw_run_header::make(const std::string& sw_version_,
                         const double unix_time_,
                         const std::string& date_,
                         const std::string& time_,
                         const std::string& data_type_)
    {
      set_software_version(sw_version_);
      unix_time = unix_time_;
      date = date_;
      time = time_;
      data_type = data_type_;
      return;
    }

    void
    raw_run_header::reset()
    {
      _reset_();
      return;
    }

    void
    raw_run_header::_reset_()
    {
      return;
    }

    // virtual
    void
    raw_run_header::print_tree(
      std::ostream& out_,
      const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Software version       : '"
           << software_version << "'" << std::endl;

      out_ << popts.indent << tag << "Software major version : '"
           << software_major_version << "'" << std::endl;

      out_ << popts.indent << tag << "Software minor version : '"
           << software_minor_version << "'" << std::endl;

      out_ << popts.indent << tag << "Unix time              : ["
           << std::setprecision(15) << unix_time << " s]" << std::endl;

      out_ << popts.indent << tag << "Date                   : [" << date << "]"
           << std::endl;

      out_ << popts.indent << tag << "Time                   : [" << time << "]"
           << std::endl;

      out_ << popts.indent << tag << "Data type              : '" << data_type
           << "'" << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Completeness           : " << std::boolalpha << is_complete()
           << std::endl;

      return;
    }

  } // namespace io
} // namespace snfee
