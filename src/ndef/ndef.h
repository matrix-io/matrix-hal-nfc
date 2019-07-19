// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_NDEF_H
#define MATRIX_HAL_NFC_NDEF_H

#include <cstdint>
#include <iostream>

#include "../nfc_ndef.h"

#include "../nfc_utility.h"

#include "../nfc_init.h"

namespace matrix_hal {

class NDEF {
   public:
    NFCInit* nfc_init;
    NFCUtility* nfc_utility;
    NDEF(NFCInit* nfc_init, NFCUtility* nfc_utility)
        : nfc_init(nfc_init), nfc_utility(nfc_utility){};
    int ReadNDEF(NFC_NDEF* nfc_ndef);
    int WriteNDEF(NFC_NDEF* nfc_ndef);
    int EraseNDEF();
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_NDEF_H
