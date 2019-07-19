#include <chrono>
#include <iostream>
#include <thread>

#include "matrix_hal/everloop.h"
#include "matrix_hal/everloop_image.h"
#include "matrix_hal/matrixio_bus.h"

#include "matrix_nfc/nfc_data.h"
#include "matrix_nfc/nfc_sensor.h"

using std::cin;
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
    hal::NFCData nfc_data;

    bool write_success = false;

    int page_number;
    cout << "Enter Page to Write/Read From" << endl;
    cin >> page_number;
    std::string write_str;
    cout << "Enter String to Write (one page is 4 characters)" << endl;
    cin >> write_str;
    std::vector<uint8_t> write_data =
        std::vector<uint8_t>(write_str.begin(), write_str.end());
    cout << "Scan a Mifare Ultralight or NTAG" << endl;

    do {
        if (write_success) {  // If write succeeded then ask user for new write
            cout << "Enter Page to Write/Read From (Integer)" << endl;
            cin >> page_number;
            cout << "Enter String to Write (one page is 4 characters)" << endl;
            cin >> write_str;
            write_data =
                std::vector<uint8_t>(write_str.begin(), write_str.end());
            write_success = false;
        }

        int status = nfc_sensor.Activate();
        if (hal::DescStatus(status) == "Activation Done") {
            std::vector<uint8_t> read_page =
                nfc_sensor.mful.ReadPage(page_number);
            if (read_page.empty()) cout << "Error Reading" << endl;
            cout << "Page: " << page_number
                 << " | Before Write: " << hal::NFCData::BytesToChar(read_page)
                 << endl;
            status = nfc_sensor.mful.WritePage(page_number, write_data);
            if (hal::DescStatus(status) == "Incorrect Card Type For Function") {
                cout << "This example only supports Mifare Ultralight and NTAG "
                        "cards"
                     << endl;
                nfc_sensor.Deactivate();
            }
            read_page = nfc_sensor.mful.ReadPage(page_number);
            cout << "Page: " << page_number
                 << " | After Write: " << hal::NFCData::BytesToChar(read_page)
                 << endl
                 << endl;
            write_success = true;
            nfc_sensor.Deactivate();
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

        everloop.Write(&everloop_image);  // Write to everloop

        std::this_thread::sleep_for(std::chrono::microseconds(10000));
    } while (true);

    return 0;
}