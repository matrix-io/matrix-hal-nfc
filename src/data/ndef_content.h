#ifndef MATRIX_HAL_NDEF_CONTENT_H
#define MATRIX_HAL_NDEF_CONTENT_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class NDEFContent {
   public:
    bool recently_updated = false;
    bool ndef_detected = false;
    // Raw NDEF message read from the NFC Tag
    std::vector<uint8_t> content;

    // HELPER FUNCTIONS
    void Reset() {
        // Reset parameters before populating with info from new object
        recently_updated = true;
        ndef_detected = false;
        content.clear();
    }

    std::string ToHex() {
        if (content.empty()) return "";
        std::stringstream result;
        auto end = std::prev(content.end());
        for (auto iter = content.begin(); iter != end; ++iter) {
            result << std::setfill('0') << std::setw(2) << std::hex
                   << std::uppercase << +*iter << " ";
        }
        result << std::setfill('0') << std::setw(2) << std::hex
               << std::uppercase << +*end << std::flush;
        return result.str();
    }

    std::string ToString() {
        if (content.empty()) return "";
        std::stringstream result;
        for (uint8_t i : content) {
            if ((i < 0x20) || (i > 0x7e))
                result << '.';
            else
                result << i;
        }
        result << std::flush;
        return result.str();
    }
};
};  // namespace matrix_hal

#endif  // MATRIX_HAL_NDEF_CONTENT_H