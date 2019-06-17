#ifndef MATRIX_HAL_NFC_ABSTRACTION_H
#define MATRIX_HAL_NFC_ABSTRACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "matrix_nfc/nxp_nfc/NfcrdlibEx2_AdvancedDiscoveryLoop/intfs/phApp_Init.h"

#define LISTEN_PHASE_TIME_MS 300 /* Listen Phase TIME */

/*NXP HAL variables*/
uint8_t bHalBufferTx[256]; /* HAL TX buffer. Size 256 - Based on maximum FSL */
uint8_t bHalBufferRx[256]; /* HAL RX buffer. Size 256 - Based on maximum FSL */

uint8_t sens_res[2] = {0x04, 0x00}; /* ATQ bytes - needed for anti-collision */
uint8_t nfc_id1[3] = {0xA1, 0xA2,
                      0xA3}; /* user defined bytes of the UID (one is hardcoded)
                                - needed for anti-collision */
uint8_t sel_res = 0x40;
uint8_t nfc_id3 = 0xFA; /* NFC3 byte - required for anti-collision */
uint8_t poll_res[18] = {0x01, 0xFE, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xC0,
                        0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0x23, 0x45};

/* Stubs, in case the phApp_Init.c expects these implementations */
#ifdef NXPBUILD__PHPAL_I14443P4MC_SW
/*
 * WTX Callback called from WTX timer of 14443p3mC PAL.
 */
void pWtoxCallBck(uint8_t bTimerId) { /* Dummy */
}

uint8_t aAppHCEBuf[32];
uint16_t wAppHCEBuffSize = sizeof(aAppHCEBuf);
#endif /* NXPBUILD__PHPAL_I14443P4MC_SW */

#ifdef NXPBUILD__PHPAL_I18092MT_SW
void pRtoxCallBck(uint8_t bTimerId) { /* Dummy */
}
#endif /* NXPBUILD__PHPAL_I18092MT_SW */
#ifdef __cplusplus
}
#endif
#endif // MATRIX_HAL_NFC_ABSTRACTION_H