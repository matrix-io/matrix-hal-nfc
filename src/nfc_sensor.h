// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <iostream>

#include "nfc_data.h"

#include "nfc_abstraction.h"

void ExportTagInfo(phacDiscLoop_Sw_DataParams_t *pDataParams,
                   uint16_t tagTechnology, matrix_hal::NFCData *nfcData);

namespace matrix_hal {

class NFCSensor {
public:
  /* This is used to save restore Poll Config.
   * If in case application has update/change PollCfg to resolve Tech
   * when Multiple Tech was detected in previous poll cycle
   */
  uint16_t bSavePollTechCfg;

  // FUNCTION DECLARATIONS
  NFCSensor();
  void Read(NFCData *nfcData);
};

} // namespace matrix_hal

#endif // MATRIX_HAL_NFC_SENSOR_H