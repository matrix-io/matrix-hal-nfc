#include "ndef.h"

namespace matrix_hal {
int NDEF::ReadNDEF(NFC_NDEF *nfc_ndef) {
    nfc_ndef->recently_updated = false;
    uint16_t tag_tech_type = 0;
    uint8_t top_tag_type = 0;
    uint8_t top_tag_state = 0;
    uint16_t ndef_length = 0;
    nfc_init->nfc_lib_status = phacDiscLoop_GetConfig(
        nfc_init->discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
        &tag_tech_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_init->nfc_lib_status;
    nfc_ndef->Reset();
    top_tag_type = nfc_utility->ExportTag(tag_tech_type, nullptr);
    /* Configure Top layer for specified tag type */
    nfc_init->nfc_lib_status = phalTop_SetConfig(
        &nfc_init->tag_operations, PHAL_TOP_CONFIG_TAG_TYPE, top_tag_type);
    if (top_tag_type == PHAL_TOP_TAG_TYPE_T4T_TAG) {
        // NXP does this before reading NDEF from a T4 Tag, this may possibly
        // not be correct...
        uint8_t CID_enabled = 0;
        uint8_t CID = 0;
        uint8_t NAD_supported = 0;
        uint8_t NAD = 0;
        uint8_t FWI = 0;
        uint8_t FSDI = 0;
        uint8_t FSCI = 0;
        phpalI14443p4a_Sw_GetProtocolParams(
            (phpalI14443p4a_Sw_DataParams_t *)
                nfc_init->discovery_loop->pPal1443p4aDataParams,
            &CID_enabled, &CID, &NAD_supported, &FWI, &FSDI, &FSCI);
        phpalI14443p4_Sw_SetProtocol(
            (phpalI14443p4_Sw_DataParams_t *)
                nfc_init->discovery_loop->pPal14443p4DataParams,
            CID_enabled, CID, NAD_supported, NAD, FWI, FSDI, FSCI);
    }

    /* Check for NDEF presence */
    nfc_init->nfc_lib_status =
        phalTop_CheckNdef(&nfc_init->tag_operations, &top_tag_state);
    if ((top_tag_state == PHAL_TOP_STATE_READONLY) ||
        (top_tag_state == PHAL_TOP_STATE_READWRITE)) {
        nfc_ndef->ndef_detected = true;
        // Read NDEF message
        nfc_init->nfc_lib_status = phalTop_ReadNdef(
            &nfc_init->tag_operations, nfc_init->data_buffer, &ndef_length);
        nfc_ndef->read_ndef = std::vector<uint8_t>(
            nfc_init->data_buffer, nfc_init->data_buffer + ndef_length);
    }
    // TODO: Fix error handling
    return -nfc_init->nfc_lib_status;
}

int NDEF::WriteNDEF(NFC_NDEF *nfc_ndef) {
    uint16_t tag_tech_type = 0;
    uint8_t top_tag_type = 0;
    uint8_t top_tag_state = 0;
    uint16_t ndef_length = 0;
    nfc_init->nfc_lib_status = phacDiscLoop_GetConfig(
        nfc_init->discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
        &tag_tech_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_init->nfc_lib_status;
    top_tag_type = nfc_utility->ExportTag(tag_tech_type, nullptr);
    /* Configure Top layer for specified tag type */
    nfc_init->nfc_lib_status = phalTop_SetConfig(
        &nfc_init->tag_operations, PHAL_TOP_CONFIG_TAG_TYPE, top_tag_type);
    /* Check for NDEF presence */
    nfc_init->nfc_lib_status =
        phalTop_CheckNdef(&nfc_init->tag_operations, &top_tag_state);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS) {
        // Format NDEF Required
        nfc_init->nfc_lib_status =
            phalTop_FormatNdef(&nfc_init->tag_operations);
    }
    nfc_init->nfc_lib_status =
        phalTop_WriteNdef(&nfc_init->tag_operations, nfc_ndef->read_ndef.data(),
                          nfc_ndef->read_ndef.size());

    return 0;
}

int NDEF::EraseNDEF() {
    uint16_t tag_tech_type = 0;
    uint8_t top_tag_type = 0;
    uint8_t top_tag_state = 0;
    uint16_t ndef_length = 0;
    nfc_init->nfc_lib_status = phacDiscLoop_GetConfig(
        nfc_init->discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
        &tag_tech_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_init->nfc_lib_status;
    top_tag_type = nfc_utility->ExportTag(tag_tech_type, nullptr);
    /* Configure Top layer for specified tag type */
    nfc_init->nfc_lib_status = phalTop_SetConfig(
        &nfc_init->tag_operations, PHAL_TOP_CONFIG_TAG_TYPE, top_tag_type);
    /* Check for NDEF presence */
    nfc_init->nfc_lib_status =
        phalTop_CheckNdef(&nfc_init->tag_operations, &top_tag_state);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS) {
        // Format NDEF Required
        nfc_init->nfc_lib_status =
            phalTop_FormatNdef(&nfc_init->tag_operations);
    }
    nfc_init->nfc_lib_status = phalTop_EraseNdef(&nfc_init->tag_operations);
    return 0;
}
}  // namespace matrix_hal