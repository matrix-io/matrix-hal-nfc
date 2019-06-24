// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc_sensor.h"

using std::cerr;
using std::cout;
using std::endl;

// void ReadNdefMessage(phalTop_Sw_DataParams_t *phalTop, uint8_t tagTechnology,
//                      matrix_hal::NFCInfo *nfcInfo) {
// // Get the phalTop component
// phalTop_Sw_DataParams_t *pTop =
//     (phalTop_Sw_DataParams_t *)phNfcLib_GetDataParams(PH_COMP_AL_TOP);
// uint8_t NDEFbuffer[1024];
// phStatus_t status;
// uint8_t bTagState;
// uint16_t NDEFlength = 0;

// /* Configure Top layer for specified tag type */
// status = phalTop_SetConfig(&pTop, PHAL_TOP_CONFIG_TAG_TYPE, topTagType);
// if (status != PH_ERR_SUCCESS) return;

// /* Check for NDEF presence */
// status = phalTop_CheckNdef(&pTop, &bTagState);
// if (status != PH_ERR_SUCCESS) return;

// if ((bTagState == PHAL_TOP_STATE_READONLY) ||
//     (bTagState == PHAL_TOP_STATE_READWRITE)) {
//   /* Read NDEF message */
//   status = phalTop_ReadNdef(&pTop, NDEFbuffer, &NDEFlength);
//   if (status != PH_ERR_SUCCESS) return;

//   if (NDEFlength > 0) {
//     // DEBUG_PRINTF("\tNDEF detected...\n");
//     // DEBUG_PRINTF("\tNDEF length: %d\n", lengthNDEF);
//     // DEBUG_PRINTF("\tNDEF message:\n");
//     // DumpBuffer(aData, lengthNDEF);
//     // DumpBuffer(bufferNDEF, 50);
//     nfcInfo->NDEF = std::unique_ptr<std::vector<uint8_t>>(
//         new std::vector<uint8_t>(NDEFbuffer, NDEFbuffer + NDEFlength));
//   } else {
//     nfcInfo->NDEF =
//         std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
//   }
// }
// }

void ExportTagInfo(phacDiscLoop_Sw_DataParams_t *pDataParams,
                   uint16_t tagTechnology, matrix_hal::NFCInfo *nfcInfo) {
  nfcInfo->reset();
  uint8_t bTagType;
#if defined(NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS) || \
    defined(NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_ACTIVE)
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_A)) {
    nfcInfo->technology = "A";
    uint8_t UIDsize = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
    uint8_t *UIDptr = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
    nfcInfo->UID = std::unique_ptr<std::vector<uint8_t>>(
        new std::vector<uint8_t>(UIDptr, UIDptr + UIDsize));
    uint8_t ATQsize = PHAC_DISCLOOP_I3P3A_MAX_ATQA_LENGTH;
    uint8_t *ATQptr = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa;
    nfcInfo->ATQ = std::unique_ptr<std::vector<uint8_t>>(
        new std::vector<uint8_t>(ATQptr, ATQptr + ATQsize));
    nfcInfo->SAK = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
    if (pDataParams->sTypeATargetInfo.bT1TFlag) {
      nfcInfo->type = "1";
      /* Check for NDEF and Read */
      // ReadNdefMessage(nullptr, PHAL_TOP_TAG_TYPE_T1T_TAG, nfcInfo);
    } else {
      bTagType = (pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
      bTagType = bTagType >> 5;
      switch (bTagType) {
        case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK:
          nfcInfo->type = "2";
          /* Check for NDEF and Read */
          // ReadNdefMessage(nullptr, PHAL_TOP_TAG_TYPE_T2T_TAG, nfcInfo);
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
          nfcInfo->type = "4A";
          /* Check for NDEF and Read */
          // ReadNdefMessage(nullptr, PHAL_TOP_TAG_TYPE_T4T_TAG, nfcInfo);
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
          nfcInfo->type = "P2P";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
          nfcInfo->type = "NFC_DEP & 4A";
          /* Check for NDEF and Read */
          // ReadNdefMessage(nullptr, PHAL_TOP_TAG_TYPE_T4T_TAG, nfcInfo);
          break;
        default:
          break;
      }
    }
  }
#endif
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_B)) {
    nfcInfo->technology = "B";
    /* PUPI Length is always 4 bytes */
    uint8_t UIDsize = 0x04;
    uint8_t *UIDptr = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
    nfcInfo->UID = std::unique_ptr<std::vector<uint8_t>>(
        new std::vector<uint8_t>(UIDptr, UIDptr + UIDsize));
    uint8_t ATQsize = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength;
    uint8_t *ATQptr = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB;
    nfcInfo->ATQ = std::unique_ptr<std::vector<uint8_t>>(
        new std::vector<uint8_t>(ATQptr, ATQptr + ATQsize));
  }
