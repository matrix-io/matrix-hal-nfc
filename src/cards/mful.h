// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_MFUL_H
#define MATRIX_HAL_NFC_MFUL_H

#include <vector>

#include "../nfc_data.h"

#include "../nfc_utility.h"

#include "../nfc_init.h"

namespace matrix_hal {

class MFUL {
   public:
    NFCInit* nfc_init;
    NFCUtility* nfc_utility;
    MFUL(NFCInit* nfc_init, NFCUtility* nfc_utility)
        : nfc_init(nfc_init), nfc_utility(nfc_utility){};
    std::vector<uint8_t> ReadPage(uint8_t page_number);
    int WritePage(uint8_t page_number, std::vector<uint8_t>& write_data);
    int ReadData(NFCData* nfc_data);
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_MFUL_H