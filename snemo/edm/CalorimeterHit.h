#ifndef CALORIMETERHIT_HH
#define CALORIMETERHIT_HH

/// Rough Model of a calibrated calorimeter hit

#include "bayeux/geomtools/geom_id.h"

namespace snemo {

  struct CalorimeterHit {
    // Info
    size_t hitID;
    geomtools::geom_id geoID;

    // Data
    double energy;
    double energyError;
    double time;
    double timeError;
  };

} // namespace snemo

#endif // CALORIMETERHIT_HH
