#include "mful.h"

namespace matrix_hal {

/* This will read a single specified page (Card must already have been
 * activated!) If card is not activated or read fails the vector will be
 * empty. page_number can be 0x00 - 0xFF depending upon card layout
 */
std::vector<uint8_t> MFUL::ReadPage(uint8_t page_number) {
    if (nfc_init->peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
        return std::vector<uint8_t>();
    nfc_init->nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber =
        page_number;
    nfc_init->nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand = MFUL_Read;
    nfc_init->nfc_lib_status =
        phNfcLib_Transmit(&nfc_init->nfc_lib_transmit, 0x0);

    if (nfc_init->nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        // Read for Block failed...
        return std::vector<uint8_t>();
    }
    /* This parameter has to be reset before every receive */
    uint16_t bytes_read = 256;
    /* To perform receive operation to get back the read data */
    nfc_init->nfc_lib_status = phNfcLib_Receive(
        &nfc_init->data_buffer[0], &bytes_read, &nfc_init->more_data_available);

    /* The status should be success and the number of bytes received should
     * be 16 for MFUL/NTAG cards */
    if ((nfc_init->nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) ||
        (bytes_read != 16)) {
        // cerr << "Read for Block " << +page_number << " failed..." <<
        // endl;
        return std::vector<uint8_t>();
    }
    return std::vector<uint8_t>(nfc_init->data_buffer,
                                nfc_init->data_buffer + 4);
}

/* page_number can be 0x00 - 0xFF depending upon card layout */
int MFUL::WritePage(uint8_t page_number, std::vector<uint8_t> &write_data) {
    // Check if card activated and is a MFUL or NTAG
    if (nfc_init->peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
        return INCORRECT_CARD_FOR_FUNCTION;
    nfc_init->nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber =
        page_number;
    nfc_init->nfc_lib_transmit.phNfcLib_MifareUltraLight.pBuffer =
        write_data.data();
    nfc_init->nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand = MFUL_Write;
    /* Transmit will execute the command */
    nfc_init->nfc_lib_status = phNfcLib_Transmit(
        &nfc_init->nfc_lib_transmit, 0x04 /* For MFUL/NTAG the size of a
                                         page to be written is 4 bytes */
    );
    return nfc_init->nfc_lib_status;
}

int MFUL::ReadData(NFCData *nfc_data) {
    nfc_data->recently_updated = false;
    // Check if card activated and is a MFUL or NTAG
    if (nfc_init->peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
        return INCORRECT_CARD_FOR_FUNCTION;
    nfc_data->Reset();
    uint8_t page_number = 0;
    std::vector<uint8_t> read_buffer;

    do {
        if (!read_buffer.empty()) {
            nfc_data->read_data.emplace_back(read_buffer);
            read_buffer.clear();
        }
        read_buffer = ReadPage(page_number);
        ++page_number;
    } while (!read_buffer.empty());
    return PH_NFCLIB_STATUS_SUCCESS;
}
}  // namespace matrix_hal