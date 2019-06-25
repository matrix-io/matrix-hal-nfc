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
  bool recentlyUpdated = false;
  std::vector<uint8_t> UID;
  std::vector<uint8_t> ATQ;
  int SAK = -1;
  int bitRate = -1;
  std::string technology = "null";
  std::string type = "null";

  // HELPER FUNCTIONS
  void reset() {
    // Reset parameters before populaing with info from new object
    recentlyUpdated = true;
    UID.clear();
    ATQ.clear();
    SAK = -1;
    bitRate = -1;
    technology = "null";
    type = "null";
  }

  std::string strHexUID() {
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

  std::string strHexATQ() {
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

  std::string strHexSAK() {
    if (SAK == -1) return "null";
    std::stringstream ret;
    ret << "0x" << std::setfill('0') << std::setw(2) << std::hex
        << std::uppercase << +SAK << std::flush;
    return ret.str();
  }

  std::string str() {
    std::stringstream ret;
    ret << "Technology : " << technology << std::endl;
    ret << "UID : " << strHexUID() << std::endl;
    ret << "ATQ(A/B) : " << strHexATQ() << std::endl;
    ret << "SAK : " << strHexSAK() << std::endl;
    ret << "Type : " << type << std::flush;
    return ret.str();
  }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_NFC_INFO_H