#include "mful.h"

namespace matrix_hal {

std::vector<uint8_t> MFUL::ReadPage(uint8_t page_number,
                                    uint8_t size_to_return) {
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
                                nfc_init->data_buffer + size_to_return);
}

/* This will read a single specified page (Card must already have been
 * activated!) If card is not activated or read fails the vector will be
 * empty. page_number can be 0x00 - 0xFF depending upon card layout
 */
std::vector<uint8_t> MFUL::ReadPage(uint8_t page_number) {
    return ReadPage(page_number, 4);
}

/* page_number can be 0x00 - 0xFF depending upon card layout */
int MFUL::WritePage(uint8_t page_number, std::vector<uint8_t> &write_data) {
    // Check if card activated
    if (nfc_init->peer_info.dwActivatedType == 0) return CARD_NOT_ACTIVATED;
    // Check if card is a MFUL or NTAG
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

int MFUL::ReadPages(PagesContent *pages_content) {
    pages_content->recently_updated = false;
    // Check if card activated
    if (nfc_init->peer_info.dwActivatedType == 0) return CARD_NOT_ACTIVATED;
    // Check if card is a MFUL or NTAG
    if (nfc_init->peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
        return INCORRECT_CARD_FOR_FUNCTION;
    pages_content->Reset();
    uint8_t page_number = 0;
    std::vector<uint8_t> read_buffer;

    do {
        if (!read_buffer.empty()) {
            // The vector must be 16 characters long, split the vector into four
            // equal parts.
            auto curr = read_buffer.begin();
            while (std::next(curr, 4) != read_buffer.end()) {
                auto next = std::next(curr, 4);
                std::vector<uint8_t> page = std::vector<uint8_t>(curr, next);
                // Pages loop around, if first page is read again then the read
                // is complete.
                if (pages_content->content.size() > 0 &&
                    page == pages_content->content[0]) {
                    pages_content->read_complete = true;
                    return PAGES_READ_FINISHED_SUCCESS;
                }
                pages_content->content.emplace_back(page);
                curr = next;
            }
            // Check the last part of the read vector, if is equal to first page
            // then read is complete.
            std::vector<uint8_t> page =
                std::vector<uint8_t>(curr, std::next(curr, 4));
            if (pages_content->content.size() > 0 &&
                page == pages_content->content[0]) {
                pages_content->read_complete = true;
                return PAGES_READ_FINISHED_SUCCESS;
            }
            read_buffer.clear();
        }
        read_buffer = ReadPage(page_number, 16);
        page_number += 3;
    } while (!read_buffer.empty());
    return PAGES_READ_UNFINISHED_ABORTED;
}
}  // namespace matrix_hal