#ifndef MATRIX_HAL_NFC_DATA_H
#define MATRIX_HAL_NFC_DATA_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace matrix_hal {
class NFCData {
 public:
  bool recentlyUpdated = false;
  std::vector<int> UID = {-1};
  std::vector<int> ATQ = {-1};
  int SAK = -1;
  int bitRate = -1;
  std::string technology = "null";
  std::string type = "null";

  // HELPER FUNCTIONS
  void reset() {
    // Reset data before populaing with info from new object
    recentlyUpdated = true;
    UID = {-1};
    ATQ = {-1};
    SAK = -1;
    bitRate = -1;
    technology = "null";
    type = "null";
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

  std::string strUID() {
    if (UID == std::vector<int>({-1})) return "null";
    std::stringstream ret;
    ret << "0x";
    for (int i : UID) {
      ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
          << +i;
    }
    ret << std::flush;
    return ret.str();
  }

  std::string strATQ() {
    if (ATQ == std::vector<int>({-1})) return "null";
    std::stringstream ret;
    ret << "0x";
    for (int i : ATQ) {
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
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_NFC_DATA_H