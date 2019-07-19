// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <cstdint>
#include <iostream>

#include "nfc_data.h"
#include "nfc_info.h"
#include "nfc_ndef.h"

#include "nfc_init.h"
#include "nfc_utility.h"

namespace matrix_hal {

class NFCSensor {
   public:
    NFCInit nfc_init = NFCInit();
    NFCUtility nfc_utility = NFCUtility(&nfc_init);
    // Begin User Functions
    NFCSensor();
    ~NFCSensor();
    int Activate();
    int Deactivate();
    int ReadInfo(NFCInfo* nfc_info);
    int SimpleReadInfo(NFCInfo* nfc_info);
    int ReadNDEF(NFC_NDEF* nfc_ndef);
    int WriteNDEF(NFC_NDEF* nfc_ndef);
    int EraseNDEF();
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
    // End Helper Functions
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_SENSOR_H
