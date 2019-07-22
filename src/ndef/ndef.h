// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_NDEF_H
#define MATRIX_HAL_NFC_NDEF_H

#include <cstdint>
#include <iostream>

#include "../data/ndef_content.h"

#include "../nfc_utility.h"

#include "../nfc_init.h"

namespace matrix_hal {

class NDEF {
   public:
    NFCInit* nfc_init;
    NFCUtility* nfc_utility;
    NDEF(NFCInit* nfc_init, NFCUtility* nfc_utility)
        : nfc_init(nfc_init), nfc_utility(nfc_utility){};
    int ReadNDEF(NDEFContent* ndef_content);
    int WriteNDEF(NDEFContent* ndef_content);
    int EraseNDEF();
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_NDEF_H
