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

#include "matrix_nfc/nfc_ndef.h"
#include "matrix_nfc/nfc_sensor.h"

using std::cout;
using std::endl;

namespace hal = matrix_hal;

int main() {
    hal::MatrixIOBus bus;

    if (!bus.Init()) return false;

    hal::EverloopImage everloop_image(bus.MatrixLeds());

    hal::Everloop everloop;

    everloop.Setup(&bus);

    hal::NFCSensor nfc_sensor;
    hal::NFC_NDEF nfc_ndef;

    cout << "Scan a Tag, Any Tag!" << endl;

    do {
        nfc_sensor.Activate();
        nfc_sensor.ndef.ReadNDEF(&nfc_ndef);
        nfc_sensor.Deactivate();

        if (nfc_ndef.recently_updated) {
            cout << endl;
            nfc_ndef.DumpBuffer();
            for (hal::LedValue &led : everloop_image.leds) {
                led.red = 0;
                led.green = 20;
                led.blue = 0;
                led.white = 0;
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