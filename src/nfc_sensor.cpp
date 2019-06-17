// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#include "nfc_sensor.h"

// using std::cout;
// using std::endl;

void ExportTagInfo(phacDiscLoop_Sw_DataParams_t *pDataParams,
                   uint16_t tagTechnology, matrix_hal::NFCData *nfcData) {
  nfcData->reset();
  uint8_t bTagType;

#if defined(NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS) ||                             \
    defined(NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_ACTIVE)
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_A)) {
    nfcData->technology = "Type A";
    uint8_t UIDsize = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
    uint8_t *UIDptr = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    // std::cout << std::endl;
    // phApp_Print_Buff(pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid,
    //                  pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize);
    // std::cout << std::endl;
    nfcData->SAK = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak;
    /* Bit b3 is set to zero, [Digital] 4.8.2 */
    /* Mask out all other bits except for b7 and b6 */

    if (pDataParams->sTypeATargetInfo.bT1TFlag)
      nfcData->type = "Type 1 Tag";
    else {
      bTagType = (pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
      bTagType = bTagType >> 5;
      switch (bTagType) {
      case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK:
        nfcData->type = "Type 2 Tag";
        //   DEBUG_PRINTF("\n\t\tType: Type 2 Tag\n");
        break;
      case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
        nfcData->type = "Type 4A Tag";
        //   DEBUG_PRINTF("\n\t\tType: Type 4A Tag\n");
        break;
      case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
        nfcData->type = "Type P2P";
        //   DEBUG_PRINTF("\n\t\tType: P2P\n");
        break;
      case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
        nfcData->type = "Type NFC_DEP and 4A Tag";
        //   DEBUG_PRINTF("\n\t\tType: Type NFC_DEP and  4A Tag\n");
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
    // DEBUG_PRINTF("\tTechnology  : Type B");
    nfcData->technology = "Type B";
    /* Loop through all the Type B tags detected and print the Pupi */
    // DEBUG_PRINTF("\n\t\tCard: %d", 0 + 1);
    // DEBUG_PRINTF("\n\t\tUID :");
    /* PUPI Length is always 4 bytes */
    uint8_t UIDsize = 0x04;
    uint8_t *UIDptr = pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    // phApp_Print_Buff(pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aPupi,
    // 0x04); DEBUG_PRINTF("\n");
  }
#endif

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F212) ||
      PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_F424)) {
    // DEBUG_PRINTF("\tTechnology  : Type F");
    nfcData->technology = "Type F";

    /* Loop through all the type F tags and print the IDm */
    // DEBUG_PRINTF("\n\t\tCard: %d", 0 + 1);
    // DEBUG_PRINTF("\n\t\tUID :");
    uint8_t UIDsize = PHAC_DISCLOOP_FELICA_IDM_LENGTH;
    uint8_t *UIDptr = pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    // phApp_Print_Buff(pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm,
    //                  PHAC_DISCLOOP_FELICA_IDM_LENGTH);
    if ((pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[0] == 0x01) &&
        (pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm[1] == 0xFE)) {
      /* This is Type F tag with P2P capabilities */
      nfcData->type = "P2P";
      // DEBUG_PRINTF("\n\t\tType: P2P");
    } else {
      /* This is Type F T3T tag */
      nfcData->type = "Type 3 Tag";
      // DEBUG_PRINTF("\n\t\tType: Type 3 Tag");
    }

    if (pDataParams->sTypeFTargetInfo.aTypeFTag[0].bBaud !=
        PHAC_DISCLOOP_CON_BITR_212) {
      nfcData->bitRate = 424;
      // DEBUG_PRINTF("\n\t\tBit Rate: 424\n");
    } else {
      nfcData->bitRate = 212;
      // DEBUG_PRINTF("\n\t\tBit Rate: 212\n");
    }
  }
