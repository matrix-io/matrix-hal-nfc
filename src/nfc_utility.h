// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_UTILITY_H
#define MATRIX_HAL_NFC_UTILITY_H

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "nfc_info.h"

#include "nfc_init.h"

namespace matrix_hal {

class NFCUtility {
   public:
    NFCInit* nfc_init;
    NFCUtility(NFCInit* nfc_init) : nfc_init(nfc_init){};
    std::pair<std::string, int> GetCardIC();
    uint8_t ExportTag(uint16_t tag_tech_type, NFCInfo* nfc_info);
};

// TODO : REFACTOR BELOW

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

#endif  // MATRIX_HAL_NFC_UTILITY_H