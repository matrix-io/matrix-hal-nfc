
#ifndef MATRIX_HAL_NFC_STATUS_H
#define MATRIX_HAL_NFC_STATUS_H

#include <string>

#include "nfc_init.h"

namespace matrix_hal {

/* If status is negative then it is standard NXP library error, not
simplified NXP library error. Errors are handled this way so that end user
can do `cout << NFCStatus(FunctionName())` and get the error logged. */
std::string NFCStatus(int64_t status_type);

// HELPERS BELOW
// Errors from the standard NXP lib, not simplified NXP lib.
std::string NXPLibStatus(uint32_t status_type);
// Errors from the simplified NXP lib.
std::string NXPSimplifiedLibStatus(uint32_t status_type);

// Defines for wrapper errors
#define INCORRECT_CARD_FOR_FUNCTION 0x90

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_STATUS_H