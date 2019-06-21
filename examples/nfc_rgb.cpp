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
  hal::NFCData nfcData;

  // Get red UID
  std::cout << "Scan Red Tag" << std::endl;
  while (true) {
    if (nfcData.recentlyUpdated) break;
    nfcSensor.Read(&nfcData);
  }
  redUID = nfcData.strUID();

  nfcData.recentlyUpdated = false;

  // Get green UID
  std::cout << "Scan Green Tag" << std::endl;
  while (true) {
    if (nfcData.recentlyUpdated && nfcData.strUID() != redUID) break;
    nfcSensor.Read(&nfcData);
  }
  greenUID = nfcData.strUID();

  nfcData.recentlyUpdated = false;

  // Get blue UID
  std::cout << "Scan Blue Tag" << std::endl;
  while (true) {
    if (nfcData.recentlyUpdated && nfcData.strUID() != redUID &&
        nfcData.strUID() != greenUID)
      break;
    nfcSensor.Read(&nfcData);
  }
  blueUID = nfcData.strUID();

  nfcData.recentlyUpdated = false;

  std::cout << "\nScan specified tags to activate Everloop" << std::endl;

  do {
    nfcSensor.Read(&nfcData);

    if (nfcData.recentlyUpdated) {
      std::string currUID = nfcData.strUID();
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