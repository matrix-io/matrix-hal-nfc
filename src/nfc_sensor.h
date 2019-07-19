// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_SENSOR_H
#define MATRIX_HAL_NFC_SENSOR_H

#include <cstdint>
#include <iostream>

#include "nfc_data.h"
#include "nfc_info.h"
#include "nfc_ndef.h"

#include "cards/mful.h"
#include "cards/ntag.h"

#include "ndef/ndef.h"

#include "nfc_utility.h"

#include "nfc_init.h"

namespace matrix_hal {

class NFCSensor {
   public:
    NFCInit nfc_init = NFCInit();
    NFCUtility nfc_utility = NFCUtility(&nfc_init);
    NDEF ndef = NDEF(&nfc_init, &nfc_utility);
    MFUL mful = MFUL(&nfc_init, &nfc_utility);
    NTAG ntag = NTAG(&nfc_init, &nfc_utility);
    // Begin User Functions for all
    int Activate();
    int Deactivate();
    int ReadInfo(NFCInfo* nfc_info);
    int SimpleReadInfo(NFCInfo* nfc_info);
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_SENSOR_H
