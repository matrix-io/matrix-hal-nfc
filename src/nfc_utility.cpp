
#include "nfc_utility.h"

namespace matrix_hal {

std::string NFCUtility::GetCardFamily() {
    switch (nfc_init->peer_info.dwActivatedType) {
        case E_PH_NFCLIB_MIFARE_CLASSIC_1K:
            return "Mifare Classic 1K";
            break;
        case E_PH_NFCLIB_MIFARE_CLASSIC_4K:
            return "Mifare Classic 4K";
            break;
        case E_PH_NFCLIB_MIFARE_ULTRALIGHT:
            return "Mifare Ultralight | NTAG";
            break;
        case E_PH_NFCLIB_MIFARE_DESFIRE:
            return "Mifare DESfire";
            break;
        case E_PH_NFCLIB_TYPEB_LAYER3:
            return "Type B Layer 3";
            break;
        case E_PH_NFCLIB_TYPEA_LAYER3:
            return "Mifare Classic | Mifare Ultralight | Type A Layer 3";
            break;
        case E_PH_NFCLIB_TYPEA_LAYER4:
            return "Mifare DESFire | Type A Layer 4";
            break;
        case E_PH_NFCLIB_TYPEB_LAYER4:
            return "Type B Layer 4";
            break;
        default:
            return "Unknown Card";
            break;
    }
}

std::pair<std::string, int> NFCUtility::GetCardIC() {
    if (nfc_init->peer_info.dwActivatedType == E_PH_NFCLIB_MIFARE_ULTRALIGHT) {
        std::vector<uint8_t> version_info = std::vector<uint8_t>(8);
        nfc_init->nfc_lib_status =
            phalMful_GetVersion(&nfc_init->al_mful, version_info.data());
        if (nfc_init->nfc_lib_status != PH_ERR_SUCCESS) {
            // First generation tag
            nfc_init->nfc_lib_status =
                phalMful_UlcAuthenticate(&nfc_init->al_mful, 0, 0);
            if ((nfc_init->nfc_lib_status == (PH_ERR_SUCCESS)) ||
                (nfc_init->nfc_lib_status == (PH_ERR_AUTH_ERROR))) {
                // Mifare Ultralight C
                return {"MF0ICU2", 144};
            } else {
                // Either Mifare Ultralight or NTAG203
                // Only NTAG203 has page 41
                bool page_41_exists = true;
                nfc_init->nfc_lib_transmit.phNfcLib_MifareUltraLight
                    .bPageNumber = 41;
                nfc_init->nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand =
                    MFUL_Read;
                nfc_init->nfc_lib_status =
                    phNfcLib_Transmit(&nfc_init->nfc_lib_transmit, 0x0);
                if (nfc_init->nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
                    // Read for Block failed...
                    page_41_exists = false;
                }
                if (page_41_exists) {
                    // Must be NTAG203
                    return {"NTAG203", 144};

                } else {
                    // Must be Mifare Ultralight
                    return {"MF0ICU1", 48};
                }
            }
        } else {
            // Tag is EV1 or later
            uint8_t type = version_info[2];
            uint8_t subtype = version_info[3];
            uint8_t major_version = version_info[4];
            uint8_t minor_version = version_info[5];
            uint8_t storage_size = version_info[6];
            std::vector<uint8_t> IC_info = {type, subtype, major_version,
                                            minor_version, storage_size};
            for (auto t : IC_list) {
                if (IC_info == std::get<0>(t)) {
                    return {std::get<1>(t), std::get<2>(t)};
                    break;
                }
            }
        }
    }
    return {"unknown", -1};
}

/* This function returns top_tag_type for use with tag_operations, it will
 * also populate nfc_info (if provided) */
uint8_t NFCUtility::ExportTag(uint16_t tag_tech_type, InfoContent *nfc_info) {
    uint8_t top_tag_type = 0;
    if (nfc_info) {
        nfc_info->Reset();
        nfc_info->card_family = GetCardFamily();
        std::pair<std::string, int> p = GetCardIC();
        nfc_info->IC_type = p.first;
        nfc_info->storage_size = p.second;
    }
    uint8_t identify_tag;
    if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                    PHAC_DISCLOOP_POS_BIT_MASK_A)) {
        if (nfc_info) {
            nfc_info->technology = "A";
            uint8_t UID_size =
                nfc_init->discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0]
                    .bUidSize;
            uint8_t *UID_ptr =
                nfc_init->discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
            uint8_t ATQ_size = PHAC_DISCLOOP_I3P3A_MAX_ATQA_LENGTH;
            uint8_t *ATQ_ptr =
                nfc_init->discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa;
            nfc_info->ATQ = std::vector<uint8_t>(ATQ_ptr, ATQ_ptr + ATQ_size);
            nfc_info->SAK =
                nfc_init->discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
        }
        if (nfc_init->discovery_loop->sTypeATargetInfo.bT1TFlag) {
            if (nfc_info) nfc_info->type = "1";
            top_tag_type = PHAL_TOP_TAG_TYPE_T1T_TAG;
        } else {
            identify_tag =
                (nfc_init->discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0]
                     .aSak &
                 0x60);
            identify_tag = identify_tag >> 5;
            switch (identify_tag) {
                case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK: {
                    if (nfc_info) nfc_info->type = "2";
                    top_tag_type = PHAL_TOP_TAG_TYPE_T2T_TAG;
                } break;
                case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
                    if (nfc_info) nfc_info->type = "4A";
                    top_tag_type = PHAL_TOP_TAG_TYPE_T4T_TAG;
                    break;
                case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
                    if (nfc_info) nfc_info->type = "P2P";
                    break;
                case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
                    if (nfc_info) nfc_info->type = "NFC_DEP & 4A";
                    top_tag_type = PHAL_TOP_TAG_TYPE_T4T_TAG;
                    break;
                default:
                    break;
            }
        }
    }
    if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                    PHAC_DISCLOOP_POS_BIT_MASK_B)) {
        if (nfc_info) {
            nfc_info->technology = "B";
            /* PUPI Length is always 4 bytes */
            uint8_t UID_size = 0x04;
            uint8_t *UID_ptr =
                nfc_init->discovery_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
            uint8_t ATQ_size =
                nfc_init->discovery_loop->sTypeBTargetInfo.aTypeB_I3P3[0]
                    .bAtqBLength;
            uint8_t *ATQ_ptr =
                nfc_init->discovery_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB;
            nfc_info->ATQ = std::vector<uint8_t>(ATQ_ptr, ATQ_ptr + ATQ_size);
        }
    }
    if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                    PHAC_DISCLOOP_POS_BIT_MASK_F212) ||
        PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                    PHAC_DISCLOOP_POS_BIT_MASK_F424)) {
        if (nfc_info) {
            nfc_info->technology = "F";
            uint8_t UID_size = PHAC_DISCLOOP_FELICA_IDM_LENGTH;
            uint8_t *UID_ptr =
                nfc_init->discovery_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
        }
        if ((nfc_init->discovery_loop->sTypeFTargetInfo.aTypeFTag[0]
                 .aIDmPMm[0] == 0x01) &&
            (nfc_init->discovery_loop->sTypeFTargetInfo.aTypeFTag[0]
                 .aIDmPMm[1] == 0xFE)) {
            if (nfc_info) nfc_info->type = "P2P";
        } else {
            if (nfc_info) nfc_info->type = "3";
            top_tag_type = PHAL_TOP_TAG_TYPE_T3T_TAG;
        }
        if (nfc_info) {
            if (nfc_init->discovery_loop->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
                PHAC_DISCLOOP_CON_BITR_212) {
                nfc_info->bit_rate = 424;
            } else {
                nfc_info->bit_rate = 212;
            }
        }
    }
    return top_tag_type;
}
}  // namespace matrix_hal