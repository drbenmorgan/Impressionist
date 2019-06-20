#ifndef GENBBPRIMARYEVENT_HH
#define GENBBPRIMARYEVENT_HH

// Art's products cannot have underscores in the friendly
// name. Workaround this for Bayeux's genbb::primary_event
// type by direct inheritance.

#include "bayeux/genbb_help/primary_event.h"

namespace snemo {
  class GenBBPrimaryEvent : public genbb::primary_event {
  public:
    GenBBPrimaryEvent() = default;
    virtual ~GenBBPrimaryEvent(){};
    GenBBPrimaryEvent(const genbb::primary_event& p) : genbb::primary_event(p)
    {}
  };
} // namespace snemo

#endif // GENBBPRIMARYEVENT_HH
