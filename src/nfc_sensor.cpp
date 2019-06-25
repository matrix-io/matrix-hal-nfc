// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc_sensor.h"

using std::cerr;
using std::cout;
using std::endl;

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
  pDiscLoop = (phacDiscLoop_Sw_DataParams_t*)phNfcLib_GetDataParams(
      PH_COMP_AC_DISCLOOP);
  /* Disable blocking so that activate function will return upon every iteration
  of discovery loop */
  phNfcLib_SetConfig_Value(PH_NFCLIB_CONFIG_ACTIVATION_BLOCKING, PH_OFF);
}

NFCSensor::~NFCSensor() { (void)phNfcLib_DeInit(); }

/* This will activate, read and populate info data, then deactivate card */
bool NFCSensor::SimpleReadInfo(NFCInfo* nfcInfo) {
  bool success = true;
  nfcInfo->recentlyUpdated = false;
  uint16_t tagTechType = 0;
  success = success && Activate();
  // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
  nfcLibStatus = phacDiscLoop_GetConfig(
      pDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
  if (nfcLibStatus != PH_ERR_SUCCESS || tagTechType == 0) return false;
  ExportTagInfo(pDiscLoop, tagTechType, nfcInfo);
  success = success && Deactivate();
  return success;
}

bool NFCSensor::Activate() {
  nfcLibStatus = phNfcLib_Activate(wTechnologyMask, &PeerInfo, NULL);
  if (nfcLibStatus != PH_NFCLIB_STATUS_PEER_ACTIVATION_DONE) {
    // cerr << "NFC Tag Activation Failed" << endl;
    return false;
  }
  return true;
}

bool NFCSensor::Deactivate() {
  nfcLibStatus =
      phNfcLib_Deactivate(PH_NFCLIB_DEACTIVATION_MODE_RELEASE, &PeerInfo);
  if (nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) {
    /* cerr << "NFC Tag Deactivate with Release Mode failed, card was removed
    from vicinity... " << endl; cerr << " Performing Deactivate with RF OFF
    mode... " << endl; */
    nfcLibStatus =
        phNfcLib_Deactivate(PH_NFCLIB_DEACTIVATION_MODE_RF_OFF, &PeerInfo);
    if (nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) return false;
  }
  return true;
}

/* This will read and populate info data (Card must already have been
 * activated!)
 */
bool NFCSensor::ReadInfo(NFCInfo* nfcInfo) {
  nfcInfo->recentlyUpdated = false;
  uint16_t tagTechType = 0;
  // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
  nfcLibStatus = phacDiscLoop_GetConfig(
      pDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
  if (nfcLibStatus != PH_ERR_SUCCESS || tagTechType == 0) return false;
  ExportTagInfo(pDiscLoop, tagTechType, nfcInfo);
  return true;
}

/* This will read a single specified page (Card must already have been
 * activated!) If card is not activated or read fails the vector will be empty.
 * pageNumber can be 0x00 - 0xFF depending upon card layout
 */
std::vector<uint8_t> NFCSensor::ReadPage(uint8_t pageNumber) {
  std::vector<uint8_t> readBuffer;
  switch (PeerInfo.dwActivatedType) {
      /* Reference application for the particular tech type will divert form
       * here */
    case E_PH_NFCLIB_MIFARE_CLASSIC_1K:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_MIFARE_CLASSIC_4K:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_MIFARE_ULTRALIGHT:
      readBuffer = ReadPage_MFUL_NTAG(pageNumber);
      break;

    case E_PH_NFCLIB_MIFARE_DESFIRE:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEB_LAYER3:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEA_LAYER3:
      /* DEBUG_PRINTF(
          " \nType A Layer 3 card or Mifare Classic/ultralight with merged "
          "atqa detected... \n"); */
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEA_LAYER4:
      /*       DEBUG_PRINTF(
                " \nType A Layer 4 card or Mifare desfire with merged atqa "
                "detected... \n");
            nfcLibStatus = NfcLib_Layer4TypeA_Reference_app(); */
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEB_LAYER4:
      /*       DEBUG_PRINTF(" \nType B Layer 4 card detected... \n");
            nfcLibStatus = NfcLib_TypeB_Reference_app(); */
      // TODO: Implement
      break;

    default:
      break;
  }
  return readBuffer;
}

void NFCSensor::ReadData(NFCData* nfcData) {
  nfcData->recentlyUpdated = false;
  // Check if card activated
  if (PeerInfo.dwActivatedType == 0) return;
  nfcData->reset();
  uint8_t pageNumber = 0;
  std::vector<uint8_t> readBuffer;

  do {
    if (!readBuffer.empty()) {
      nfcData->readData.emplace_back(readBuffer);
      readBuffer.clear();
    }
    readBuffer = ReadPage(pageNumber);
    pageNumber++;
  } while (!readBuffer.empty());
}

/* pageNumber can be 0x00 - 0xFF depending upon card layout */
bool NFCSensor::WritePage(uint8_t pageNumber, std::vector<uint8_t> writeData) {
  bool success = false;
  switch (PeerInfo.dwActivatedType) {
      /* Reference application for the particular tech type will divert form
       * here */
    case E_PH_NFCLIB_MIFARE_CLASSIC_1K:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_MIFARE_CLASSIC_4K:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_MIFARE_ULTRALIGHT:
      success = WritePage_MFUL_NTAG(pageNumber, writeData);
      break;

    case E_PH_NFCLIB_MIFARE_DESFIRE:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEB_LAYER3:
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEA_LAYER3:
      /* DEBUG_PRINTF(
          " \nType A Layer 3 card or Mifare Classic/ultralight with merged "
          "atqa detected... \n"); */
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEA_LAYER4:
      /*       DEBUG_PRINTF(
                " \nType A Layer 4 card or Mifare desfire with merged atqa "
                "detected... \n");
            nfcLibStatus = NfcLib_Layer4TypeA_Reference_app(); */
      // TODO: Implement
      break;

    case E_PH_NFCLIB_TYPEB_LAYER4:
      /*       DEBUG_PRINTF(" \nType B Layer 4 card detected... \n");
            nfcLibStatus = NfcLib_TypeB_Reference_app(); */
      // TODO: Implement
      break;

    default:
      break;
  }
  return success;
}

/* *********************** PRIVATE FUNCTIONS ************************ */
void NFCSensor::ExportTagInfo(phacDiscLoop_Sw_DataParams_t* pDataParams,
                              uint16_t tagTechnology, NFCInfo* nfcInfo) {
  nfcInfo->reset();
  uint8_t bTagType;
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_A)) {
    nfcInfo->technology = "A";
    uint8_t UIDsize = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
    uint8_t* UIDptr = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
    nfcInfo->UID = std::vector<uint8_t>(UIDptr, UIDptr + UIDsize);
    uint8_t ATQsize = PHAC_DISCLOOP_I3P3A_MAX_ATQA_LENGTH;
    uint8_t* ATQptr = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa;
    nfcInfo->ATQ = std::vector<uint8_t>(ATQptr, ATQptr + ATQsize);
    nfcInfo->SAK = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
    if (pDataParams->sTypeATargetInfo.bT1TFlag) {
      nfcInfo->type = "1";
    } else {
      bTagType = (pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
      bTagType = bTagType >> 5;
      switch (bTagType) {
        case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK:
          nfcInfo->type = "2";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
          nfcInfo->type = "4A";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
          nfcInfo->type = "P2P";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
          nfcInfo->type = "NFC_DEP & 4A";
          break;
        default:
          break;
      }
    }
  }
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_B)) {
    nfcInfo->technology = "B";
    /* PUPI Length is always 4 bytes */
    uint8_t UIDsize = 0x04;
    uint8_t* UIDptr = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
    nfcInfo->UID = std::vector<uint8_t>(UIDptr, UIDptr + UIDsize);
    uint8_t ATQsize = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength;
    uint8_t* ATQptr = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB;
    nfcInfo->ATQ = std::vector<uint8_t>(ATQptr, ATQptr + ATQsize);
  }
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F212) ||
      PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F424)) {
    nfcInfo->technology = "F";
    uint8_t UIDsize = PHAC_DISCLOOP_FELICA_IDM_LENGTH;
    uint8_t* UIDptr = pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
    nfcInfo->UID = std::vector<uint8_t>(UIDptr, UIDptr + UIDsize);
    if ((pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[0] == 0x01) &&
        (pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[1] == 0xFE)) {
      nfcInfo->type = "P2P";
    } else {
      nfcInfo->type = "3";
    }
    if (pDataParams->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
        PHAC_DISCLOOP_CON_BITR_212) {
      nfcInfo->bitRate = 424;
    } else {
      nfcInfo->bitRate = 212;
    }
  }
}

