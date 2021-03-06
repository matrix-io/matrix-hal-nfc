#include "ndef.h"

namespace matrix_hal {
int NDEF::Read(NDEFContent *ndef_content) {
    ndef_content->recently_updated = false;
    // Check if card activated
    if (nfc_init->peer_info.dwActivatedType == 0) return CARD_NOT_ACTIVATED;
    uint16_t tag_tech_type = 0;
    uint8_t top_tag_type = 0;
    uint8_t top_tag_state = 0;
    uint16_t ndef_length = 0;
    nfc_init->nfc_lib_status = phacDiscLoop_GetConfig(
        nfc_init->discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
        &tag_tech_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_init->nfc_lib_status;
    ndef_content->Reset();
    top_tag_type = nfc_utility->ExportTag(tag_tech_type, nullptr);
    /* Configure Top layer for specified tag type */
    nfc_init->nfc_lib_status = phalTop_SetConfig(
        &nfc_init->tag_operations, PHAL_TOP_CONFIG_TAG_TYPE, top_tag_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
        return -nfc_init->nfc_lib_status;
    if (top_tag_type == PHAL_TOP_TAG_TYPE_T4T_TAG) {
        // NXP does this before reading NDEF from a T4 Tag, this may
        // possibly not be correct...
        uint8_t CID_enabled = 0;
        uint8_t CID = 0;
        uint8_t NAD_supported = 0;
        uint8_t NAD = 0;
        uint8_t FWI = 0;
        uint8_t FSDI = 0;
        uint8_t FSCI = 0;
        nfc_init->nfc_lib_status = phpalI14443p4a_Sw_GetProtocolParams(
            (phpalI14443p4a_Sw_DataParams_t *)
                nfc_init->discovery_loop->pPal1443p4aDataParams,
            &CID_enabled, &CID, &NAD_supported, &FWI, &FSDI, &FSCI);
        if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
            return -nfc_init->nfc_lib_status;
        nfc_init->nfc_lib_status = phpalI14443p4_Sw_SetProtocol(
            (phpalI14443p4_Sw_DataParams_t *)
                nfc_init->discovery_loop->pPal14443p4DataParams,
            CID_enabled, CID, NAD_supported, NAD, FWI, FSDI, FSCI);
        if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
            return -nfc_init->nfc_lib_status;
    }

    /* Check for NDEF presence */
    nfc_init->nfc_lib_status =
        phalTop_CheckNdef(&nfc_init->tag_operations, &top_tag_state);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
        return -nfc_init->nfc_lib_status;
    if ((top_tag_state == PHAL_TOP_STATE_READONLY) ||
        (top_tag_state == PHAL_TOP_STATE_READWRITE)) {
        ndef_content->valid = true;
        // Read NDEF message
        nfc_init->nfc_lib_status = phalTop_ReadNdef(
            &nfc_init->tag_operations, nfc_init->data_buffer, &ndef_length);
        if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
            return -nfc_init->nfc_lib_status;
        ndef_content->content = std::vector<uint8_t>(
            nfc_init->data_buffer, nfc_init->data_buffer + ndef_length);
    }
    return -nfc_init->nfc_lib_status;
}

int NDEF::Write(NDEFContent *ndef_content) {
    // Check if card activated
    if (nfc_init->peer_info.dwActivatedType == 0) return CARD_NOT_ACTIVATED;
    uint16_t tag_tech_type = 0;
    uint8_t top_tag_type = 0;
    uint8_t top_tag_state = 0;
    nfc_init->nfc_lib_status = phacDiscLoop_GetConfig(
        nfc_init->discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
        &tag_tech_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_init->nfc_lib_status;
    top_tag_type = nfc_utility->ExportTag(tag_tech_type, nullptr);
    /* Configure Top layer for specified tag type */
    nfc_init->nfc_lib_status = phalTop_SetConfig(
        &nfc_init->tag_operations, PHAL_TOP_CONFIG_TAG_TYPE, top_tag_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
        return -nfc_init->nfc_lib_status;
    /* Check for NDEF presence */
    nfc_init->nfc_lib_status =
        phalTop_CheckNdef(&nfc_init->tag_operations, &top_tag_state);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS) {
        // Format NDEF Required
        nfc_init->nfc_lib_status =
            phalTop_FormatNdef(&nfc_init->tag_operations);
        if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
            return -nfc_init->nfc_lib_status;
    }
    nfc_init->nfc_lib_status = phalTop_WriteNdef(&nfc_init->tag_operations,
                                                 ndef_content->content.data(),
                                                 ndef_content->content.size());
    return -nfc_init->nfc_lib_status;
}

int NDEF::Write(NDEFParser *ndef_parser) {
    NDEFContent ndef_content;
    ndef_content.content = std::vector<uint8_t>(ndef_parser->GetEncodedSize());
    ndef_parser->Encode(ndef_content.content.data());
    return Write(&ndef_content);
}

int NDEF::Erase() {
    // Check if card activated
    if (nfc_init->peer_info.dwActivatedType == 0) return CARD_NOT_ACTIVATED;
    uint16_t tag_tech_type = 0;
    uint8_t top_tag_type = 0;
    uint8_t top_tag_state = 0;
    nfc_init->nfc_lib_status = phacDiscLoop_GetConfig(
        nfc_init->discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
        &tag_tech_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_init->nfc_lib_status;
    top_tag_type = nfc_utility->ExportTag(tag_tech_type, nullptr);
    /* Configure Top layer for specified tag type */
    nfc_init->nfc_lib_status = phalTop_SetConfig(
        &nfc_init->tag_operations, PHAL_TOP_CONFIG_TAG_TYPE, top_tag_type);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
        return -nfc_init->nfc_lib_status;
    /* Check for NDEF presence */
    nfc_init->nfc_lib_status =
        phalTop_CheckNdef(&nfc_init->tag_operations, &top_tag_state);
    if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS) {
        // Format NDEF Required
        nfc_init->nfc_lib_status =
            phalTop_FormatNdef(&nfc_init->tag_operations);
        if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS)
            return -nfc_init->nfc_lib_status;
    }
    nfc_init->nfc_lib_status = phalTop_EraseNdef(&nfc_init->tag_operations);
    return -nfc_init->nfc_lib_status;
}
}  // namespace matrix_hal