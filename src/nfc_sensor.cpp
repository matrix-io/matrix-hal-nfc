// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc_sensor.h"

using std::cerr;
using std::cout;
using std::endl;

namespace matrix_hal {
NFCSensor::NFCSensor() {}

NFCSensor::~NFCSensor() {}

int NFCSensor::Activate() {
    nfc_init.nfc_lib_status =
        phNfcLib_Activate(nfc_init.technology_mask, &nfc_init.peer_info, NULL);
    return nfc_init.nfc_lib_status;
}

int NFCSensor::Deactivate() {
    nfc_init.nfc_lib_status = phNfcLib_Deactivate(
        PH_NFCLIB_DEACTIVATION_MODE_RELEASE, &nfc_init.peer_info);
    if (nfc_init.nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        /* NFC Tag Deactivate with Release Mode failed, card was removed
        from vicinity...
        Performing Deactivate with RF OFF mode */
        phNfcLib_Deactivate(PH_NFCLIB_DEACTIVATION_MODE_RF_OFF,
                            &nfc_init.peer_info);
    }
    return nfc_init.nfc_lib_status;
}

/* This will read and populate info data (Card must already have been
 * activated!)
 */
int NFCSensor::ReadInfo(NFCInfo *nfc_info) {
    nfc_info->recently_updated = false;
    uint16_t tag_tech_type = 0;
    // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
    nfc_init.nfc_lib_status = phacDiscLoop_GetConfig(
        nfc_init.discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
        &tag_tech_type);
    if (nfc_init.nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_init.nfc_lib_status;
    nfc_utility.ExportTag(tag_tech_type, nfc_info);
    return nfc_init.nfc_lib_status;
}

/* This will activate, read and populate info data, then deactivate card */
int NFCSensor::SimpleReadInfo(NFCInfo *nfc_info) {
    nfc_info->recently_updated = false;
    nfc_init.nfc_lib_status = Activate();
    if (nfc_init.nfc_lib_status != PH_NFCLIB_STATUS_PEER_ACTIVATION_DONE)
        return nfc_init.nfc_lib_status;
    nfc_init.nfc_lib_status = ReadInfo(nfc_info);
    // If already errored at ReadInfo then return the error from ReadInfo
    if (nfc_init.nfc_lib_status < 0)
        Deactivate();
    else
        nfc_init.nfc_lib_status = Deactivate();
    return nfc_init.nfc_lib_status;
}

// int NFCSensor::ReadNDEF(NFC_NDEF *nfc_ndef) {
//     nfc_ndef->recently_updated = false;
//     uint16_t tag_tech_type = 0;
//     uint8_t top_tag_type = 0;
//     uint8_t top_tag_state = 0;
//     uint16_t ndef_length = 0;
//     nfc_init.nfc_lib_status = phacDiscLoop_GetConfig(
//         nfc_init.discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
//         &tag_tech_type);
//     if (nfc_init.nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
//         return -nfc_init.nfc_lib_status;
//     nfc_ndef->Reset();
//     top_tag_type = ExportTag(tag_tech_type, nullptr);
//     /* Configure Top layer for specified tag type */
//     nfc_init.nfc_lib_status = phalTop_SetConfig(&tag_operations,
//                                        PHAL_TOP_CONFIG_TAG_TYPE,
//                                        top_tag_type);
//     if (top_tag_type == PHAL_TOP_TAG_TYPE_T4T_TAG) {
//         // NXP does this before reading NDEF from a T4 Tag, this may possibly
//         // not be correct...
//         uint8_t CID_enabled = 0;
//         uint8_t CID = 0;
//         uint8_t NAD_supported = 0;
//         uint8_t NAD = 0;
//         uint8_t FWI = 0;
//         uint8_t FSDI = 0;
//         uint8_t FSCI = 0;
//         phpalI14443p4a_Sw_GetProtocolParams(
//             (phpalI14443p4a_Sw_DataParams_t *)
//                 nfc_init.discovery_loop->pPal1443p4aDataParams,
//             &CID_enabled, &CID, &NAD_supported, &FWI, &FSDI, &FSCI);
//         phpalI14443p4_Sw_SetProtocol((phpalI14443p4_Sw_DataParams_t *)
//                                          nfc_init.discovery_loop->pPal14443p4DataParams,
//                                      CID_enabled, CID, NAD_supported, NAD,
//                                      FWI, FSDI, FSCI);
//     }

//     /* Check for NDEF presence */
//     nfc_init.nfc_lib_status = phalTop_CheckNdef(&tag_operations,
//     &top_tag_state); if ((top_tag_state == PHAL_TOP_STATE_READONLY) ||
//         (top_tag_state == PHAL_TOP_STATE_READWRITE)) {
//         nfc_ndef->ndef_detected = true;
//         // Read NDEF message
//         nfc_init.nfc_lib_status =
//             phalTop_ReadNdef(&tag_operations, data_buffer, &ndef_length);
//         nfc_ndef->read_ndef =
//             std::vector<uint8_t>(data_buffer, data_buffer + ndef_length);
//     }
//     // TODO: Fix error handling
//     return -nfc_init.nfc_lib_status;
// }

// int NFCSensor::WriteNDEF(NFC_NDEF *nfc_ndef) {
//     uint16_t tag_tech_type = 0;
//     uint8_t top_tag_type = 0;
//     uint8_t top_tag_state = 0;
//     uint16_t ndef_length = 0;
//     nfc_init.nfc_lib_status = phacDiscLoop_GetConfig(
//         nfc_init.discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
//         &tag_tech_type);
//     if (nfc_init.nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
//         return -nfc_init.nfc_lib_status;
//     top_tag_type = ExportTag(tag_tech_type, nullptr);
//     /* Configure Top layer for specified tag type */
//     nfc_init.nfc_lib_status = phalTop_SetConfig(&tag_operations,
//                                        PHAL_TOP_CONFIG_TAG_TYPE,
//                                        top_tag_type);
//     /* Check for NDEF presence */
//     nfc_init.nfc_lib_status = phalTop_CheckNdef(&tag_operations,
//     &top_tag_state); if (nfc_init.nfc_lib_status != PH_ERR_SUCCESS) {
//         // Format NDEF Required
//         nfc_init.nfc_lib_status = phalTop_FormatNdef(&tag_operations);
//     }
//     nfc_init.nfc_lib_status =
//         phalTop_WriteNdef(&tag_operations, nfc_ndef->read_ndef.data(),
//                           nfc_ndef->read_ndef.size());

//     return 0;
// }

// int NFCSensor::EraseNDEF() {
//     uint16_t tag_tech_type = 0;
//     uint8_t top_tag_type = 0;
//     uint8_t top_tag_state = 0;
//     uint16_t ndef_length = 0;
//     nfc_init.nfc_lib_status = phacDiscLoop_GetConfig(
//         nfc_init.discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
//         &tag_tech_type);
//     if (nfc_init.nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
//         return -nfc_init.nfc_lib_status;
//     top_tag_type = ExportTag(tag_tech_type, nullptr);
//     /* Configure Top layer for specified tag type */
//     nfc_init.nfc_lib_status = phalTop_SetConfig(&tag_operations,
//                                        PHAL_TOP_CONFIG_TAG_TYPE,
//                                        top_tag_type);
//     /* Check for NDEF presence */
//     nfc_init.nfc_lib_status = phalTop_CheckNdef(&tag_operations,
//     &top_tag_state); if (nfc_init.nfc_lib_status != PH_ERR_SUCCESS) {
//         // Format NDEF Required
//         nfc_init.nfc_lib_status = phalTop_FormatNdef(&tag_operations);
//     }
//     nfc_init.nfc_lib_status = phalTop_EraseNdef(&tag_operations);
//     return 0;
// }

// /* This will read a single specified page (Card must already have been
//  * activated!) If card is not activated or read fails the vector will be
//  * empty. page_number can be 0x00 - 0xFF depending upon card layout
//  */
// std::vector<uint8_t> NFCSensor::MFUL::ReadPage(uint8_t page_number) {
//     if (nfc_sensor->peer_info.dwActivatedType !=
//     E_PH_NFCLIB_MIFARE_ULTRALIGHT)
//         return std::vector<uint8_t>();
//     nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber =
//         page_number;
//     nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand =
//     MFUL_Read; nfc_sensor->nfc_init.nfc_lib_status =
//         phNfcLib_Transmit(&nfc_sensor->nfc_lib_transmit, 0x0);

//     if (nfc_sensor->nfc_init.nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
//         // Read for Block failed...
//         return std::vector<uint8_t>();
//     }
//     /* This parameter has to be reset before every receive */
//     uint16_t bytes_read = 256;
//     /* To perform receive operation to get back the read data */
//     nfc_sensor->nfc_init.nfc_lib_status =
//         phNfcLib_Receive(&nfc_sensor->data_buffer[0], &bytes_read,
//                          &nfc_sensor->more_data_available);

//     /* The status should be success and the number of bytes received should
//      * be 16 for MFUL/NTAG cards */
//     if ((nfc_sensor->nfc_init.nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) ||
//         (bytes_read != 16)) {
//         // cerr << "Read for Block " << +page_number << " failed..." <<
//         // endl;
//         return std::vector<uint8_t>();
//     }
//     return std::vector<uint8_t>(nfc_sensor->data_buffer,
//                                 nfc_sensor->data_buffer + 4);
// }

// /* page_number can be 0x00 - 0xFF depending upon card layout */
// int NFCSensor::MFUL::WritePage(uint8_t page_number,
//                                std::vector<uint8_t> &write_data) {
//     // Check if card activated and is a MFUL or NTAG
//     if (nfc_sensor->peer_info.dwActivatedType !=
//     E_PH_NFCLIB_MIFARE_ULTRALIGHT)
//         return INCORRECT_CARD_FOR_FUNCTION;
//     nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber =
//         page_number;
//     nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.pBuffer =
//         write_data.data();
//     nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand =
//         MFUL_Write;
//     /* Transmit will execute the command */
//     nfc_sensor->nfc_init.nfc_lib_status = phNfcLib_Transmit(
//         &nfc_sensor->nfc_lib_transmit, 0x04 /* For MFUL/NTAG the size of a
//                                          page to be written is 4 bytes */
//     );
//     return nfc_sensor->nfc_init.nfc_lib_status;
// }

// int NFCSensor::MFUL::ReadData(NFCData *nfc_data) {
//     nfc_data->recently_updated = false;
//     // Check if card activated and is a MFUL or NTAG
//     if (nfc_sensor->peer_info.dwActivatedType !=
//     E_PH_NFCLIB_MIFARE_ULTRALIGHT)
//         return INCORRECT_CARD_FOR_FUNCTION;
//     nfc_data->Reset();
//     uint8_t page_number = 0;
//     std::vector<uint8_t> read_buffer;

//     do {
//         if (!read_buffer.empty()) {
//             nfc_data->read_data.emplace_back(read_buffer);
//             read_buffer.clear();
//         }
//         read_buffer = ReadPage(page_number);
//         ++page_number;
//     } while (!read_buffer.empty());
//     return PH_NFCLIB_STATUS_SUCCESS;
// }

// nfc_info->IC_type = "MF0ICU2";
// nfc_info->storage_size

}  // namespace matrix_hal