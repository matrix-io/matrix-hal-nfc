// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc_sensor.h"

using std::cerr;
using std::cout;
using std::endl;

namespace matrix_hal {
NFCSensor::NFCSensor() {
    std::string init_err_msg = "NFC Sensor Init Failed";
    (void)phNfcLib_Init();
    if (nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        throw init_err_msg;
    }
    /* Configuring the activation profile as ISO */
    phNfcLib_SetConfig_Value(PH_NFCLIB_CONFIG_ACTIVATION_PROFILE,
                             PH_NFCLIB_ACTIVATION_PROFILE_ISO);
    if (nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        throw init_err_msg;
    }
    // Get pointer to all components initialized by Simplified NXP Lib
    bal = (phbalReg_Type_t *)phNfcLib_GetDataParams(PH_COMP_BAL);
    hal = (phhalHw_Nfc_Ic_DataParams_t *)phNfcLib_GetDataParams(PH_COMP_HAL);
    pal_iso14443p3a = (phpalI14443p3a_Sw_DataParams_t *)phNfcLib_GetDataParams(
        PH_COMP_PAL_ISO14443P3A);
    pal_iso14443p3b = (phpalI14443p3b_Sw_DataParams_t *)phNfcLib_GetDataParams(
        PH_COMP_PAL_ISO14443P3B);
    pal_iso14443p4a = (phpalI14443p4a_Sw_DataParams_t *)phNfcLib_GetDataParams(
        PH_COMP_PAL_ISO14443P4A);
    pal_iso14443p4 = (phpalI14443p4_Sw_DataParams_t *)phNfcLib_GetDataParams(
        PH_COMP_PAL_ISO14443P4);
    discovery_loop = (phacDiscLoop_Sw_DataParams_t *)phNfcLib_GetDataParams(
        PH_COMP_AC_DISCLOOP);
    // Initialize Keystore and Crypto components
    nfc_lib_status = phKeyStore_Sw_Init(
        &keystore, sizeof(phKeyStore_Sw_DataParams_t), key_entries.data(),
        key_entires_amount, key_version_pairs.data(), key_version_pairs_amount,
        KUC_entries.data(), KUC_entries_amount);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status = phCryptoSym_Sw_Init(
        &crypto_sym, sizeof(phCryptoSym_Sw_DataParams_t), &keystore);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status = phCryptoRng_Sw_Init(
        &crypto_rng, sizeof(phCryptoRng_Sw_DataParams_t), &crypto_sym);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    // Initialize additional components
    nfc_lib_status =
        phpalMifare_Sw_Init(&pal_mifare, sizeof(phpalMifare_Sw_DataParams_t),
                            &hal->sHal, pal_iso14443p4);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status = phpalFelica_Sw_Init(
        &pal_felica, sizeof(phpalFelica_Sw_DataParams_t), &hal->sHal);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status =
        phalMful_Sw_Init(&al_mful, sizeof(phalMful_Sw_DataParams_t),
                         &pal_mifare, &keystore, &crypto_sym, &crypto_rng);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status = phalMfdf_Sw_Init(
        &al_mfdf, sizeof(phalMfdf_Sw_DataParams_t), &pal_mifare, &keystore,
        &crypto_sym, &crypto_rng, &hal->sHal);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status = phalMfc_Sw_Init(&al_mfc, sizeof(phalMfc_Sw_DataParams_t),
                                     &pal_mifare, &keystore);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status = phalFelica_Sw_Init(
        &al_felica, sizeof(phalFelica_Sw_DataParams_t), &pal_felica);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status = phalT1T_Sw_Init(&al_t1t, sizeof(phalT1T_Sw_DataParams_t),
                                     pal_iso14443p3a);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    nfc_lib_status =
        phalTop_Sw_Init(&tag_operations, sizeof(phalTop_Sw_DataParams_t),
                        &al_t1t, &al_mful, &al_felica, &al_mfdf, NULL);
    if (nfc_lib_status != PH_ERR_SUCCESS) {
        throw init_err_msg;
    }
    // Pass additional components into discovery loop
    discovery_loop->pHalDataParams = &hal->sHal;
    discovery_loop->pPal1443p3aDataParams = pal_iso14443p3a;
    discovery_loop->pPal1443p3bDataParams = pal_iso14443p3b;
    discovery_loop->pPal1443p4aDataParams = pal_iso14443p4a;
    discovery_loop->pPal14443p4DataParams = pal_iso14443p4;
    discovery_loop->pPalFelicaDataParams = &pal_felica;
    discovery_loop->pAlT1TDataParams = &al_t1t;
    /* Disable blocking so that activate function will return if no card in
     * sensor field */
    phNfcLib_SetConfig_Value(PH_NFCLIB_CONFIG_ACTIVATION_BLOCKING, PH_OFF);
}

NFCSensor::~NFCSensor() { (void)phNfcLib_DeInit(); }

int NFCSensor::Activate() {
    nfc_lib_status = phNfcLib_Activate(technology_mask, &peer_info, NULL);
    return nfc_lib_status;
}

int NFCSensor::Deactivate() {
    nfc_lib_status =
        phNfcLib_Deactivate(PH_NFCLIB_DEACTIVATION_MODE_RELEASE, &peer_info);
    if (nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        /* NFC Tag Deactivate with Release Mode failed, card was removed
        from vicinity...
        Performing Deactivate with RF OFF mode */
        phNfcLib_Deactivate(PH_NFCLIB_DEACTIVATION_MODE_RF_OFF, &peer_info);
    }
    return nfc_lib_status;
}  // namespace matrix_hal

/* This will read and populate info data (Card must already have been
 * activated!)
 */
int NFCSensor::ReadInfo(NFCInfo *nfc_info) {
    nfc_info->recently_updated = false;
    uint16_t tag_tech_type = 0;
    // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
    nfc_lib_status = phacDiscLoop_GetConfig(
        discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tag_tech_type);
    if (nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_lib_status;
    ExportTag(tag_tech_type, nfc_info);
    return nfc_lib_status;
}

/* This will activate, read and populate info data, then deactivate card */
int NFCSensor::SimpleReadInfo(NFCInfo *nfc_info) {
    nfc_info->recently_updated = false;
    nfc_lib_status = Activate();
    if (nfc_lib_status != PH_NFCLIB_STATUS_PEER_ACTIVATION_DONE)
        return nfc_lib_status;
    nfc_lib_status = ReadInfo(nfc_info);
    // If already errored at ReadInfo then return the error from ReadInfo
    if (nfc_lib_status < 0)
        Deactivate();
    else
        nfc_lib_status = Deactivate();
    return nfc_lib_status;
}

int NFCSensor::ReadNDEF(NFC_NDEF *nfc_ndef) {
    nfc_ndef->recently_updated = false;
    uint16_t tag_tech_type = 0;
    uint8_t top_tag_type = 0;
    uint8_t top_tag_state = 0;
    uint16_t ndef_length = 0;
    nfc_lib_status = phacDiscLoop_GetConfig(
        discovery_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tag_tech_type);
    if (nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_lib_status;
    top_tag_type = ExportTag(tag_tech_type, nullptr);
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
                discovery_loop->pPal1443p4aDataParams,
            &CID_enabled, &CID, &NAD_supported, &FWI, &FSDI, &FSCI);
        phpalI14443p4_Sw_SetProtocol((phpalI14443p4_Sw_DataParams_t *)
                                         discovery_loop->pPal14443p4DataParams,
                                     CID_enabled, CID, NAD_supported, NAD, FWI,
                                     FSDI, FSCI);
    }
    nfc_ndef->Reset();
    /* Configure Top layer for specified tag type */
    nfc_lib_status = phalTop_SetConfig(&tag_operations,
                                       PHAL_TOP_CONFIG_TAG_TYPE, top_tag_type);
    /* Check for NDEF presence */
    nfc_lib_status = phalTop_CheckNdef(&tag_operations, &top_tag_state);
    if ((top_tag_state == PHAL_TOP_STATE_READONLY) ||
        (top_tag_state == PHAL_TOP_STATE_READWRITE)) {
        nfc_ndef->ndef_detected = true;
        // Read NDEF message
        nfc_lib_status =
            phalTop_ReadNdef(&tag_operations, data_buffer, &ndef_length);
        nfc_ndef->read_ndef =
            std::vector<uint8_t>(data_buffer, data_buffer + ndef_length);
    }
    // TODO: Fix error handling
    return -nfc_lib_status;
}

/* This will read a single specified page (Card must already have been
 * activated!) If card is not activated or read fails the vector will be empty.
 * page_number can be 0x00 - 0xFF depending upon card layout
 */
std::vector<uint8_t> NFCSensor::MFUL::ReadPage(uint8_t page_number) {
    if (nfc_sensor->peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
        return std::vector<uint8_t>();
    nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber =
        page_number;
    nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand = MFUL_Read;
    nfc_sensor->nfc_lib_status =
        phNfcLib_Transmit(&nfc_sensor->nfc_lib_transmit, 0x0);

    if (nfc_sensor->nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        // Read for Block failed...
        return std::vector<uint8_t>();
    }
    /* This parameter has to be reset before every receive */
    uint16_t bytes_read = 256;
    /* To perform receive operation to get back the read data */
    nfc_sensor->nfc_lib_status =
        phNfcLib_Receive(&nfc_sensor->data_buffer[0], &bytes_read,
                         &nfc_sensor->more_data_available);

    /* The status should be success and the number of bytes received should be
     * 16 for MFUL/NTAG cards */
    if ((nfc_sensor->nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) ||
        (bytes_read != 16)) {
        // cerr << "Read for Block " << +page_number << " failed..." << endl;
        return std::vector<uint8_t>();
    }
    return std::vector<uint8_t>(nfc_sensor->data_buffer,
                                nfc_sensor->data_buffer + 4);
}

/* page_number can be 0x00 - 0xFF depending upon card layout */
int NFCSensor::MFUL::WritePage(uint8_t page_number,
                               std::vector<uint8_t> &write_data) {
    // Check if card activated and is a MFUL or NTAG
    if (nfc_sensor->peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
        return INCORRECT_CARD_FOR_FUNCTION;
    nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber =
        page_number;
    nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.pBuffer =
        write_data.data();
    nfc_sensor->nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand =
        MFUL_Write;
    /* Transmit will execute the command */
    nfc_sensor->nfc_lib_status = phNfcLib_Transmit(
        &nfc_sensor->nfc_lib_transmit, 0x04 /* For MFUL/NTAG the size of a page
                                         to be written is 4 bytes */
    );
    return nfc_sensor->nfc_lib_status;
}

int NFCSensor::MFUL::ReadData(NFCData *nfc_data) {
    nfc_data->recently_updated = false;
    // Check if card activated and is a MFUL or NTAG
    if (nfc_sensor->peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
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

/* *********************** PRIVATE FUNCTIONS ************************ */

/* https://stackoverflow.com/questions/37002498/distinguish-ntag213-from-mf0icu2
 */
std::string NFCSensor::GetCardIC(NFCInfo *nfc_info) {
    if (peer_info.dwActivatedType == E_PH_NFCLIB_MIFARE_ULTRALIGHT) {
        // TODO: look into this
        // TODO: This is for testing only, remove
        std::vector<uint8_t> version_info = std::vector<uint8_t>(8);
        nfc_lib_status = phalMful_GetVersion(&al_mful, version_info.data());
        if (nfc_lib_status != PH_ERR_SUCCESS) {
            // First generation tag
        }
        cout << "MFUL Data" << endl;
        cout << NFCData::StrHexByteVec(version_info) << endl;
    }
}

/* This function returns top_tag_type for use with tag_operations, it will also
 * populate nfc_info (if provided) */
uint8_t NFCSensor::ExportTag(uint16_t tag_tech_type, NFCInfo *nfc_info) {
    uint8_t top_tag_type = 0;
    if (nfc_info) {
        nfc_info->Reset();
        // TODO: Clean up DescCardType function
        nfc_info->card_type = DescCardType(peer_info.dwActivatedType);
    }
    uint8_t identify_tag;
    if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                    PHAC_DISCLOOP_POS_BIT_MASK_A)) {
        if (nfc_info) {
            nfc_info->technology = "A";
            uint8_t UID_size =
                discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
            uint8_t *UID_ptr =
                discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
            uint8_t ATQ_size = PHAC_DISCLOOP_I3P3A_MAX_ATQA_LENGTH;
            uint8_t *ATQ_ptr =
                discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa;
            nfc_info->ATQ = std::vector<uint8_t>(ATQ_ptr, ATQ_ptr + ATQ_size);
            nfc_info->SAK =
                discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
        }
        if (discovery_loop->sTypeATargetInfo.bT1TFlag) {
            if (nfc_info) nfc_info->type = "1";
            top_tag_type = PHAL_TOP_TAG_TYPE_T1T_TAG;
        } else {
            identify_tag =
                (discovery_loop->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
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
                discovery_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
            uint8_t ATQ_size =
                discovery_loop->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength;
            uint8_t *ATQ_ptr =
                discovery_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB;
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
                discovery_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
        }
        if ((discovery_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[0] ==
             0x01) &&
            (discovery_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[1] ==
             0xFE)) {
            if (nfc_info) nfc_info->type = "P2P";
        } else {
            if (nfc_info) nfc_info->type = "3";
            top_tag_type = PHAL_TOP_TAG_TYPE_T3T_TAG;
        }
        if (nfc_info) {
            if (discovery_loop->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
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