#ifndef MATRIX_HAL_NFC_DATA_H
#define MATRIX_HAL_NFC_DATA_H

#include "info_content.h"
#include "ndef_content.h"
#include "pages_content.h"

namespace matrix_hal {
class NFCData {
   public:
    InfoContent info = InfoContent();
    PagesContent pages = PagesContent();
    NDEFContent ndef = NDEFContent();
};
};  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_DATA_H