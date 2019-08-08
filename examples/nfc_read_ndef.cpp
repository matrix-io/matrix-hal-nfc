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

    std::cout << "Scan a tag to read NDEF!" << std::endl;

    do {
        // Scan NFC tag and read NDEF into nfc_data.ndef
        nfc.Activate();
        nfc.ndef.Read(&nfc_data.ndef);
        nfc.Deactivate();

        // If tag was detected output scanned tag's NDEF and set everloop to
        // green, else set everloop to off.
        if (nfc_data.ndef.recently_updated) {
            // Only output NDEF if the scanned tag was NDEF formatted
            if (nfc_data.ndef.valid) {
                std::cout << std::endl;
                std::cout << "String:\n"
                          << nfc_data.ndef.ToString() << std::endl;
                std::cout << "Hex:\n" << nfc_data.ndef.ToHex() << std::endl;
                std::cout << "Output from NDEF parser:" << std::endl;
                matrix_hal::NDEFParser ndef_parser =
                    matrix_hal::NDEFParser(&nfc_data.ndef);
                std::cout << ndef_parser.ToString() << std::endl;
            }
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