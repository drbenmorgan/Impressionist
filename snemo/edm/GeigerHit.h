#ifndef GEIGERHIT_HH
#define GEIGERHIT_HH

/// Rough model of a calibrated Geiger hit
#include "bayeux/geomtools/geom_id.h"
#include <cstdint>

namespace snemo {
  struct GeigerHit {
    uint32_t flags;
    double radius;
    double radiusError;
    double z;
    double zError;
    double x;
    double y;
    double delayedTime;
    double delayedTimeError;
    geomtools::geom_id geoId;
  };
} // namespace snemo

#endif // GEIGERHIT_HH
