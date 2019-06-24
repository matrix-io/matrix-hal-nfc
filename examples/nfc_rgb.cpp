#include <chrono>
#include <iostream>
#include <thread>

#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

#include "matrix_nfc/nfc_data.h"
#include "matrix_nfc/nfc_sensor.h"

namespace hal = matrix_hal;

int main() {
  std::string greenUID = "";
  std::string blueUID = "";
  std::string redUID = "";

  hal::MatrixIOBus bus;

  if (!bus.Init()) return false;

  hal::EverloopImage image1d(bus.MatrixLeds());

  hal::Everloop everloop;

  everloop.Setup(&bus);

  hal::NFCSensor nfcSensor;
  hal::NFCInfo nfcInfo;

  // Get red UID
  std::cout << "Scan Red Tag" << std::endl;
  while (true) {
    if (nfcInfo.recentlyUpdated) break;
    nfcSensor.Read(&nfcInfo);
  }
  redUID = nfcInfo.strUID();

  nfcInfo.recentlyUpdated = false;

  // Get green UID
  std::cout << "Scan Green Tag" << std::endl;
  while (true) {
    if (nfcInfo.recentlyUpdated && nfcInfo.strUID() != redUID) break;
    nfcSensor.Read(&nfcInfo);
  }
  greenUID = nfcInfo.strUID();

  nfcInfo.recentlyUpdated = false;

  // Get blue UID
  std::cout << "Scan Blue Tag" << std::endl;
  while (true) {
    if (nfcInfo.recentlyUpdated && nfcInfo.strUID() != redUID &&
        nfcInfo.strUID() != greenUID)
      break;
    nfcSensor.Read(&nfcInfo);
  }
  blueUID = nfcInfo.strUID();

  nfcInfo.recentlyUpdated = false;

  std::cout << "\nScan specified tags to activate Everloop" << std::endl;

  do {
    nfcSensor.Read(&nfcInfo);

    if (nfcInfo.recentlyUpdated) {
      std::string currUID = nfcInfo.strUID();
      if (redUID == currUID) {
        for (hal::LedValue &led : image1d.leds) {
          led.red = 50;
          led.green = 0;
          led.blue = 0;
          led.white = 0;
        }
      } else if (greenUID == currUID) {
        for (hal::LedValue &led : image1d.leds) {
          led.red = 0;
          led.green = 50;
          led.blue = 0;
          led.white = 0;
        }
      } else if (blueUID == currUID) {
        for (hal::LedValue &led : image1d.leds) {
          led.red = 0;
          led.green = 0;
          led.blue = 50;
          led.white = 0;
        }
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