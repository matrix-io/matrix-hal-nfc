#ifndef MATRIX_HAL_NFC_DATA_H
#define MATRIX_HAL_NFC_DATA_H

#include <cstdint>
#include <vector>

namespace matrix_hal {
class NFCData {
  bool recentlyUpdated = false;
  // All data read from the NFC Tag
  std::vector<std::vector<uint8_t>>> readData;

  // HELPER FUNCTIONS
  void reset() {
    // Reset parameters before populaing with info from new object
    recentlyUpdated = true;
    readData.clear();
  }
};
};      // namespace matrix_hal
#endif  // MATRIX_HAL_NFC_DATA_H