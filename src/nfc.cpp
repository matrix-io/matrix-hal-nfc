// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc.h"

using std::cerr;
using std::cout;
using std::endl;

namespace matrix_hal {

int NFC::Activate() {
    nfc_init.nfc_lib_status =
        phNfcLib_Activate(nfc_init.technology_mask, &nfc_init.peer_info, NULL);
    return nfc_init.nfc_lib_status;
}

int NFC::Deactivate() {
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
int NFC::ReadInfo(InfoContent *nfc_info) {
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

}  // namespace matrix_hal