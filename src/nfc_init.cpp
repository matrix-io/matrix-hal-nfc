
#include "nfc_init.h"

// Begin Variable Setup for NXP HAL
// DO NOT change variable names in this section
extern "C" {
uint8_t bHalBufferTx[DATA_BUFFER_LEN];  // HAL TX buffer
uint8_t bHalBufferRx[DATA_BUFFER_LEN];  // HAL RX buffer
uint8_t sens_res[2] = {0x04, 0x00};  // ATQ bytes - required for anti-collision
uint8_t nfc_id1[3] = {
    0xA1, 0xA2,
    0xA3};  // User defined bytes of the UID - required for anti-collision
uint8_t sel_res = 0x40;
uint8_t nfc_id3 = 0xFA;  // NFC3 byte - required for anti-collision
uint8_t poll_res[18] = {0x01, 0xFE, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xC0,
                        0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0x23, 0x45};
// Stubs, in case NXP HAL expects these implementations
void pWtoxCallBck(uint8_t bTimerId) {}
void pRtoxCallBck(uint8_t bTimerId) {}
uint8_t aAppHCEBuf[32];
uint16_t wAppHCEBuffSize = sizeof(aAppHCEBuf);
// End Variable Setup for NXP HAL
}

namespace matrix_hal {

NFCInit::NFCInit() {
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
        &keystore, sizeof(phKeyStore_Sw_DataParams_t), key_entries,
        sizeof(key_entries) / sizeof(key_entries[0]), key_version_pairs,
        sizeof(key_version_pairs) / sizeof(key_version_pairs[0]), KUC_entries,
        sizeof(KUC_entries) / sizeof(KUC_entries[0]));
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

NFCInit::~NFCInit() { (void)phNfcLib_DeInit(); }

}  // namespace matrix_hal