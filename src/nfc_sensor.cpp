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
  bal = (phbalReg_Type_t*)phNfcLib_GetDataParams(PH_COMP_BAL);
  hal = (phhalHw_Nfc_Ic_DataParams_t*)phNfcLib_GetDataParams(PH_COMP_HAL);
  pal_iso14443p3a = (phpalI14443p3a_Sw_DataParams_t*)phNfcLib_GetDataParams(
      PH_COMP_PAL_ISO14443P3A);
  pal_iso14443p3b = (phpalI14443p3b_Sw_DataParams_t*)phNfcLib_GetDataParams(
      PH_COMP_PAL_ISO14443P3B);
  pal_iso14443p4a = (phpalI14443p4a_Sw_DataParams_t*)phNfcLib_GetDataParams(
      PH_COMP_PAL_ISO14443P4A);
  pal_iso14443p4 = (phpalI14443p4_Sw_DataParams_t*)phNfcLib_GetDataParams(
      PH_COMP_PAL_ISO14443P4);
  disc_loop = (phacDiscLoop_Sw_DataParams_t*)phNfcLib_GetDataParams(
      PH_COMP_AC_DISCLOOP);
  /* Disable blocking so that activate function will return upon every iteration
  of discovery loop */
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
int NFCSensor::ReadInfo(NFCInfo* nfc_info) {
  nfc_info->recently_updated = false;
  uint16_t tag_tech_type = 0;
  // Hook into the discovery loop and pull UID/AQT(A/B)/SAK/Type from there
  nfc_lib_status = phacDiscLoop_GetConfig(
      disc_loop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tag_tech_type);
  if (nfc_lib_status != PH_ERR_SUCCESS || tag_tech_type == 0)
    return -nfc_lib_status;
  ExportTagInfo(disc_loop, tag_tech_type, nfc_info);
  return nfc_lib_status;
}