std::vector<uint8_t> NFCSensor::ReadPage_MFUL_NTAG(uint8_t pageNumber) {
  phNfcLib_TransmitInput.phNfcLib_MifareUltraLight.bPageNumber = pageNumber;
  phNfcLib_TransmitInput.phNfcLib_MifareUltraLight.bCommand = MFUL_Read;
  nfcLibStatus = phNfcLib_Transmit(&phNfcLib_TransmitInput, 0x0);

  if (nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) {
    // cerr << "Read for Block " << +pageNumber << " failed..." << endl;
    return std::vector<uint8_t>();
  }
  /* This parameter has to be reset before every receive */
  uint16_t bytesRead = 256;
  /* To perform receive operation to get back the read data */
  nfcLibStatus =
      phNfcLib_Receive(&bDataBuffer[0], &bytesRead, &bMoreDataAvailable);

  /* The status should be success and the number of bytes received should be 16
   * for MFUL/NTAG cards */
  if ((nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) || (bytesRead != 16)) {
    // cerr << "Read for Block " << +pageNumber << " failed..." << endl;
    return std::vector<uint8_t>();
  }
  return std::vector<uint8_t>(bDataBuffer, bDataBuffer + 4);
}

bool NFCSensor::WritePage_MFUL_NTAG(uint8_t pageNumber,
                                    std::vector<uint8_t> writeData) {
  phNfcLib_TransmitInput.phNfcLib_MifareUltraLight.bPageNumber = pageNumber;
  phNfcLib_TransmitInput.phNfcLib_MifareUltraLight.pBuffer = writeData.data();
  phNfcLib_TransmitInput.phNfcLib_MifareUltraLight.bCommand = MFUL_Write;
  /* Transmit will execute the command */
  nfcLibStatus = phNfcLib_Transmit(
      &phNfcLib_TransmitInput, 0x04 /* For MFUL/NTAG the size of a page
                                       to be written is 4 bytes */
  );

  /* The status should be success, if not break from the application */
  if (nfcLibStatus != PH_NFCLIB_STATUS_SUCCESS) {
    // cerr << "Write to Block " << +pageNumber << " failed..." << endl;
    return false;
  }
  return true;
}
}  // namespace matrix_hal