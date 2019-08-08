/////////////////////////
// INCLUDE STATEMENTS //
///////////////////////

// For console output
#include <iostream>
// For sleep
#include <chrono>
#include <thread>
// For string
#include <string>
// For set
#include <set>

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

    // Set used to check for uniqueness
    std::set<std::string> UID_set;

    // Strings to store the unique UID for three tags.
    std::string green_UID = "";
    std::string blue_UID = "";
    std::string red_UID = "";

    // Scan red tag, using a set to ensure that the UID has not been scanned
    // before. Loop until a unique UID has been scanned.
    std::cout << "Scan Red Tag" << std::endl;
    while (true) {
        nfc.Activate();
        nfc.ReadInfo(&nfc_data.info);
        nfc.Deactivate();
        red_UID = nfc_data.info.UIDToHex();
        // If the find function returns UID_set.end() then the UID was not
        // previously read.
        if (nfc_data.info.recently_updated &&
            UID_set.find(red_UID) == UID_set.end())
            break;
    }
    UID_set.insert(red_UID);

    // Scan green tag, using a set to ensure that the UID has not been scanned
    // before. Loop until a unique UID has been scanned.
    std::cout << "Scan Green Tag" << std::endl;
    while (true) {
        nfc.Activate();
        nfc.ReadInfo(&nfc_data.info);
        nfc.Deactivate();
        green_UID = nfc_data.info.UIDToHex();
        // If the find function returns UID_set.end() then the UID was not
        // previously read.
        if (nfc_data.info.recently_updated &&
            UID_set.find(green_UID) == UID_set.end())
            break;
    }
    UID_set.insert(green_UID);

    // Scan blue tag, using a set to ensure that the UID has not been scanned
    // before. Loop until a unique UID has been scanned.
    std::cout << "Scan Blue Tag" << std::endl;
    while (true) {
        nfc.Activate();
        nfc.ReadInfo(&nfc_data.info);
        nfc.Deactivate();
        blue_UID = nfc_data.info.UIDToHex();
        // If the find function returns UID_set.end() then the UID was not
        // previously read.
        if (nfc_data.info.recently_updated &&
            UID_set.find(blue_UID) == UID_set.end())
            break;
    }
    UID_set.insert(blue_UID);

    std::cout << "\nScan specified tags to activate Everloop" << std::endl;

    do {
        // Scan NFC tag and read info into nfc_data.info
        nfc.Activate();
        nfc.ReadInfo(&nfc_data.info);
        nfc.Deactivate();

        // If tag was detected set everloop to the color that matches the
        // scanned tag's UID, else set everloop to off.
        if (nfc_data.info.recently_updated) {
            std::string curr_UID = nfc_data.info.UIDToHex();
            if (curr_UID == red_UID) {
                for (matrix_hal::LedValue &led : everloop_image.leds) {
                    led.red = 50;
                    led.green = 0;
                    led.blue = 0;
                    led.white = 0;
                }
            } else if (curr_UID == green_UID) {
                for (matrix_hal::LedValue &led : everloop_image.leds) {
                    led.red = 0;
                    led.green = 50;
                    led.blue = 0;
                    led.white = 0;
                }
            } else if (curr_UID == blue_UID) {
                for (matrix_hal::LedValue &led : everloop_image.leds) {
                    led.red = 0;
                    led.green = 0;
                    led.blue = 50;
                    led.white = 0;
                }
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