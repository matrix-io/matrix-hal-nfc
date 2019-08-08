/////////////////////////
// INCLUDE STATEMENTS //
///////////////////////

// For console output
#include <iostream>
// For sleep
#include <chrono>
#include <thread>

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

    std::cout << "Scan a Mifare Ultralight or NTAG." << std::endl;

    do {
        // Scan NFC tag and read pages into nfc_data.pages
        nfc.Activate();
        nfc.mful.ReadPages(&nfc_data.pages);
        nfc.Deactivate();

        // Output pages from tag and set everloop to green only if tag was
        // detected and all pages were read, else set everloop to off.
        if (nfc_data.pages.recently_updated && nfc_data.pages.read_complete) {
            std::cout << std::endl;
            std::cout << "String:\n" << nfc_data.pages.ToString() << std::endl;
            std::cout << "Hex:\n" << nfc_data.pages.ToHex() << std::endl;
            for (matrix_hal::LedValue &led : everloop_image.leds) {
                led.red = 0;
                led.green = 20;
                led.blue = 0;
                led.white = 0;
            }
        } else {
            for (matrix_hal::LedValue &led : everloop_image.leds) {
                led.red = 0;
                led.green = 0;
                led.blue = 0;
                led.white = 0;
            }
        }

        // Update the Everloop
        everloop.Write(&everloop_image);

        // Sleep for a reasonable amount of time
        std::this_thread::sleep_for(std::chrono::microseconds(10000));
    } while (true);

    return 0;
}