#ifndef STEP_HIT_HH
#define STEP_HIT_HH

// Art's products cannot have underscores in the friendly
// name. Workaround this for Bayeux's mctools::base_step_hit
// type by direct inheritance.

#include "bayeux/mctools/base_step_hit.h"

namespace snemo {
  class BaseStepHit : public mctools::base_step_hit {
  public:
    BaseStepHit() = default;
    virtual ~BaseStepHit(){};
    BaseStepHit(const mctools::base_step_hit& p) : mctools::base_step_hit(p) {}
  };
  using StepHit = BaseStepHit;
} // namespace snemo

#endif // STEP_HIT_HH
