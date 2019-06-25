#include <chrono>
#include <iostream>
#include <thread>

#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

#include "matrix_nfc/nfc_info.h"
#include "matrix_nfc/nfc_sensor.h"

namespace hal = matrix_hal;

int main() {
  std::string greenUID = "";
  std::string blueUID = "";
  std::string redUID = "";

  hal::MatrixIOBus bus;

  if (!bus.Init()) return false;

  hal::EverloopImage everloop_image(bus.MatrixLeds());

  hal::Everloop everloop;

  everloop.Setup(&bus);

  hal::NFCSensor nfc_sensor;
  hal::NFCInfo nfc_info;

  // Get red UID
  std::cout << "Scan Red Tag" << std::endl;
  while (true) {
    if (nfc_info.recentlyUpdated) break;
    nfc_sensor.SimpleReadInfo(&nfc_info);
  }
  redUID = nfc_info.strHexUID();

  nfc_info.recentlyUpdated = false;

  // Get green UID
  std::cout << "Scan Green Tag" << std::endl;
  while (true) {
    if (nfc_info.recentlyUpdated && nfc_info.strHexUID() != redUID) break;
    nfc_sensor.SimpleReadInfo(&nfc_info);
  }
  greenUID = nfc_info.strHexUID();

  nfc_info.recentlyUpdated = false;

  // Get blue UID
  std::cout << "Scan Blue Tag" << std::endl;
  while (true) {
    if (nfc_info.recentlyUpdated && nfc_info.strHexUID() != redUID &&
        nfc_info.strHexUID() != greenUID)
      break;
    nfc_sensor.SimpleReadInfo(&nfc_info);
  }
  blueUID = nfc_info.strHexUID();

  nfc_info.recentlyUpdated = false;

  std::cout << "\nScan specified tags to activate Everloop" << std::endl;

  do {
    nfc_sensor.SimpleReadInfo(&nfc_info);

    if (nfc_info.recentlyUpdated) {
      std::string currUID = nfc_info.strHexUID();
      if (redUID == currUID) {
        for (hal::LedValue &led : everloop_image.leds) {
          led.red = 50;
          led.green = 0;
          led.blue = 0;
          led.white = 0;
        }
      } else if (greenUID == currUID) {
        for (hal::LedValue &led : everloop_image.leds) {
          led.red = 0;
          led.green = 50;
          led.blue = 0;
          led.white = 0;
        }
      } else if (blueUID == currUID) {
        for (hal::LedValue &led : everloop_image.leds) {
          led.red = 0;
          led.green = 0;
          led.blue = 50;
          led.white = 0;
        }
      }
    } else {
      for (hal::LedValue &led : everloop_image.leds) {
        led.red = 0;
        led.green = 0;
        led.blue = 0;
        led.white = 0;
      }
    }

    everloop.Write(&everloop_image);

    std::this_thread::sleep_for(std::chrono::microseconds(10000));
  } while (true);

  return 0;
}