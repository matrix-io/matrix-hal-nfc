// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_NTAG_H
#define MATRIX_HAL_NFC_NTAG_H

#include <vector>

#include "mful.h"

#include "../nfc_data.h"

#include "../nfc_utility.h"

#include "../nfc_init.h"

namespace matrix_hal {

class NTAG : public MFUL {
   public:
    NFCInit* nfc_init;
    NFCUtility* nfc_utility;
    NTAG(NFCInit* nfc_init, NFCUtility* nfc_utility)
        : MFUL(nfc_init, nfc_utility){};
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_NTAG_H