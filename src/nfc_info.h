#ifndef MATRIX_HAL_NFC_INFO_H
#define MATRIX_HAL_NFC_INFO_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class NFCInfo {
 public:
  bool recently_updated = false;
  std::vector<uint8_t> UID;
  std::vector<uint8_t> ATQ;
  int SAK = -1;
  int bit_rate = -1;
  std::string technology = "null";
  std::string type = "null";
  std::string card_type = "null";

  // HELPER FUNCTIONS
  void Reset() {
    // Reset parameters before populaing with info from new object
    recently_updated = true;
    UID.clear();
    ATQ.clear();
    SAK = -1;
    bit_rate = -1;
    technology = "null";
    type = "null";
    card_type = "null";
  }

  std::string StrHexUID() {
    if (UID.empty()) return "null";
    std::stringstream ret;
    ret << "0x";
    for (int i : UID) {
      ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
          << +i;
    }
    ret << std::flush;
    return ret.str();
  }

  std::string StrHexATQ() {
    if (ATQ.empty()) return "null";
    std::stringstream ret;
    ret << "0x";
    for (int i : ATQ) {
      ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
          << +i;
    }
    ret << std::flush;
    return ret.str();
  }

  std::string StrHexSAK() {
    if (SAK == -1) return "null";
    std::stringstream ret;
    ret << "0x" << std::setfill('0') << std::setw(2) << std::hex
        << std::uppercase << +SAK << std::flush;
    return ret.str();
  }

  std::string Str() {
    std::stringstream ret;
    ret << "Technology : " << technology << std::endl;
    ret << "Type : " << type << std::endl;
    ret << "UID : " << StrHexUID() << std::endl;
    ret << "ATQ(A/B) : " << StrHexATQ() << std::endl;
    ret << "SAK : " << StrHexSAK() << std::endl;
    ret << "Card Type : " << card_type << std::flush;
    return ret.str();
  }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_NFC_INFO_H