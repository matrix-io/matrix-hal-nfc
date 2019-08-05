#ifndef MATRIX_HAL_PAGES_CONTENT_H
#define MATRIX_HAL_PAGES_CONTENT_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class PagesContent {
   public:
    // True if new tag was read, False if no tag found
    bool recently_updated = false;
    // True if all pages were read, False if read was interrupted
    bool read_complete = false;
    // All data read from the NFC Tag
    std::vector<std::vector<uint8_t>> content;

    // HELPER FUNCTIONS
    void Reset() {
        // Reset parameters before populating with info from new object
        recently_updated = true;
        read_complete = false;
        content.clear();
    }

    static std::string BytesToHex(const std::vector<uint8_t> &vec) {
        if (vec.empty()) return "";
        std::stringstream result;
        auto end = std::prev(vec.end());
        for (auto iter = vec.begin(); iter != end; ++iter) {
            result << std::setfill('0') << std::setw(2) << std::hex
                   << std::uppercase << +*iter << " ";
        }
        result << std::setfill('0') << std::setw(2) << std::hex
               << std::uppercase << +*end << std::flush;
        return result.str();
    }

    static std::string BytesToString(const std::vector<uint8_t> &vec) {
        if (vec.empty()) return "";
        std::stringstream result;
        for (uint8_t i : vec) {
            if ((i < 0x20) || (i > 0x7e))
                result << '.';
            else
                result << i;
        }
        result << std::flush;
        return result.str();
    }

    std::string ToHex() {
        if (content.empty()) return "";
        uint8_t page_number = 0;
        std::stringstream result;
        auto end = std::prev(content.end());
        for (auto iter = content.begin(); iter != end; ++iter) {
            result << "Page " << std::setfill('0') << std::setw(2) << std::hex
                   << std::uppercase << +page_number << ": "
                   << BytesToHex(*iter) << std::endl;
            ++page_number;
        }
        result << "Page " << std::setfill('0') << std::setw(2) << std::hex
               << std::uppercase << +page_number << ": " << BytesToHex(*end)
               << std::flush;
        return result.str();
    }

    std::string ToString() {
        if (content.empty()) return "";
        uint8_t page_number = 0;
        std::stringstream result;
        for (std::vector<uint8_t> vec : content) {
            result << BytesToString(vec);
            ++page_number;
        }
        result << std::flush;
        return result.str();
    }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_PAGES_CONTENT_H