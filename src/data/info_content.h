#ifndef MATRIX_HAL_INFO_CONTENT_H
#define MATRIX_HAL_INFO_CONTENT_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class InfoContent {
   public:
    bool recently_updated = false;
    std::vector<uint8_t> UID;
    std::vector<uint8_t> ATQ;
    int SAK = -1;
    int bit_rate = -1;
    std::string technology = "null";
    std::string type = "null";
    std::string card_family = "null";
    std::string IC_type = "null";
    int storage_size = -1;

    // HELPER FUNCTIONS
    void Reset() {
        // Reset parameters before populating with info from new object
        recently_updated = true;
        UID.clear();
        ATQ.clear();
        SAK = -1;
        bit_rate = -1;
        technology = "null";
        type = "null";
        card_family = "null";
        IC_type = "null";
        storage_size = -1;
    }

    std::string UIDToHex() {
        if (UID.empty()) return "null";
        std::stringstream result;
        result << "0x";
        for (int i : UID) {
            result << std::setfill('0') << std::setw(2) << std::hex
                   << std::uppercase << +i;
        }
        result << std::flush;
        return result.str();
    }

    std::string ATQToHex() {
        if (ATQ.empty()) return "null";
        std::stringstream result;
        result << "0x";
        for (int i : ATQ) {
            result << std::setfill('0') << std::setw(2) << std::hex
                   << std::uppercase << +i;
        }
        result << std::flush;
        return result.str();
    }

    std::string SAKToHex() {
        if (SAK == -1) return "null";
        std::stringstream result;
        result << "0x" << std::setfill('0') << std::setw(2) << std::hex
               << std::uppercase << +SAK << std::flush;
        return result.str();
    }

    std::string ToString() {
        std::stringstream result;
        result << "Technology : " << technology << std::endl;
        result << "Type : " << type << std::endl;
        result << "UID : " << UIDToHex() << std::endl;
        result << "ATQ(A/B) : " << ATQToHex() << std::endl;
        result << "SAK : " << SAKToHex() << std::endl;
        if (bit_rate != -1)
            result << "Bit Rate :" << bit_rate << " baud" << std::endl;
        result << "Card Family : " << card_family << std::endl;
        result << "IC Type : " << IC_type << std::endl;
        if (storage_size != -1)
            result << "Storage Size : " << storage_size << " bytes"
                   << std::flush;
        return result.str();
    }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_INFO_CONTENT_H