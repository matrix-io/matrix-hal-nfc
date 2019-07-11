#ifndef MATRIX_HAL_NFC_DATA_H
#define MATRIX_HAL_NFC_DATA_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class NFCData {
   public:
    bool recently_updated = false;
    // All data read from the NFC Tag
    std::vector<std::vector<uint8_t>> read_data;

    // HELPER FUNCTIONS
    void Reset() {
        // Reset parameters before populating with info from new object
        recently_updated = true;
        read_data.clear();
    }

    static std::string StrHexByteVec(const std::vector<uint8_t> &vec) {
        if (vec.empty()) return "";
        std::stringstream ret;
        auto end = std::prev(vec.end());
        for (auto iter = vec.begin(); iter != end; ++iter) {
            ret << std::setfill('0') << std::setw(2) << std::hex
                << std::uppercase << +*iter << " ";
        }
        ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
            << +*end << std::flush;
        return ret.str();
    }

    static std::string StrCharByteVec(const std::vector<uint8_t> &vec) {
        if (vec.empty()) return "";
        std::stringstream ret;
        for (uint8_t i : vec) {
            ret << i;
        }
        ret << std::flush;
        return ret.str();
    }

    std::string StrHex() {
        if (read_data.empty()) return "";
        uint8_t page_number = 0;
        std::stringstream ret;
        auto end = std::prev(read_data.end());
        for (auto iter = read_data.begin(); iter != end; ++iter) {
            ret << "Page " << std::setfill('0') << std::setw(2) << std::hex
                << std::uppercase << +page_number << ": "
                << StrHexByteVec(*iter) << std::endl;
            ++page_number;
        }
        ret << "Page " << std::setfill('0') << std::setw(2) << std::hex
            << std::uppercase << +page_number << ": " << StrHexByteVec(*end)
            << std::flush;
        return ret.str();
    }

    std::string StrChar() {
        if (read_data.empty()) return "";
        uint8_t page_number = 0;
        std::stringstream ret;
        for (std::vector<uint8_t> vec : read_data) {
            ret << StrCharByteVec(vec);
            ++page_number;
        }
        ret << std::flush;
        return ret.str();
    }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_NFC_DATA_H