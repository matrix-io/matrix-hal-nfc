#ifndef MATRIX_HAL_NFC_INFO_H
#define MATRIX_HAL_NFC_INFO_H

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class NFCInfo {
 public:
  bool recentlyUpdated = false;
  std::unique_ptr<std::vector<uint8_t>> UID;
  std::unique_ptr<std::vector<uint8_t>> ATQ;
  int SAK = -1;
  int bitRate = -1;
  std::string technology = "null";
  std::string type = "null";

  // HELPER FUNCTIONS
  void reset() {
    // Reset parameters before populaing with info from new object
    recentlyUpdated = true;
    UID.reset();
    ATQ.reset();
    SAK = -1;
    bitRate = -1;
    technology = "null";
    type = "null";
  }

  std::string strUID() {
    if (UID == nullptr) return "null";
    std::stringstream ret;
    ret << "0x";
    for (int i : *UID) {
      ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
          << +i;
    }
    ret << std::flush;
    return ret.str();
  }

  std::string strATQ() {
    if (ATQ == nullptr) return "null";
    std::stringstream ret;
    ret << "0x";
    for (int i : *ATQ) {
      ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
          << +i;
    }
    ret << std::flush;
    return ret.str();
  }

  std::string strSAK() {
    if (SAK == -1) return "null";
    std::stringstream ret;
    ret << "0x" << std::setfill('0') << std::setw(2) << std::hex
        << std::uppercase << +SAK << std::flush;
    return ret.str();
  }

  std::string str() {
    std::stringstream ret;
    ret << "Technology : " << technology << std::endl;
    ret << "UID : " << strUID() << std::endl;
    ret << "ATQ(A/B) : " << strATQ() << std::endl;
    ret << "SAK : " << strSAK() << std::endl;
    ret << "Type : " << type << std::flush;
    return ret.str();
  }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_NFC_INFO_H