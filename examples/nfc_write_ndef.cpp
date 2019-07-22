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

#include "matrix_nfc/NdefMessage.h"

using std::cout;
using std::endl;

namespace hal = matrix_hal;

int writeToNewTag(hal::NFC &nfc, hal::NFCData &nfc_data) {
    cout << "Press enter to write" << endl;
    char temp = 'x';
    while (temp != '\n') std::cin.get(temp);
    nfc.Activate();
    NdefMessage message = NdefMessage();
    message.addUriRecord("http://docs.matrix.one");
    cout << message.toString() << endl;

    nfc_data.ndef.content = std::vector<uint8_t>(message.getEncodedSize());
    message.encode(nfc_data.ndef.content.data());
    nfc.ndef.EraseNDEF();
    nfc.ndef.WriteNDEF(&nfc_data.ndef);
    cout << "Wrote to new Tag" << endl;
    nfc.Deactivate();
    return 0;
}

int main() {
    hal::MatrixIOBus bus;

    if (!bus.Init()) return false;

    hal::EverloopImage everloop_image(bus.MatrixLeds());

    hal::Everloop everloop;

    everloop.Setup(&bus);

    hal::NFC nfc;
    hal::NFCData nfc_data;

    cout << "Scan a Tag, Any Tag!" << endl;

    do {
        nfc.Activate();
        nfc.ndef.ReadNDEF(&nfc_data.ndef);
        nfc.Deactivate();

        if (nfc_data.ndef.recently_updated) {
            cout << nfc_data.ndef.ToString() << endl;
            cout << endl;
            cout << nfc_data.ndef.ToHex() << endl;
            writeToNewTag(nfc, nfc_data);
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