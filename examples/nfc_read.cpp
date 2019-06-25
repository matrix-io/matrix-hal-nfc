#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

#include "matrix_nfc/nfc_data.h"
#include "matrix_nfc/nfc_sensor.h"

using std::cout;
using std::endl;

namespace hal = matrix_hal;

std::string strVector(const std::vector<uint8_t> &vec) {
  std::stringstream ret;
  ret << "0x";
  for (int i : vec) {
    ret << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
        << +i;
  }
  ret << std::flush;
  return ret.str();
}

int main() {
  hal::MatrixIOBus bus;

  if (!bus.Init()) return false;

  hal::EverloopImage image1d(bus.MatrixLeds());

  hal::Everloop everloop;

  everloop.Setup(&bus);

  hal::NFCSensor nfcSensor;
  hal::NFCInfo nfcInfo;

  cout << "Scan a Tag, Any Tag!" << endl;

  uint8_t pageNumber = 0;
  std::vector<uint8_t> result;

  do {
    result.clear();
    nfcSensor.Activate();
    result = nfcSensor.ReadPage(pageNumber);
    nfcSensor.Deactivate();

    if (!result.empty()) {
      cout << strVector(result) << endl << endl;
      for (hal::LedValue &led : image1d.leds) {
        led.red = 0;
        led.green = 20;
        led.blue = 0;
        led.white = 0;
      }
    } else {
      for (hal::LedValue &led : image1d.leds) {
        led.red = 0;
        led.green = 0;
        led.blue = 0;
        led.white = 0;
      }
    }

    everloop.Write(&image1d);

    std::this_thread::sleep_for(std::chrono::microseconds(10000));
  } while (true);

  return 0;
}