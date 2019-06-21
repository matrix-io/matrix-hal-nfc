// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <iostream>

#include "nfc_abstraction.h"
#include "nfc_data.h"

// Set to false to disable std::cerr from outputting error and debug messages
bool cerrMessage = true;

void ExportTagInfo(phacDiscLoop_Sw_DataParams_t *pDataParams,
                   uint16_t tagTechnology, matrix_hal::NFCData *nfcData);

namespace matrix_hal {

class NFCSensor {
 public:
// Begin Variable Setup for Simplified NXP Lib
#define SIMPLIFIED_ISO_PRIO 0
#define SIMPLIFIED_ISO_STACK 0x20000
  uint32_t nfcLibStatus = 0;
  uint16_t wTechnologyMask = 0x1B;
  uint8_t bMoreDataAvailable = 0;
  uint16_t wNumberofBytes = DATA_BUFFER_LEN;
  phNfcLib_Transmit_t phNfcLib_TransmitInput;
  phNfcLib_PeerInfo_t PeerInfo = {0};
  uint8_t bDataBuffer[DATA_BUFFER_LEN]; /* universal data buffer */
  // End Variable Setup for Simplified NXP Lib

  // Begin Public User Functions
  NFCSensor();
  ~NFCSensor();
  void Read(NFCData *nfcData);
  // End Public User Functions
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_SENSOR_H