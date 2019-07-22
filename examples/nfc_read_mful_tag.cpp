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

#include "matrix_nfc/nfc.h"
#include "matrix_nfc/nfc_data.h"

using std::cout;
using std::endl;

namespace hal = matrix_hal;

int main() {
    hal::MatrixIOBus bus;

    if (!bus.Init()) return false;

    hal::EverloopImage everloop_image(bus.MatrixLeds());

    hal::Everloop everloop;

    everloop.Setup(&bus);

    hal::NFC nfc;
    hal::NFCData nfc_data;

    cout << "Scan a Mifare Ultralight or NTAG" << endl;

    do {
        nfc.Activate();
        nfc.mful.ReadPages(&nfc_data.pages);
        nfc.Deactivate();

        if (nfc_data.pages.recently_updated) {
            cout << nfc_data.pages.ToHex() << endl << endl;
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