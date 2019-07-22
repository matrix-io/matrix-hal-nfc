#ifndef MATRIX_HAL_NFC_DATA_H
#define MATRIX_HAL_NFC_DATA_H

#include "data/info_content.h"
#include "data/ndef_content.h"
#include "data/pages_content.h"

#include "ndef/ndef_parse/NdefMessage.h"

namespace matrix_hal {
class NFCData {
   public:
    InfoContent info = InfoContent();
    PagesContent pages = PagesContent();
    NDEFContent ndef = NDEFContent();
};
};  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_DATA_H