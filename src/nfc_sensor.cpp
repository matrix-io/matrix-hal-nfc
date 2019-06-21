// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc_sensor.h"

using std::cerr;
using std::cout;
using std::endl;

void ExportTagInfo(phacDiscLoop_Sw_DataParams_t *pDataParams,
                   uint16_t tagTechnology, matrix_hal::NFCData *nfcData) {
  nfcData->reset();
  uint8_t bTagType;
#if defined(NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS) || \
    defined(NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_ACTIVE)
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_A)) {
    nfcData->technology = "Type A";
    uint8_t UIDsize = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
    uint8_t *UIDptr = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    uint8_t ATQsize = PHAC_DISCLOOP_I3P3A_MAX_ATQA_LENGTH;
    uint8_t *ATQptr = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa;
    nfcData->ATQ = std::vector<int>(ATQptr, ATQptr + ATQsize);
    nfcData->SAK = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
    if (pDataParams->sTypeATargetInfo.bT1TFlag)
      nfcData->type = "Type 1 Tag";
    else {
      bTagType = (pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
      bTagType = bTagType >> 5;
      switch (bTagType) {
        case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK:
          nfcData->type = "Type 2 Tag";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
          nfcData->type = "Type 4A Tag";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
          nfcData->type = "Type P2P";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
          nfcData->type = "Type NFC_DEP and 4A Tag";
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
    nfcData->technology = "Type B";
    /* PUPI Length is always 4 bytes */
    uint8_t UIDsize = 0x04;
    uint8_t *UIDptr = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    uint8_t ATQsize = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength;
    uint8_t *ATQptr = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB;
    nfcData->ATQ = std::vector<int>(ATQptr, ATQptr + ATQsize);
  }
#endif
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F212) ||
      PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F424)) {
    nfcData->technology = "Type F";
    uint8_t UIDsize = PHAC_DISCLOOP_FELICA_IDM_LENGTH;
    uint8_t *UIDptr = pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    if ((pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[0] == 0x01) &&
        (pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[1] == 0xFE)) {
      nfcData->type = "P2P";
    } else {
      nfcData->type = "Type 3 Tag";
    }
    if (pDataParams->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
        PHAC_DISCLOOP_CON_BITR_212) {
      nfcData->bitRate = 424;
    } else {
      nfcData->bitRate = 212;
    }
  }
#endif
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_V)) {
    nfcData->technology = "Type V / ISO 15693 / T5T";
    uint8_t UIDsize = 0x08;
    uint8_t *UIDptr = pDataParams->sTypeVTargetInfo.aTypeV[0].aUid;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
  }
#endif
#ifdef NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_18000P3M3)) {
    nfcData->technology = "ISO 18000p3m3 / EPC Gen2";
    uint8_t UIDsize =
        pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[0].wUiiLength / 8;
    uint8_t *UIDptr = pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[0].aUii;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
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
}

NFCSensor::~NFCSensor() { (void)phNfcLib_DeInit(); }

void NFCSensor::Read(NFCData *nfcData) {
  // Read function takes between 30ms and 40ms to run.
  nfcData->recentlyUpdated = false;
  phStatus_t discLoopStatus = PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED;
  uint16_t tagTechType = 0;
  // Disable blocking so that function will exit if no detection made
  phNfcLib_SetConfig_Value(PH_NFCLIB_CONFIG_ACTIVATION_BLOCKING, PH_OFF);
  nfcLibStatus = phNfcLib_Activate(wTechnologyMask, &PeerInfo, NULL);
  if (nfcLibStatus != PH_NFCLIB_STATUS_PEER_ACTIVATION_DONE) {
    // cerr << "NFC Tag Activation Failed" << endl;
  }
  // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
  phacDiscLoop_Sw_DataParams_t *pDiscLoop =
      (phacDiscLoop_Sw_DataParams_t *)phNfcLib_GetDataParams(
          PH_COMP_AC_DISCLOOP);
  discLoopStatus = phacDiscLoop_GetConfig(
      pDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
  if (tagTechType != 0) ExportTagInfo(pDiscLoop, tagTechType, nfcData);
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