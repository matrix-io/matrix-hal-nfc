// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_INIT_H
#define MATRIX_HAL_NFC_INIT_H

#include <string>
#include <vector>

// This MUST be included last!
extern "C" {
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phNfcLib.h>
// TODO: Refine include statements and organize by type
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phCryptoRng.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phCryptoSym.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phalFelica.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phalMfc.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phalTop.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phpalFelica.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/types/ph_Status.h>
}

// Begin NXP Defines
#define DATA_BUFFER_LEN 260
// These two are specifically for the Simplified NXP Lib
#define SIMPLIFIED_ISO_PRIO 0
#define SIMPLIFIED_ISO_STACK 0x20000
// END NXP Defines

namespace matrix_hal {
class NFCInit {
   public:
    // Begin Variable Setup for Simplified NXP Lib
    uint32_t nfc_lib_status = 0;
    uint16_t technology_mask = 0x1B;
    uint8_t more_data_available = 0;
    phNfcLib_Transmit_t nfc_lib_transmit;
    phNfcLib_PeerInfo_t peer_info = {0};
    uint8_t data_buffer[DATA_BUFFER_LEN];  // universal data buffer
    // End Variable Setup for Simplified NXP Lib
    // NXP Simplified Lib Component pointers (Managed by Simplified NXP Lib)
    phbalReg_Type_t* bal;              // Bus Abstraction Layer component
    phhalHw_Nfc_Ic_DataParams_t* hal;  // Hardware Abstraction Layer component
    phpalI14443p3a_Sw_DataParams_t*
        pal_iso14443p3a;  // Protocol Abstraction Layer ISO I14443-A component
    phpalI14443p3b_Sw_DataParams_t*
        pal_iso14443p3b;  // Protocol Abstraction Layer ISO I14443-B component
    phpalI14443p4a_Sw_DataParams_t*
        pal_iso14443p4a;  // Protocol Abstraction Layer ISO I14443-4A component
    phpalI14443p4_Sw_DataParams_t*
        pal_iso14443p4;  // Protocol Abstraction Layer ISO I14443-4 component
    phacDiscLoop_Sw_DataParams_t*
        discovery_loop;  // Application Layer Discovery Loop component
    // Keystore and Crypto components
    // uint16_t key_entires_amount = 10;
    // uint16_t key_version_pairs_amount = 10;
    // uint16_t KUC_entries_amount = 10;
    // phKeyStore_Sw_DataParams_t keystore;
    // std::vector<phKeyStore_Sw_KeyEntry_t> key_entries =
    //     std::vector<phKeyStore_Sw_KeyEntry_t>(key_entires_amount);
    // std::vector<phKeyStore_Sw_KeyVersionPair_t> key_version_pairs =
    //     std::vector<phKeyStore_Sw_KeyVersionPair_t>(key_version_pairs_amount);
    // std::vector<phKeyStore_Sw_KUCEntry_t> KUC_entries =
    //     std::vector<phKeyStore_Sw_KUCEntry_t>(KUC_entries_amount);
    // phCryptoSym_Sw_DataParams_t crypto_sym;
    // phCryptoRng_Sw_DataParams_t crypto_rng;
    // NFC Standard Lib Components (Adding additional functionality)
    phpalMifare_Sw_DataParams_t
        pal_mifare;  // Protocol Abstraction Layer Mifare component
    phpalFelica_Sw_DataParams_t
        pal_felica;  // Protocol Abstraction Layer Felica component
    phalMful_Sw_DataParams_t
        al_mful;  // Application Layer Mifare Ultralight component
    phalMfdf_Sw_DataParams_t
        al_mfdf;  // Application Layer Mifare DESFire component
    phalMfc_Sw_DataParams_t
        al_mfc;  // Application Layer Mifare Classic component
    phalFelica_Sw_DataParams_t al_felica;  // Application Layer Felica component
    phalT1T_Sw_DataParams_t al_t1t;        // Application Layer Mifare component
    phalTop_Sw_DataParams_t tag_operations;  // Application Layer Tag Operations
                                             // component Begin User Functions
    NFCInit();
    ~NFCInit();
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_INIT_H