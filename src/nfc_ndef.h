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
    bool ndef_detected = false;
    // Raw NDEF message read from the NFC Tag
    std::vector<uint8_t> read_ndef;

    // HELPER FUNCTIONS
    void Reset() {
        // Reset parameters before populating with info from new object
        recently_updated = true;
        ndef_detected = false;
        read_ndef.clear();
    }

    // TODO: Rename and Redo DumpBuffer
    void DumpBuffer() {
        if (ndef_detected == false) {
            std::cout << "NDEF is Not Detected" << std::endl;
            return;
        }
        if (read_ndef.empty()) {
            std::cout << "NDEF is NULL" << std::endl;
            return;
        }
        uint8_t* pBuffer = read_ndef.data();
        uint32_t dwBufferLength = read_ndef.size();
        uint32_t i;
        uint8_t aTempBuffer[17] = {0};

        for (i = 0; i < dwBufferLength; i++) {
            if ((i % 16) == 0) {
                if (i != 0) {
                    printf("  |%s|\n", aTempBuffer);
                }

                printf("\t\t[%04X] ", i);
            }

            printf(" %02X", pBuffer[i]);

            if ((pBuffer[i] < 0x20) || (pBuffer[i] > 0x7e)) {
                aTempBuffer[i % 16] = '.';
            } else {
                aTempBuffer[i % 16] = pBuffer[i];
            }

            aTempBuffer[16] = 0;
        }

        while ((i % 16) != 0) {
            printf("   ");
            aTempBuffer[((i++) % 16)] = ' ';
        }

        printf("  |%s|\n", aTempBuffer);
    }
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_NDEF_H