#endif

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_V)) {
    // DEBUG_PRINTF("\tTechnology  : Type V / ISO 15693 / T5T");
    nfcData->technology = "Type V / ISO 15693 / T5T";
    /* Loop through all the Type V tags detected and print the UIDs */
    // DEBUG_PRINTF("\n\t\tCard: %d", 0 + 1);
    // DEBUG_PRINTF("\n\t\tUID :");
    uint8_t UIDsize = 0x08;
    uint8_t *UIDptr = pDataParams->sTypeVTargetInfo.aTypeV[0].aUid;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    // phApp_Print_Buff(pDataParams->sTypeVTargetInfo.aTypeV[0].aUid, 0x08);

    // DEBUG_PRINTF("\n");
  }
#endif

#ifdef NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS
  if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechnology,
                                  PHAC_DISCLOOP_POS_BIT_MASK_18000P3M3)) {
    // DEBUG_PRINTF("\tTechnology  : ISO 18000p3m3 / EPC Gen2");
    nfcData->technology = "ISO 18000p3m3 / EPC Gen2";
    /* Loop through all the 18000p3m3 tags detected and print the UII */
    // DEBUG_PRINTF("\n\t\tCard: %d", 0 + 1);
    // DEBUG_PRINTF("\n\t\tUII :");
    uint8_t UIDsize =
        pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[0].wUiiLength / 8;
    uint8_t *UIDptr = pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[0].aUii;
    nfcData->UID = std::vector<int>(UIDptr, UIDptr + UIDsize);
    // phApp_Print_Buff(
    //     pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[0].aUii,
    //     (pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[0].wUiiLength / 8));

    // DEBUG_PRINTF("\n");
  }
#endif
}

namespace matrix_hal {
NFCSensor::NFCSensor() { /* Stub */
  phStatus_t status = PH_ERR_INTERNAL_ERROR;
  phOsal_ThreadObj_t BasicDisc;

  /* Initialize the Controller */
  phPlatform_Controller_Init();

  /*Perform OSAL Init first before everything*/
  phOsal_Init();

  /* Perform Platform Init */
  status = phPlatform_Init(&sPlatform, bHalBufferTx, sizeof(bHalBufferTx),
                           bHalBufferRx, sizeof(bHalBufferRx));
  CHECK_STATUS(status);
  if (status != PH_ERR_SUCCESS)
    throw "NFC Sensor Init Failed";

  /* Initialize Reader Library PAL/AL Components */
  status = phApp_RdLibInit();
  CHECK_STATUS(status);

  if (status != PH_ERR_SUCCESS)
    throw "NFC Sensor Init Failed";

  /* Set the generic pointer */
  pHal = &sPlatform.sHal;
}

void NFCSensor::Read(NFCData *nfcData) {
  nfcData->recentlyUpdated = false;
  void *pDataParams = &sDiscLoop;
  phStatus_t status = PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED;
  uint16_t tagTechType = 0;
  uint16_t numberOfTags = 0;
  uint16_t wEntryPoint;
  uint16_t wValue;

  status = phApp_HALConfigAutoColl();
  CHECK_STATUS(status);

  /* Get Poll Configuration */
  status = phacDiscLoop_GetConfig(
      pDataParams, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, &bSavePollTechCfg);
  CHECK_STATUS(status);

  /* Start in poll mode */
  wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
  status = PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED;

  /* Set Discovery Poll State to Detection */
  status =
      phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE,
                             PHAC_DISCLOOP_POLL_STATE_DETECTION);
  CHECK_STATUS(status);

  /* Set Poll Configuration */
  status = phacDiscLoop_SetConfig(
      pDataParams, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, bSavePollTechCfg);
  CHECK_STATUS(status);

  /* Switch off RF field */
  status = phhalHw_FieldOff(pHal);
  CHECK_STATUS(status);

  /* Start discovery loop */
  status = phacDiscLoop_Run(pDataParams, wEntryPoint);

