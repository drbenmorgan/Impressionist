#include "canvas/Persistency/Common/Wrapper.h"

// Need to include entire inheritance chain
#include "bayeux/datatools/i_clear.h"
#include "bayeux/datatools/i_serializable.h"
#include "bayeux/datatools/i_tree_dump.h"

#include "bayeux/datatools/event_id.h"
#include "bayeux/datatools/properties.h"
#include "bayeux/geomtools/geom_id.h"

// Datatools workarounds
#include "snemo/edm/MultiProperties.h"

// MC
#include "snemo/edm/GenBBPrimaryEvent.h"
#include "snemo/edm/StepHit.h"
#include "snemo/edm/StepHitCollection.h"

// Processing
#include "snemo/edm/CalorimeterHit.h"
#include "snemo/edm/CalorimeterHitCollection.h"
#include "snemo/edm/GeigerHit.h"
#include "snemo/edm/GeigerHitCollection.h"
