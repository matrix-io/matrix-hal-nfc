#ifndef MATRIX_HAL_NFC_NDEF_H
#define MATRIX_HAL_NFC_NDEF_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class NFC_NDEF {
   public:
    bool recently_updated = false;
    // Raw NDEF message read from the NFC Tag
    std::vector<uint8_t> read_ndef;

    // HELPER FUNCTIONS
    void Reset() {
        // Reset parameters before populating with info from new object
        recently_updated = true;
        read_ndef.clear();
    }
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_NDEF_H