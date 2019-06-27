// Ourselves:
#include <snfee/model/feb_constants.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/clhep_units.h>

namespace snfee {
  namespace model {

    const double feb_constants::SAMLONG_DEFAULT_SAMPLING_FREQUENCY =
      SAMLONG_DEFAULT_SAMPLING_FREQUENCY_MHZ * CLHEP::megahertz;     

    const double feb_constants::SAMLONG_ADC_MIN_VOLTAGE = -1.25 * CLHEP::volt;
    
    const double feb_constants::SAMLONG_ADC_MAX_VOLTAGE = +1.25 * CLHEP::volt;
    
    const double feb_constants::SAMLONG_ADC_MIN_VOLTAGE_MV =
      SAMLONG_ADC_MIN_VOLTAGE / (1.e-3 * CLHEP::volt);
    
    const double feb_constants::SAMLONG_ADC_MAX_VOLTAGE_MV =
      SAMLONG_ADC_MAX_VOLTAGE / (1.e-3 * CLHEP::volt);
    
    const double feb_constants::SAMLONG_ADC_VOLTAGE_LSB =
      (feb_constants::SAMLONG_ADC_MAX_VOLTAGE - feb_constants::SAMLONG_ADC_MIN_VOLTAGE) /
      feb_constants::SAMLONG_ADC_NBITS;
    
    const double feb_constants::SAMLONG_ADC_VOLTAGE_LSB_MV = 
      feb_constants::SAMLONG_ADC_VOLTAGE_LSB / (1.e-3 * CLHEP::volt);
   
    const double feb_constants::SAMLONG_DEFAULT_TDC_LSB = 
      1. / feb_constants::SAMLONG_DEFAULT_SAMPLING_FREQUENCY;
   
    const double feb_constants::SAMLONG_DEFAULT_TDC_LSB_NS = 
      feb_constants::SAMLONG_DEFAULT_TDC_LSB / CLHEP::nanosecond;

  } // namespace model
} // namespace snfee