  if (wEntryPoint == PHAC_DISCLOOP_ENTRY_POINT_POLL) {

    if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_MULTI_TECH_DETECTED) {
      // DEBUG_PRINTF(" \n Multiple technology detected: \n");

      status = phacDiscLoop_GetConfig(
          pDataParams, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
      CHECK_STATUS(status);

      // if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechType,
      //                                 PHAC_DISCLOOP_POS_BIT_MASK_A)) {
      //   DEBUG_PRINTF(" \tType A detected... \n");
      // }
      // if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechType,
      //                                 PHAC_DISCLOOP_POS_BIT_MASK_B)) {
      //   DEBUG_PRINTF(" \tType B detected... \n");
      // }
      // if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechType,
      //                                 PHAC_DISCLOOP_POS_BIT_MASK_F212)) {
      //   DEBUG_PRINTF(" \tType F detected with baud rate 212... \n");
      // }
      // if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechType,
      //                                 PHAC_DISCLOOP_POS_BIT_MASK_F424)) {
      //   DEBUG_PRINTF(" \tType F detected with baud rate 424... \n");
      // }
      // if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechType,
      //                                 PHAC_DISCLOOP_POS_BIT_MASK_V)) {
      //   DEBUG_PRINTF(" \tType V / ISO 15693 / T5T detected... \n");
      // }

      /* Select 1st Detected Technology to Resolve*/
      if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechType, 1)) {
        /* Configure for one of the detected technology */
        status = phacDiscLoop_SetConfig(
            pDataParams, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, 1);
        CHECK_STATUS(status);
      }

      /* Print the technology resolved */
      // phApp_PrintTech(1);

      /* Set Discovery Poll State to collision resolution */
      status = phacDiscLoop_SetConfig(
          pDataParams, PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE,
          PHAC_DISCLOOP_POLL_STATE_COLLISION_RESOLUTION);
      CHECK_STATUS(status);

