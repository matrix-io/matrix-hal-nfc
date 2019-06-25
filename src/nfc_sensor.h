// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <iostream>
#include <memory>
#include <vector>

#include "nfc_data.h"
#include "nfc_info.h"

extern "C" {
// This MUST be included last!
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phNfcLib.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/types/ph_Status.h>

// Begin Variable Setup for NXP HAL
#define DATA_BUFFER_LEN 260            /* Buffer length */
uint8_t bHalBufferTx[DATA_BUFFER_LEN]; /* HAL TX buffer. Size 260 - Based on
                                          maximum FSL */
uint8_t bHalBufferRx[DATA_BUFFER_LEN]; /* HAL RX buffer. Size 260 - Based on
                                          maximum FSL */
uint8_t sens_res[2] = {0x04, 0x00};    /* ATQ bytes - needed for anti-collision
                                        */
uint8_t nfc_id1[3] = {0xA1, 0xA2, 0xA3}; /* user defined bytes of the UID (one
                                  is hardcoded)
                                            - needed for anti-collision */
uint8_t sel_res = 0x40;
uint8_t nfc_id3 = 0xFA; /* NFC3 byte - required for anti-collision */
uint8_t poll_res[18] = {0x01, 0xFE, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xC0,
                        0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0x23, 0x45};
// Stubs, in case NXP HAL expects these implementations
void pWtoxCallBck(uint8_t bTimerId) {}
void pRtoxCallBck(uint8_t bTimerId) {}
uint8_t aAppHCEBuf[32];
uint16_t wAppHCEBuffSize = sizeof(aAppHCEBuf);
// End Variable Setup for NXP HAL
}

// void ReadNdefMessage(phalTop_Sw_DataParams_t *phalTop, uint8_t tagTechnology,
//                      matrix_hal::NFCInfo *nfcInfo);

void ExportTagInfo(phacDiscLoop_Sw_DataParams_t *pDataParams,
                   uint16_t tagTechnology, matrix_hal::NFCInfo *nfcInfo);

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
  // NXP Lib Component pointers
  phacDiscLoop_Sw_DataParams_t *pDiscLoop;
  // Begin Public User Functions
  NFCSensor();
  ~NFCSensor();
  void SimpleReadInfo(NFCInfo *nfcInfo);
  void Activate();  // Activates Card
  void Deactivate();
  void ReadInfo(NFCInfo *nfcInfo);
  std::vector<uint8_t> ReadBlock(int block);
  void ReadData(NFCData *nfcData);
  // End Public User Functions
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_SENSOR_H