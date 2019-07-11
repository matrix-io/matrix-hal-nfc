// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc_sensor.h"

using std::cerr;
using std::cout;
using std::endl;

namespace matrix_hal {
NFCSensor::NFCSensor() {
    (void)phNfcLib_Init();
    if (nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        throw "NFC Sensor Init Failed";
    }
    /* Configuring the activation profile as ISO */
    phNfcLib_SetConfig_Value(PH_NFCLIB_CONFIG_ACTIVATION_PROFILE,
                             PH_NFCLIB_ACTIVATION_PROFILE_ISO);
    if (nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
        throw "NFC Sensor Init Failed";
    }
    // Get pointer to all components initialized by Simplified Lib
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
    disc_loop = (phacDiscLoop_Sw_DataParams_t *)phNfcLib_GetDataParams(
        PH_COMP_AC_DISCLOOP);
    // Initialize additional components
    nfc_lib_status =
        phpalMifare_Sw_Init(&pal_Mifare, sizeof(phpalMifare_Sw_DataParams_t),
                            &hal->sHal, pal_iso14443p4);
    nfc_lib_status = phpalFelica_Sw_Init(
        &pal_Felica, sizeof(phpalFelica_Sw_DataParams_t), &hal->sHal);
    nfc_lib_status =
        phalMful_Sw_Init(&al_MFUL, sizeof(phalMful_Sw_DataParams_t),
                         &pal_Mifare, NULL, NULL, NULL);
    nfc_lib_status =
        phalMfdf_Sw_Init(&al_MFDF, sizeof(phalMfdf_Sw_DataParams_t),
                         &pal_Mifare, NULL, NULL, NULL, &hal->sHal);
    nfc_lib_status = phalMfc_Sw_Init(&al_MFC, sizeof(phalMfc_Sw_DataParams_t),
                                     &pal_Mifare, NULL);
    nfc_lib_status = phalFelica_Sw_Init(
        &al_Felica, sizeof(phalFelica_Sw_DataParams_t), &pal_Felica);
    nfc_lib_status = phalT1T_Sw_Init(&al_T1T, sizeof(phalT1T_Sw_DataParams_t),
                                     pal_iso14443p3a);
    nfc_lib_status =
        phalTop_Sw_Init(&tag_operation, sizeof(phalTop_Sw_DataParams_t),
                        &al_T1T, &al_MFUL, &al_Felica, &al_MFDF, NULL);
    // Pass additional components into discovery loop
    disc_loop->pHalDataParams = &hal->sHal;
    disc_loop->pPal1443p3aDataParams = pal_iso14443p3a;
    disc_loop->pPal1443p3bDataParams = pal_iso14443p3b;
    disc_loop->pPal1443p4aDataParams = pal_iso14443p4a;
    disc_loop->pPal14443p4DataParams = pal_iso14443p4;
    disc_loop->pPalFelicaDataParams = &pal_Felica;
    disc_loop->pAlT1TDataParams = &al_T1T;
    /* Disable blocking so that activate function will return upon every
    iteration of discovery loop */
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
}

/* This will read and populate info data (Card must already have been
 * activated!)
 */
int NFCSensor::ReadInfo(NFCInfo *nfc_info) {
    nfc_info->recently_updated = false;
    uint16_t tag_tech_type = 0;
    // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
    nfc_lib_status = phacDiscLoop_GetConfig(
        disc_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tag_tech_type);
    if (nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_lib_status;
    ExportTag(tag_tech_type, nfc_info, nullptr);
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
    nfc_lib_status = phacDiscLoop_GetConfig(
        disc_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tag_tech_type);
    if (nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
        return -nfc_lib_status;
    nfc_lib_status = ExportTag(tag_tech_type, nullptr, nfc_ndef);
    return nfc_lib_status;
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

// TODO : Make ReadNdef neater
int NFCSensor::ReadNdefMessage(uint8_t tag_tech_type, NFC_NDEF *nfc_ndef) {
    phStatus_t status;
    uint8_t bTagState;
    uint16_t wDataLength = 0;

    /* Configure Top layer for specified tag type */
    status = phalTop_SetConfig(&tag_operation, PHAL_TOP_CONFIG_TAG_TYPE,
                               tag_tech_type);
    // DEBUG_ERROR_PRINT(status);

    /* Check for NDEF presence */
    status = phalTop_CheckNdef(&tag_operation, &bTagState);
    // DEBUG_ERROR_PRINT(status);
    if ((bTagState == PHAL_TOP_STATE_READONLY) ||
        (bTagState == PHAL_TOP_STATE_READWRITE)) {
        /* Read NDEF message */
        status = phalTop_ReadNdef(&tag_operation, data_buffer, &wDataLength);
        // DEBUG_ERROR_PRINT(status);
        /* Print NDEF message, if not NULL NDEF */
        if (wDataLength) {
            //     printf("\tNDEF detected...\n");
            //     printf("\tNDEF length: %d\n", wDataLength);
            //     printf("\tNDEF message:\n");
            nfc_ndef->read_ndef =
                std::vector<uint8_t>(data_buffer, data_buffer + wDataLength);
        } else {
            // printf("\tNDEF content is NULL...\n");
        }
    } else {
        // printf("\tNo NDEF content detected...\n");
    }
    return status;
}
// TODO: Make NDEF Read work with all tags
int NFCSensor::ExportTag(uint16_t tag_tech_type, NFCInfo *nfc_info,
                         NFC_NDEF *nfc_ndef) {
    if (nfc_info) {
        nfc_info->Reset();
        nfc_info->card_type = DescCardType(peer_info.dwActivatedType);
    }
    uint8_t tag_type;
    if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                    PHAC_DISCLOOP_POS_BIT_MASK_A)) {
        if (nfc_info) {
            nfc_info->technology = "A";
            uint8_t UID_size =
                disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
            uint8_t *UID_ptr = disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
            uint8_t ATQ_size = PHAC_DISCLOOP_I3P3A_MAX_ATQA_LENGTH;
            uint8_t *ATQ_ptr = disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa;
            nfc_info->ATQ = std::vector<uint8_t>(ATQ_ptr, ATQ_ptr + ATQ_size);
            nfc_info->SAK = disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
        }
        if (disc_loop->sTypeATargetInfo.bT1TFlag) {
            if (nfc_info) nfc_info->type = "1";
            if (nfc_ndef)
                nfc_lib_status =
                    ReadNdefMessage(PHAL_TOP_TAG_TYPE_T1T_TAG, nfc_ndef);
        } else {
            tag_type = (disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
            tag_type = tag_type >> 5;
            switch (tag_type) {
                case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK: {
                    if (nfc_info) nfc_info->type = "2";
                    if (nfc_ndef)
                        nfc_lib_status = ReadNdefMessage(
                            PHAL_TOP_TAG_TYPE_T2T_TAG, nfc_ndef);
                    // TODO: look into this
                    // TODO: This is for testing only, remove
                    // uint8_t tmp[8];
                    // phalMful_GetVersion(&al_MFUL, tmp);
                    // cout << "MFUL Data" << endl;
                    // std::vector<uint8_t> vec(tmp, tmp + 8);
                    // cout << NFCData::StrHexByteVec(vec) << endl;
                } break;
                case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
                    if (nfc_info) nfc_info->type = "4A";
                    if (nfc_ndef) {
                        // phpalI14443p4a_Sw_GetProtocolParams(
                        //     (phpalI14443p4a_Sw_DataParams_t *)
                        //         disc_loop->pPal1443p4aDataParams,
                        //     &bCidEnabled, &bCid, &bNadSuported, &bFwi,
                        //     &bFsdi, &bFsci);
                        // phpalI14443p4_Sw_SetProtocol(
                        //     (phpalI14443p4_Sw_DataParams_t *)
                        //         disc_loop->pPal14443p4DataParams,
                        //     bCidEnabled, bCid, bNadSuported, bNad, bFwi,
                        //     bFsdi, bFsci);
                        nfc_lib_status = ReadNdefMessage(
                            PHAL_TOP_TAG_TYPE_T4T_TAG, nfc_ndef);
                    }
                    break;
                case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
                    if (nfc_info) nfc_info->type = "P2P";
                    break;
                case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
                    if (nfc_info) nfc_info->type = "NFC_DEP & 4A";
                    if (nfc_ndef) {
                        nfc_lib_status = ReadNdefMessage(
                            PHAL_TOP_TAG_TYPE_T4T_TAG, nfc_ndef);
                    }
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
            uint8_t *UID_ptr = disc_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
            uint8_t ATQ_size =
                disc_loop->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength;
            uint8_t *ATQ_ptr = disc_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB;
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
            uint8_t *UID_ptr = disc_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
            nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
        }
        if ((disc_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[0] == 0x01) &&
            (disc_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[1] == 0xFE)) {
            if (nfc_info) nfc_info->type = "P2P";
        } else {
            if (nfc_info) nfc_info->type = "3";
            nfc_lib_status =
                ReadNdefMessage(PHAL_TOP_TAG_TYPE_T3T_TAG, nfc_ndef);
        }
        if (nfc_info) {
            if (disc_loop->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
                PHAC_DISCLOOP_CON_BITR_212) {
                nfc_info->bit_rate = 424;
            } else {
                nfc_info->bit_rate = 212;
            }
        }
    }
    // Return negative status since function is part of NXP standard library.
    if (nfc_ndef)
        return -nfc_lib_status;
    else
        return PH_NFCLIB_STATUS_SUCCESS;
}
}  // namespace matrix_hal