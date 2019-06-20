// Third party:
// - Boost:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/serialization/export.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// - Bayeux/datatools:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <datatools/archives_instantiation.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "timestamp.ipp"
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
  snemo::datamodel::timestamp)

/**********************************
 * snemo::datamodel::event_header *
 **********************************/

#include "event_header.ipp"
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
  snemo::datamodel::event_header)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::event_header)
