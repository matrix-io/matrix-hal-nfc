// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_TYPES_H
#define MATRIX_HAL_NFC_TYPES_H

#include <string>
#include <tuple>
#include <vector>

// This MUST be included last!
extern "C" {
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phNfcLib.h>
// TODO: Refine include statements and organize by type
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phCryptoRng.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phCryptoSym.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phalFelica.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phalMfc.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phalTop.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/intfs/phpalFelica.h>
#include <matrix_nfc/nxp_nfc/NxpNfcRdLib/types/ph_Status.h>
}

namespace matrix_hal {
#define INCORRECT_CARD_FOR_FUNCTION 0x377
// Errors from the standard NXP library, not simplfied NXP library.
std::string DescPhStatus(int status_type);

/* If status is negative then it is standard NXP library error, not simplified
NXP library error. Errors are handled this way so that end user can do `cout <<
DescStatus(FunctionName())` and get the error logged. */
std::string DescStatus(int status_type);

std::string DescCardFamily(uint16_t activated_type);

// For detecting card IC
// https://stackoverflow.com/questions/37002498/distinguish-ntag213-from-mf0icu2
// {Chip, Type, Subtype, Major Version, Minor Version, Storage Size}
extern std::vector<uint8_t> NTAG210;
extern std::vector<uint8_t> NTAG212;
extern std::vector<uint8_t> NTAG213;
extern std::vector<uint8_t> NTAG213F;
extern std::vector<uint8_t> NTAG215;
extern std::vector<uint8_t> NTAG216;
extern std::vector<uint8_t> NTAG216F;
// +------------+------+---------+-----------+--------------+
extern std::vector<uint8_t> NT3H1101;
extern std::vector<uint8_t> NT3H1101W0;
extern std::vector<uint8_t> NT3H2111W0;
extern std::vector<uint8_t> NT3H2101;
extern std::vector<uint8_t> NT3H1201W0;
extern std::vector<uint8_t> NT3H2211W0;
// +------------+------+---------+-----------+--------------+
extern std::vector<uint8_t> MF0UL1101;
extern std::vector<uint8_t> MF0ULH1101;
extern std::vector<uint8_t> MF0UL2101;
extern std::vector<uint8_t> MF0ULH2101;
// +------------+------+---------+-----------+--------------+
// {Identification Info, IC Name, User Storage Size (Bytes)}
extern std::vector<std::tuple<std::vector<uint8_t>, std::string, uint32_t>>
    IC_list;
}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_TYPES_H