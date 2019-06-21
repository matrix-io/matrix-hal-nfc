#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

#include "matrix_nfc/nfc_data.h"
#include "matrix_nfc/nfc_sensor.h"

using namespace std;

namespace hal = matrix_hal;

int main() {
  std::string switchUID = "";

  hal::MatrixIOBus bus;

  if (!bus.Init()) return false;

  hal::EverloopImage everloop_image(bus.MatrixLeds());

  hal::Everloop everloop;

  everloop.Setup(&bus);

  hal::NFCSensor nfcSensor;
  hal::NFCData nfcData;

  // Get switch UID
  std::cout << "Scan ON/OFF Tag" << std::endl;
  while (true) {
    if (nfcData.recentlyUpdated) break;
    nfcSensor.Read(&nfcData);
  }
  switchUID = nfcData.strUID();

  std::cout << "ON/OFF Tag Scanned!" << std::endl;

  float counter = 0;
  const float freq = 0.375;

  do {
    nfcSensor.Read(&nfcData);

    if (nfcData.recentlyUpdated) {
      std::string currUID = nfcData.strUID();
      if (switchUID == currUID) {
        for (matrix_hal::LedValue &led : everloop_image.leds) {
          // Sine waves 120 degrees out of phase for rainbow
          led.red =
              (std::sin(freq * counter + (M_PI / 180 * 240)) * 155 + 100) / 10;
          led.green =
              (std::sin(freq * counter + (M_PI / 180 * 120)) * 155 + 100) / 10;
          led.blue = (std::sin(freq * counter + 0) * 155 + 100) / 10;
          // If MATRIX Creator, increment by 0.51 for proper rainbow speed
          counter = counter + 0.51;
        }
      }
    }

    everloop.Write(&everloop_image);
    std::this_thread::sleep_for(std::chrono::microseconds(10000));
  } while (true);

  return 0;
}