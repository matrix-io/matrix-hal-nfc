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
// For vector
#include <vector>

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

    int status;
    bool prompt_user = true;
    int page_number = 0;
    std::string write_str = "";
    std::vector<uint8_t> write_data;

    do {
        // Ask user for new data to write
        if (prompt_user) {
            std::cout << "Enter Page to Write/Read From (Integer)" << std::endl;
            std::cin >> page_number;
            std::cout << "Enter String to Write (one page is 4 characters)"
                      << std::endl;
            std::cin >> write_str;
            // Convert the string to a vector
            write_data =
                std::vector<uint8_t>(write_str.begin(), write_str.end());
            prompt_user = false;
            std::cout << "Scan a Mifare Ultralight or NTAG" << std::endl;
        }

        // Wait until an NFC tag is activated
        do {
            status = nfc.Activate();
        } while (matrix_hal::NFCStatus(status) != "Activation Done");

        // Read the specified page from the NFC tag
        std::vector<uint8_t> read_page = nfc.mful.ReadPage(page_number);
        if (read_page.empty()) std::cout << "Error Reading" << std::endl;

        std::cout << "Page: " << page_number << " | Before Write: "
                  << matrix_hal::PagesContent::BytesToString(read_page)
                  << std::endl;

        // Write the specified page to the NFC tag
        status = nfc.mful.WritePage(page_number, write_data);

        // If the write succeeded then prompt the user for a new write next loop
        // iteration and set everloop to green, else set everloop to off
        if (matrix_hal::NFCStatus(status) == "Success") {
            prompt_user = true;
            for (matrix_hal::LedValue &led : everloop_image.leds) {
                led.red = 0;
                led.green = 20;
                led.blue = 0;
                led.white = 0;
            }
        } else {
            if (matrix_hal::NFCStatus(status) ==
                "Incorrect Card Type For Function") {
                std::cout
                    << "This example only supports Mifare Ultralight and NTAG "
                       "cards"
                    << std::endl;
            }
            for (matrix_hal::LedValue &led : everloop_image.leds) {
                led.red = 0;
                led.green = 0;
                led.blue = 0;
                led.white = 0;
            }
        }

        // Read the specified page from the NFC tag
        read_page = nfc.mful.ReadPage(page_number);
        if (read_page.empty()) std::cout << "Error Reading" << std::endl;

        std::cout << "Page: " << page_number << " | After Write: "
                  << matrix_hal::PagesContent::BytesToString(read_page)
                  << std::endl
                  << std::endl;

        nfc.Deactivate();

        // Update the Everloop
        everloop.Write(&everloop_image);

        // Sleep for a reasonable amount of time
        std::this_thread::sleep_for(std::chrono::microseconds(10000));
    } while (true);

    return 0;
}