      /* Restart discovery loop in poll mode from collision resolution phase */
      status = phacDiscLoop_Run(pDataParams, wEntryPoint);
    }

    if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_MULTI_DEVICES_RESOLVED) {
      /* Get Detected Technology Type */
      status = phacDiscLoop_GetConfig(
          &sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
      CHECK_STATUS(status);

      /* Get number of tags detected */
      status = phacDiscLoop_GetConfig(
          &sDiscLoop, PHAC_DISCLOOP_CONFIG_NR_TAGS_FOUND, &numberOfTags);
      CHECK_STATUS(status);

      // DEBUG_PRINTF(" \n Multiple cards resolved: %d cards \n",
      // numberOfTags); phApp_PrintTagInfo(pDataParams, numberOfTags,
      // tagTechType);
      ExportTagInfo(pDataParams, tagTechType, nfcData);

      if (numberOfTags > 1) {
        /* Get 1st Detected Tag and Activate device at index 0 */
        if (PHAC_DISCLOOP_CHECK_ANDMASK(tagTechType, 1)) {
          // DEBUG_PRINTF("\t Activating one card...\n");
          status = phacDiscLoop_ActivateCard(pDataParams, 0, 0);
        }

        if (((status & PH_ERR_MASK) == PHAC_DISCLOOP_DEVICE_ACTIVATED) ||
            ((status & PH_ERR_MASK) ==
             PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED) ||
            ((status & PH_ERR_MASK) == PHAC_DISCLOOP_MERGED_SEL_RES_FOUND)) {
          /* Get Detected Technology Type */
          status = phacDiscLoop_GetConfig(
              &sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
          CHECK_STATUS(status);

          // phApp_PrintTagInfo(pDataParams, 0x01, tagTechType);
          ExportTagInfo(pDataParams, tagTechType, nfcData);
        } else {
          // PRINT_INFO("\t\tCard activation failed...\n");
        }
      }
      /* Switch to LISTEN mode after POLL mode */
    } else if (((status & PH_ERR_MASK) == PHAC_DISCLOOP_NO_TECH_DETECTED) ||
               ((status & PH_ERR_MASK) == PHAC_DISCLOOP_NO_DEVICE_RESOLVED)) {
      /* Switch to LISTEN mode after POLL mode */
    } else if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_EXTERNAL_RFON) {
      /*
       * If external RF is detected during POLL, return back so that the
       * application can restart the loop in LISTEN mode
       */
    } else if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_MERGED_SEL_RES_FOUND) {
      // DEBUG_PRINTF(" \n Device having T4T and NFC-DEP support detected...
      // \n");

      /* Get Detected Technology Type */
      status = phacDiscLoop_GetConfig(
          &sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
      CHECK_STATUS(status);

      // phApp_PrintTagInfo(pDataParams, 1, tagTechType);
      ExportTagInfo(pDataParams, tagTechType, nfcData);

      /* Switch to LISTEN mode after POLL mode */
    } else if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_DEVICE_ACTIVATED) {
      // DEBUG_PRINTF(" \n Card detected and activated successfully... \n");
      status = phacDiscLoop_GetConfig(
          pDataParams, PHAC_DISCLOOP_CONFIG_NR_TAGS_FOUND, &numberOfTags);
      CHECK_STATUS(status);

      /* Get Detected Technology Type */
      status = phacDiscLoop_GetConfig(
          pDataParams, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
      CHECK_STATUS(status);

      // uint8_t i = phApp_PrintUUID(pDataParams, pBuffUUID);

      // DEBUG_PRINTF("\n\t\tUUID :");
      // phApp_Print_Buff(pBuffUUID, i);

      // phApp_PrintTagInfo(pDataParams, numberOfTags, tagTechType);
      ExportTagInfo(pDataParams, tagTechType, nfcData);

      /* Switch to LISTEN mode after POLL mode */
    } else if ((status & PH_ERR_MASK) ==
               PHAC_DISCLOOP_ACTIVE_TARGET_ACTIVATED) {
      // DEBUG_PRINTF(" \n Active target detected... \n");
      /* Switch to LISTEN mode after POLL mode */
    } else if ((status & PH_ERR_MASK) ==
               PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED) {
      // DEBUG_PRINTF(" \n Passive target detected... \n");

      /* Get Detected Technology Type */
      status = phacDiscLoop_GetConfig(
          &sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &tagTechType);
      CHECK_STATUS(status);

      // phApp_PrintTagInfo(pDataParams, 1, tagTechType);
      ExportTagInfo(pDataParams, tagTechType, nfcData);

      /* Switch to LISTEN mode after POLL mode */
    } else if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED) {
      /* LPCD is succeed but no tag is detected. */
    } else {
      if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_FAILURE) {
        status = phacDiscLoop_GetConfig(
            pDataParams, PHAC_DISCLOOP_CONFIG_ADDITIONAL_INFO, &wValue);
        CHECK_STATUS(status);
        // DEBUG_ERROR_PRINT(PrintErrorInfo(wValue));
      } else {
        // DEBUG_ERROR_PRINT(PrintErrorInfo(status));
      }
    }

    /* Update the Entry point to LISTEN mode. */
    wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
  } else {

    if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_EXTERNAL_RFOFF) {
      /*
       * Enters here if in the target/card mode and external RF is not available
       * Wait for LISTEN timeout till an external RF is detected.
       * Application may choose to go into standby at this point.
       */
      status = phhalHw_EventConsume(pHal);
      CHECK_STATUS(status);

      status = phhalHw_SetConfig(pHal, PHHAL_HW_CONFIG_RFON_INTERRUPT, PH_ON);
      CHECK_STATUS(status);

      status = phhalHw_EventWait(pHal, LISTEN_PHASE_TIME_MS);
      if ((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT) {
        wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
      } else {
        wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
      }
    } else {
      if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_ACTIVATED_BY_PEER) {
        // DEBUG_PRINTF(" \n Device activated in listen mode... \n");
      } else if ((status & PH_ERR_MASK) == PH_ERR_INVALID_PARAMETER) {
        /* In case of Front end used is RC663, then listen mode is not
         * supported. Switch from listen mode to poll mode. */
      } else {
        if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_FAILURE) {
          status = phacDiscLoop_GetConfig(
              pDataParams, PHAC_DISCLOOP_CONFIG_ADDITIONAL_INFO, &wValue);
          CHECK_STATUS(status);
          // DEBUG_ERROR_PRINT(PrintErrorInfo(wValue));
        } else {
          // DEBUG_ERROR_PRINT(PrintErrorInfo(status));
        }
      }

      /* On successful activated by Peer, switch to LISTEN mode */
      wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
    }
  }
}
} // namespace matrix_hal