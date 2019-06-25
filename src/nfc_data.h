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
  bool recentlyUpdated = false;
  // All data read from the NFC Tag
  std::vector<std::vector<uint8_t>> readData;

  // HELPER FUNCTIONS
  void reset() {
    // Reset parameters before populaing with info from new object
    recentlyUpdated = true;
    readData.clear();
  }

  static std::string strHexByteVec(const std::vector<uint8_t> &vec) {
    std::stringstream ret;
    for (uint8_t i : vec) {
      ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
          << +i << " ";
    }
    ret << std::flush;
    std::string str = ret.str();
    str.pop_back();
    return str;
  }

  static std::string strCharByteVec(const std::vector<uint8_t> &vec) {
    std::stringstream ret;
    for (uint8_t i : vec) {
      ret << i;
    }
    ret << std::flush;
    return ret.str();
  }

  std::string strHex() {
    uint8_t currPage = 0;
    std::stringstream ret;
    for (std::vector<uint8_t> vec : readData) {
      ret << "Page " << std::setfill('0') << std::setw(2) << std::hex
          << std::uppercase << +currPage << ": " << strHexByteVec(vec)
          << std::endl;
      currPage++;
    }
    std::string str = ret.str();
    str.pop_back();
    return str;
  }

  std::string strChar() {
    uint8_t currPage = 0;
    std::stringstream ret;
    for (std::vector<uint8_t> vec : readData) {
      ret << strCharByteVec(vec);
      currPage++;
    }
    ret << std::flush;
    return ret.str();
  }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_NFC_DATA_H