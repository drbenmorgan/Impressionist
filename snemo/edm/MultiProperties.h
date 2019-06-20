#ifndef SNEMO_MULTI_PROPERTIES_HH
#define SNEMO_MULTI_PROPERTIES_HH

// Art's products cannot have underscores in the friendly
// name. Workaround this for Bayeux's datatools::multi_properties
// type by direct inheritance.

#include "bayeux/datatools/multi_properties.h"

namespace snemo {
  class MultiProperties : public datatools::multi_properties {
  public:
    MultiProperties() = default;
    virtual ~MultiProperties(){};
    MultiProperties(const datatools::multi_properties& p) : datatools::multi_properties(p) {}
  };
} // namespace snemo

#endif // SNEMO_MULTI_PROPERTIES_HH
