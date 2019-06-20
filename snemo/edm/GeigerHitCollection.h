#ifndef GEIGERHITCOLLECTION_HH
#define GEIGERHITCOLLECTION_HH

/// Rough model of collection of Geiger hits
#include "snemo/edm/GeigerHit.h"
#include <vector>

namespace snemo {
  using GeigerHitCollection = std::vector<GeigerHit>;
} // namespace snemo

#endif // GEIGERHITCOLLECTION_HH
