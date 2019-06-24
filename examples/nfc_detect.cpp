#include <chrono>
#include <iostream>
#include <thread>

#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

#include "matrix_nfc/nfc_info.h"
#include "matrix_nfc/nfc_sensor.h"

using std::cout;
using std::endl;

namespace hal = matrix_hal;

int main() {
  hal::MatrixIOBus bus;

  if (!bus.Init()) return false;

  hal::EverloopImage image1d(bus.MatrixLeds());

  hal::Everloop everloop;

  everloop.Setup(&bus);

  hal::NFCSensor nfcSensor;
  hal::NFCInfo nfcInfo;

  cout << "Scan a Tag, Any Tag!" << endl;

  do {
    nfcSensor.Read(&nfcInfo);

    if (nfcInfo.recentlyUpdated) {
      cout << nfcInfo.str() << endl << endl;
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