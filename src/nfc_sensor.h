// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <iostream>

#include "nfc_data.h"

extern "C" {
#include "./nfc_abstraction.h"

/*NXP HAL variables*/
uint8_t bHalBufferTx[256]; /* HAL TX buffer. Size 256 - Based on maximum FSL */
uint8_t bHalBufferRx[256]; /* HAL RX buffer. Size 256 - Based on maximum FSL */

uint8_t sens_res[2] = {0x04, 0x00}; /* ATQ bytes - needed for anti-collision */
uint8_t nfc_id1[3] = {0xA1, 0xA2,
                      0xA3}; /* user defined bytes of the UID (one is hardcoded)
                                - needed for anti-collision */
uint8_t sel_res = 0x40;
uint8_t nfc_id3 = 0xFA; /* NFC3 byte - required for anti-collision */
uint8_t poll_res[18] = {0x01, 0xFE, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xC0,
                        0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0x23, 0x45};
}

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

  uint8_t pBuffUUID[10];
  uint8_t newCard;

  // FUNCTION DECLARATIONS
  NFCSensor();
  void Read(NFCData *nfcData);
};

} // namespace matrix_hal

#endif // MATRIX_HAL_NFC_SENSOR_H