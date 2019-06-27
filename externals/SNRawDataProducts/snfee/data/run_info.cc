// snfee/data/run_info.cc

// Ourselves:
#include <snfee/data/run_info.h>

// Third Party:
#include <boost/algorithm/string.hpp>

// This project:
// #include <snfee/data/utils.h>

namespace snfee {
  namespace data {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(run_info, "snfee::data::run_info")

    run_info::run_info() { return; }

    bool
    run_info::has_run_id() const
    {
      return _run_id_ != INVALID_RUN_ID;
    }

    int32_t
    run_info::get_run_id() const
    {
      return _run_id_;
    }

    void
    run_info::set_run_id(const int32_t run_id_)
    {
      _run_id_ = run_id_ < INVALID_RUN_ID ? INVALID_RUN_ID : run_id_;
      return;
    }

    bool
    run_info::has_run_start_time() const
    {
      return !_run_start_time_.is_not_a_date_time();
    }

    void
    run_info::set_run_start_time(const boost::posix_time::ptime& t_)
    {
      _run_start_time_ = t_;
      return;
    }

    const boost::posix_time::ptime&
    run_info::get_run_start_time() const
    {
      return _run_start_time_;
    }

    bool
    run_info::has_run_stop_time() const
    {
      return !_run_stop_time_.is_not_a_date_time();
    }

    void
    run_info::set_run_stop_time(const boost::posix_time::ptime& t_)
    {
      _run_stop_time_ = t_;
      return;
    }

    const boost::posix_time::ptime&
    run_info::get_run_stop_time() const
    {
      return _run_stop_time_;
    }

    bool
    run_info::has_run_category() const
    {
      return !_run_category_.empty();
    }

    const std::string&
    run_info::get_run_category() const
    {
      return _run_category_;
    }

    void
    run_info::set_run_category(const std::string& cat_)
    {
      _run_category_ = boost::trim_copy(cat_);
      return;
    }

    bool
    run_info::has_run_subcategory() const
    {
      return !_run_subcategory_.empty();
    }

    const std::string&
    run_info::get_run_subcategory() const
    {
      return _run_subcategory_;
    }

    void
    run_info::set_run_subcategory(const std::string& cat_)
    {
      _run_subcategory_ = boost::trim_copy(cat_);
      return;
    }

    bool
    run_info::is_complete() const
    {
      if (!has_run_id()) {
        return false;
      }
      if (!has_run_start_time()) {
        return false;
      }
      return true;
    }

    void
    run_info::reset()
    {
      _run_id_ = INVALID_RUN_ID;
      _run_start_time_ = boost::posix_time::not_a_date_time;
      _run_stop_time_ = boost::posix_time::not_a_date_time;
      _run_category_.clear();
      _run_subcategory_.clear();
      return;
    }

    // virtual
    void
    run_info::print_tree(std::ostream& out_,
                         const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Run ID           : " << _run_id_
           << std::endl;

      out_ << popts.indent << tag << "Run start time   : ";
      if (has_run_start_time()) {
        out_ << boost::posix_time::to_iso_string(_run_start_time_);
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << popts.indent << tag << "Run stop time    : ";
      if (has_run_stop_time()) {
        out_ << boost::posix_time::to_iso_string(_run_stop_time_);
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << popts.indent << tag << "Run category     : '" << _run_category_
           << "'" << std::endl;

      out_ << popts.indent << tag << "Run sub-category : '" << _run_subcategory_
           << "'" << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete         : " << std::boolalpha << is_complete()
           << std::endl;

      return;
    }

  } // namespace data
} // namespace snfee
