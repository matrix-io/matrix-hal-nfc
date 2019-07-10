// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <cstdint>
#include <iostream>
#include <vector>

#include "nfc_data.h"
#include "nfc_info.h"
#include "nfc_types.h"

// Begin NXP Defines
#define DATA_BUFFER_LEN 260
// These two are specifically for the Simplified NXP Lib
#define SIMPLIFIED_ISO_PRIO 0
#define SIMPLIFIED_ISO_STACK 0x20000
// END NXP Defines

extern "C" {
// Begin Variable Setup for NXP HAL
// Do not change variable names in this section
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

namespace matrix_hal {
class NFCSensor {
 public:
  // Begin Variable Setup for Simplified NXP Lib
  uint32_t nfc_lib_status = 0;
  uint16_t technology_mask = 0x1B;
  uint8_t more_data_available = 0;
  phNfcLib_Transmit_t nfc_lib_transmit;
  phNfcLib_PeerInfo_t peer_info = {0};
  uint8_t data_buffer[DATA_BUFFER_LEN];  // universal data buffer
  // End Variable Setup for Simplified NXP Lib
  // NXP Lib Component pointers
  phbalReg_Type_t* bal;              // Bus Abstraction Layer component
  phhalHw_Nfc_Ic_DataParams_t* hal;  // Hardware Abstraction Layer component
  phpalI14443p3a_Sw_DataParams_t*
      pal_iso14443p3a;  // PAL ISO I14443-A component
  phpalI14443p3b_Sw_DataParams_t*
      pal_iso14443p3b;  // PAL ISO I14443-B component
  phpalI14443p4a_Sw_DataParams_t*
      pal_iso14443p4a;                            // PAL ISO I14443-4A component
  phpalI14443p4_Sw_DataParams_t* pal_iso14443p4;  // PAL ISO I14443-4 component
  phacDiscLoop_Sw_DataParams_t* disc_loop;        // Discovery loop component
  // Begin User Functions
  NFCSensor();
  ~NFCSensor();
  int Activate();
  int Deactivate();
  int ReadInfo(NFCInfo* nfc_info);
  int SimpleReadInfo(NFCInfo* nfc_info);
  std::vector<uint8_t> ReadPage_MFUL_NTAG(uint8_t page_number);
  int WritePage_MFUL_NTAG(uint8_t page_number,
                          std::vector<uint8_t>& write_data);
  int ReadData_MFUL_NTAG(NFCData* nfc_data);
  // End User Functions
  // Begin Helper Functions
  void ExportTagInfo(phacDiscLoop_Sw_DataParams_t* disc_loop,
                     uint16_t tag_tech_type, NFCInfo* nfc_info);
  // End Helper Functions
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_SENSOR_H