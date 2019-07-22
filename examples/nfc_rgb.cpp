#include <chrono>
#include <iostream>
#include <thread>

#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

#include "matrix_nfc/nfc.h"
#include "matrix_nfc/nfc_data.h"

namespace hal = matrix_hal;

int main() {
    std::string green_UID = "";
    std::string blue_UID = "";
    std::string red_UID = "";

    hal::MatrixIOBus bus;

    if (!bus.Init()) return false;

    hal::EverloopImage everloop_image(bus.MatrixLeds());

    hal::Everloop everloop;

    everloop.Setup(&bus);

    hal::NFC nfc;
    hal::NFCData nfc_data;

    // Get red UID
    std::cout << "Scan Red Tag" << std::endl;
    while (true) {
        if (nfc_data.info.recently_updated) break;
        nfc.SimpleReadInfo(&nfc_data.info);
    }
    red_UID = nfc_data.info.UIDToHex();

    nfc_data.info.recently_updated = false;

    // Get green UID
    std::cout << "Scan Green Tag" << std::endl;
    while (true) {
        if (nfc_data.info.recently_updated &&
            nfc_data.info.UIDToHex() != red_UID)
            break;
        nfc.SimpleReadInfo(&nfc_data.info);
    }
    green_UID = nfc_data.info.UIDToHex();

    nfc_data.info.recently_updated = false;

    // Get blue UID
    std::cout << "Scan Blue Tag" << std::endl;
    while (true) {
        if (nfc_data.info.recently_updated &&
            nfc_data.info.UIDToHex() != red_UID &&
            nfc_data.info.UIDToHex() != green_UID)
            break;
        nfc.SimpleReadInfo(&nfc_data.info);
    }
    blue_UID = nfc_data.info.UIDToHex();

    nfc_data.info.recently_updated = false;

    std::cout << "\nScan specified tags to activate Everloop" << std::endl;

    do {
        nfc.SimpleReadInfo(&nfc_data.info);

        if (nfc_data.info.recently_updated) {
            std::string curr_UID = nfc_data.info.UIDToHex();
            if (red_UID == curr_UID) {
                for (hal::LedValue &led : everloop_image.leds) {
                    led.red = 50;
                    led.green = 0;
                    led.blue = 0;
                    led.white = 0;
                }
            } else if (green_UID == curr_UID) {
                for (hal::LedValue &led : everloop_image.leds) {
                    led.red = 0;
                    led.green = 50;
                    led.blue = 0;
                    led.white = 0;
                }
            } else if (blue_UID == curr_UID) {
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