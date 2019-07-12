// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <cstdint>
#include <iostream>
#include <vector>

#include "nfc_data.h"
#include "nfc_info.h"
#include "nfc_ndef.h"
#include "nfc_types.h"

// Begin NXP Defines
#define DATA_BUFFER_LEN 260
// These two are specifically for the Simplified NXP Lib
#define SIMPLIFIED_ISO_PRIO 0
#define SIMPLIFIED_ISO_STACK 0x20000
// END NXP Defines

// Begin Variable Setup for NXP HAL
// DO NOT change variable names in this section
extern "C" {
uint8_t bHalBufferTx[DATA_BUFFER_LEN];  // HAL TX buffer
uint8_t bHalBufferRx[DATA_BUFFER_LEN];  // HAL RX buffer
uint8_t sens_res[2] = {0x04, 0x00};  // ATQ bytes - required for anti-collision
uint8_t nfc_id1[3] = {
    0xA1, 0xA2,
    0xA3};  // User defined bytes of the UID - required for anti-collision
uint8_t sel_res = 0x40;
uint8_t nfc_id3 = 0xFA;  // NFC3 byte - required for anti-collision
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
    // NXP Simplified Lib Component pointers (Managed by Simplified NXP Lib)
    phbalReg_Type_t* bal;              // Bus Abstraction Layer component
    phhalHw_Nfc_Ic_DataParams_t* hal;  // Hardware Abstraction Layer component
    phpalI14443p3a_Sw_DataParams_t*
        pal_iso14443p3a;  // Protocol Abstraction Layer ISO I14443-A component
    phpalI14443p3b_Sw_DataParams_t*
        pal_iso14443p3b;  // Protocol Abstraction Layer ISO I14443-B component
    phpalI14443p4a_Sw_DataParams_t*
        pal_iso14443p4a;  // Protocol Abstraction Layer ISO I14443-4A component
    phpalI14443p4_Sw_DataParams_t*
        pal_iso14443p4;  // Protocol Abstraction Layer ISO I14443-4 component
    phacDiscLoop_Sw_DataParams_t*
        discovery_loop;  // Application Layer Discovery Loop component
    // NFC Standard Lib Components (Adding additional functionality)
    phpalMifare_Sw_DataParams_t
        pal_mifare;  // Protocol Abstraction Layer Mifare component
    phpalFelica_Sw_DataParams_t
        pal_felica;  // Protocol Abstraction Layer Felica component
    phalMful_Sw_DataParams_t
        al_mful;  // Application Layer Mifare Ultralight component
    phalMfdf_Sw_DataParams_t
        al_mfdf;  // Application Layer Mifare DESFire component
    phalMfc_Sw_DataParams_t
        al_mfc;  // Application Layer Mifare Classic component
    phalFelica_Sw_DataParams_t al_felica;  // Application Layer Felica component
    phalT1T_Sw_DataParams_t al_t1t;        // Application Layer Mifare component
    phalTop_Sw_DataParams_t
        tag_operations;  // Application Layer Tag Operations component
    // Begin User Functions
    NFCSensor();
    ~NFCSensor();
    int Activate();
    int Deactivate();
    int ReadInfo(NFCInfo* nfc_info);
    int SimpleReadInfo(NFCInfo* nfc_info);
    int ReadNDEF(NFC_NDEF* nfc_ndef);
    // Card specific functions reside in their corresponding classes
    // Functions for Mifare Ultralight Cards
    class MFUL {
       public:
        NFCSensor* nfc_sensor;
        MFUL(NFCSensor* nfc_sensor) : nfc_sensor(nfc_sensor){};
        std::vector<uint8_t> ReadPage(uint8_t page_number);
        int WritePage(uint8_t page_number, std::vector<uint8_t>& write_data);
        int ReadData(NFCData* nfc_data);
    };
    //   Functions for NDEF Cards
    //   Read and Write are identical to MFUL
    class NTAG : public MFUL {
       public:
        NTAG(NFCSensor* nfc_sensor) : MFUL(nfc_sensor){};
    };
    // End User Functions
    // Instantiate above subclasses
    MFUL mful = MFUL(this);
    NTAG ntag = NTAG(this);
    // Begin Helper Functions
    // The three functions below are for NDEF
    // TODO: Cleanup NDEF
    int ActuallyReadNDEF(uint8_t tag_tech_type, NFC_NDEF* nfc_ndef);
    int ExportTag(uint16_t tag_tech_type, NFCInfo* nfc_info,
                  NFC_NDEF* nfc_ndef);
    // End Helper Functions
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_SENSOR_H