#endif
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F212) ||
      PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F424)) {
    nfcInfo->technology = "F";
    uint8_t UIDsize = PHAC_DISCLOOP_FELICA_IDM_LENGTH;
    uint8_t *UIDptr = pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
    nfcInfo->UID = std::unique_ptr<std::vector<uint8_t>>(
        new std::vector<uint8_t>(UIDptr, UIDptr + UIDsize));
    if ((pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[0] == 0x01) &&
        (pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[1] == 0xFE)) {
      nfcInfo->type = "P2P";
    } else {
      nfcInfo->type = "3";
      /* Check for NDEF and Read */
      // ReadNdefMessage(nullptr, PHAL_TOP_TAG_TYPE_T3T_TAG, nfcInfo);
    }
    if (pDataParams->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
        PHAC_DISCLOOP_CON_BITR_212) {
      nfcInfo->bitRate = 424;
    } else {
      nfcInfo->bitRate = 212;
    }
  }
#endif
}

namespace matrix_hal {
NFCSensor::NFCSensor() {
  (void)phNfcLib_Init();
  if (nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) {
    throw "NFC Sensor Init Failed";
  }
  /* Configuring the activation profile as ISO */
  phNfcLib_SetConfig_Value(PH_NFCLIB_CONFIG_ACTIVATION_PROFILE,
                           PH_NFCLIB_ACTIVATION_PROFILE_ISO);
  if (nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) {
    throw "NFC Sensor Init Failed";
  }
  pDiscLoop = (phacDiscLoop_Sw_DataParams_t *)phNfcLib_GetDataParams(
      PH_COMP_AC_DISCLOOP);
}

NFCSensor::~NFCSensor() { (void)phNfcLib_DeInit(); }

void NFCSensor::Read(NFCInfo *nfcInfo) {
  // Read function takes between 30ms and 40ms to run.
  nfcInfo->recentlyUpdated = false;
  uint16_t tagTechType = 0;
  // Disable blocking so that function will exit if no detection made
  phNfcLib_SetConfig_Value(PH_NFCLIB_CONFIG_ACTIVATION_BLOCKING, PH_OFF);
  nfcLibStatus = phNfcLib_Activate(wTechnologyMask, &PeerInfo, NULL);
  if (nfcLibStatus != PH_NFCLIB_STATUS_PEER_ACTIVATION_DONE) {
    // cerr << "NFC Tag Activation Failed" << endl;
  }
  // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
  phacDiscLoop_GetConfig(pDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED,
                         &tagTechType);
  if (tagTechType != 0) ExportTagInfo(pDiscLoop, tagTechType, nfcInfo);
  nfcLibStatus =
      phNfcLib_Deactivate(PH_NFCLIB_DEACTIVATION_MODE_RELEASE, &PeerInfo);
  if (nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) {
    // cerr << "NFC Tag Deactivate with Release Mode failed, card was removed
    // from vicinity... " << endl; cerr << " Performing Deactivate with RF OFF
    // mode... " << endl;
    nfcLibStatus =
        phNfcLib_Deactivate(PH_NFCLIB_DEACTIVATION_MODE_RF_OFF, &PeerInfo);
  }
}
}  // namespace matrix_hal