/* This will activate, read and populate info data, then deactivate card */
int NFCSensor::SimpleReadInfo(NFCInfo* nfc_info) {
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

/* This will read a single specified page (Card must already have been
 * activated!) If card is not activated or read fails the vector will be empty.
 * page_number can be 0x00 - 0xFF depending upon card layout
 */
std::vector<uint8_t> NFCSensor::ReadPage_MFUL_NTAG(uint8_t page_number) {
  if (peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
    return std::vector<uint8_t>();
  nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber = page_number;
  nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand = MFUL_Read;
  nfc_lib_status = phNfcLib_Transmit(&nfc_lib_transmit, 0x0);

  if (nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) {
    // Read for Block failed...
    return std::vector<uint8_t>();
  }
  /* This parameter has to be reset before every receive */
  uint16_t bytes_read = 256;
  /* To perform receive operation to get back the read data */
  nfc_lib_status =
      phNfcLib_Receive(&data_buffer[0], &bytes_read, &more_data_available);

  /* The status should be success and the number of bytes received should be 16
   * for MFUL/NTAG cards */
  if ((nfc_lib_status != PH_NFCLIB_STATUS_SUCCESS) || (bytes_read != 16)) {
    // cerr << "Read for Block " << +page_number << " failed..." << endl;
    return std::vector<uint8_t>();
  }
  return std::vector<uint8_t>(data_buffer, data_buffer + 4);
}

/* page_number can be 0x00 - 0xFF depending upon card layout */
int NFCSensor::WritePage_MFUL_NTAG(uint8_t page_number,
                                   std::vector<uint8_t> write_data) {
  // Check if card activated and is a MFUL or NTAG
  if (peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
    return INCORRECT_CARD_FOR_FUNCTION;
  nfc_lib_transmit.phNfcLib_MifareUltraLight.bPageNumber = page_number;
  nfc_lib_transmit.phNfcLib_MifareUltraLight.pBuffer = write_data.data();
  nfc_lib_transmit.phNfcLib_MifareUltraLight.bCommand = MFUL_Write;
  /* Transmit will execute the command */
  nfc_lib_status = phNfcLib_Transmit(
      &nfc_lib_transmit, 0x04 /* For MFUL/NTAG the size of a page
                                       to be written is 4 bytes */
  );
  return nfc_lib_status;
}

int NFCSensor::ReadData_MFUL_NTAG(NFCData* nfc_data) {
  nfc_data->recently_updated = false;
  // Check if card activated and is a MFUL or NTAG
  if (peer_info.dwActivatedType != E_PH_NFCLIB_MIFARE_ULTRALIGHT)
    return INCORRECT_CARD_FOR_FUNCTION;
  nfc_data->Reset();
  uint8_t page_number = 0;
  std::vector<uint8_t> read_buffer;

  do {
    if (!read_buffer.empty()) {
      nfc_data->read_data.emplace_back(read_buffer);
      read_buffer.clear();
    }
    read_buffer = ReadPage_MFUL_NTAG(page_number);
    ++page_number;
  } while (!read_buffer.empty());
  return PH_NFCLIB_STATUS_SUCCESS;
}

/* *********************** PRIVATE FUNCTIONS ************************ */
void NFCSensor::ExportTagInfo(phacDiscLoop_Sw_DataParams_t* disc_loop,
                              uint16_t tag_tech_type, NFCInfo* nfc_info) {
  nfc_info->Reset();
  nfc_info->card_type = DescCardType(peer_info.dwActivatedType);
  uint8_t tag_type;
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                  PHAC_DISCLOOP_POS_BIT_MASK_A)) {
    nfc_info->technology = "A";
    uint8_t UID_size = disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
    uint8_t* UID_ptr = disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
    nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
    uint8_t ATQ_size = PHAC_DISCLOOP_I3P3A_MAX_ATQA_LENGTH;
    uint8_t* ATQ_ptr = disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa;
    nfc_info->ATQ = std::vector<uint8_t>(ATQ_ptr, ATQ_ptr + ATQ_size);
    nfc_info->SAK = disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
    if (disc_loop->sTypeATargetInfo.bT1TFlag) {
      nfc_info->type = "1";
    } else {
      tag_type = (disc_loop->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
      tag_type = tag_type >> 5;
      switch (tag_type) {
        case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK:
          nfc_info->type = "2";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
          nfc_info->type = "4A";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
          nfc_info->type = "P2P";
          break;
        case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
          nfc_info->type = "NFC_DEP & 4A";
          break;
        default:
          break;
      }
    }
  }
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                  PHAC_DISCLOOP_POS_BIT_MASK_B)) {
    nfc_info->technology = "B";
    /* PUPI Length is always 4 bytes */
    uint8_t UID_size = 0x04;
    uint8_t* UID_ptr = disc_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
    nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
    uint8_t ATQ_size = disc_loop->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength;
    uint8_t* ATQ_ptr = disc_loop->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB;
    nfc_info->ATQ = std::vector<uint8_t>(ATQ_ptr, ATQ_ptr + ATQ_size);
  }
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F212) ||
      PHAC_DISCLOOP_CHECK_ANDMASK(tag_tech_type,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F424)) {
    nfc_info->technology = "F";
    uint8_t UID_size = PHAC_DISCLOOP_FELICA_IDM_LENGTH;
    uint8_t* UID_ptr = disc_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
    nfc_info->UID = std::vector<uint8_t>(UID_ptr, UID_ptr + UID_size);
    if ((disc_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[0] == 0x01) &&
        (disc_loop->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[1] == 0xFE)) {
      nfc_info->type = "P2P";
    } else {
      nfc_info->type = "3";
    }
    if (disc_loop->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
        PHAC_DISCLOOP_CON_BITR_212) {
      nfc_info->bit_rate = 424;
    } else {
      nfc_info->bit_rate = 212;
    }
  }
}
}  // namespace matrix_hal