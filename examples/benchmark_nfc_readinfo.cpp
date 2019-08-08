/////////////////////////
// INCLUDE STATEMENTS //
///////////////////////

// For console output
#include <iostream>
// For sleep
#include <chrono>
#include <thread>
// For sine
#include <cmath>
// For string
#include <string>

// For using the Everloop
#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

// For using NFC
#include "matrix_nfc/nfc.h"
#include "matrix_nfc/nfc_data.h"

int main() {
    ////////////////////
    // INITIAL SETUP //
    //////////////////

    // Setting up HAL bus
    matrix_hal::MatrixIOBus bus;
    if (!bus.Init()) return false;

    // Setting up Everloop
    matrix_hal::EverloopImage everloop_image(bus.MatrixLeds());
    matrix_hal::Everloop everloop;
    everloop.Setup(&bus);

    // Setting up NFC
    matrix_hal::NFC nfc;
    matrix_hal::NFCData nfc_data;

    /////////////////
    // MAIN CODE //
    ///////////////

    std::cout << "Scan ON/OFF Tag" << std::endl;

    // Loop until a tag is detected and the info from it is read
    while (true) {
        if (nfc_data.info.recently_updated) break;
        nfc.Activate();
        nfc.ReadInfo(&nfc_data.info);
        nfc.Deactivate();
    }

    // Get the UID from the tag
    std::string switchUID = nfc_data.info.UIDToHex();

    std::cout << "ON/OFF Tag Scanned!" << std::endl;

    float counter = 0;
    const float freq = 0.375;

    std::cout << "Scan the same tag to update rainbow." << std::endl;

    do {
        // Scan NFC tag and read info into nfc_data.info
        auto start = std::chrono::steady_clock::now();
        nfc.Activate();
        nfc.ReadInfo(&nfc_data.info);
        nfc.Deactivate();
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Update the rainbow on everloop only if a a tag was detected and the
        // UID of the detected tag matches the initial scanned tag.
        if (nfc_data.info.recently_updated) {
            std::string currUID = nfc_data.info.UIDToHex();
            if (switchUID == currUID) {
                for (matrix_hal::LedValue &led : everloop_image.leds) {
                    // Sine waves 120 degrees out of phase for rainbow
                    led.red =
                        (std::sin(freq * counter + (M_PI / 180 * 240)) * 155 +
                         100) /
                        10;
                    led.green =
                        (std::sin(freq * counter + (M_PI / 180 * 120)) * 155 +
                         100) /
                        10;
                    led.blue = (std::sin(freq * counter + 0) * 155 + 100) / 10;
                    counter += 0.51;
                }
            }
        }

        // Update the Everloop
        everloop.Write(&everloop_image);

        // Sleep for a reasonable amount of time
        std::this_thread::sleep_for(std::chrono::microseconds(5000));
    } while (true);

    return 0;
}