#ifndef MATRIX_HAL_NFC_ABSTRACTION_H
#define MATRIX_HAL_NFC_ABSTRACTION_H

extern "C" {
#include "./phApp_Init.h"

#define LISTEN_PHASE_TIME_MS 300 /* Listen Phase TIME */

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
}

#endif // MATRIX_HAL_NFC_ABSTRACTION_H