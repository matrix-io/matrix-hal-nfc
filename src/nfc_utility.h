// NOTE: This file integrates with the NXP NFC Reader Library v4.040.05

#ifndef MATRIX_HAL_NFC_UTILITY_H
#define MATRIX_HAL_NFC_UTILITY_H

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "data/info_content.h"

#include "nfc_init.h"

namespace matrix_hal {

class NFCUtility {
   public:
    NFCInit* nfc_init;
    NFCUtility(NFCInit* nfc_init) : nfc_init(nfc_init){};
    std::string GetCardFamily();
    std::pair<std::string, int> GetCardIC();
    uint8_t ExportTag(uint16_t tag_tech_type, InfoContent* nfc_info);
    // For detecting card IC
    // https://stackoverflow.com/questions/37002498/distinguish-ntag213-from-mf0icu2
    // {Type, Subtype, Major Version, Minor Version, Storage Size}
    std::vector<uint8_t> NTAG210 = {0x04, 0x01, 0x01, 0x00, 0x0B};
    std::vector<uint8_t> NTAG212 = {0x04, 0x01, 0x01, 0x00, 0x0E};
    std::vector<uint8_t> NTAG213 = {0x04, 0x02, 0x01, 0x00, 0x0F};
    std::vector<uint8_t> NTAG213F = {0x04, 0x04, 0x01, 0x00, 0x0F};
    std::vector<uint8_t> NTAG215 = {0x04, 0x02, 0x01, 0x00, 0x11};
    std::vector<uint8_t> NTAG216 = {0x04, 0x02, 0x01, 0x00, 0x13};
    std::vector<uint8_t> NTAG216F = {0x04, 0x04, 0x01, 0x00, 0x13};
    // +------------+------+---------+-----------+--------------+
    std::vector<uint8_t> NT3H1101 = {0x04, 0x02, 0x01, 0x01, 0x13};
    std::vector<uint8_t> NT3H1101W0 = {0x04, 0x05, 0x02, 0x01, 0x13};
    std::vector<uint8_t> NT3H2111W0 = {0x04, 0x05, 0x02, 0x02, 0x13};
    std::vector<uint8_t> NT3H2101 = {0x04, 0x02, 0x01, 0x01, 0x15};
    std::vector<uint8_t> NT3H1201W0 = {0x04, 0x05, 0x02, 0x01, 0x15};
    std::vector<uint8_t> NT3H2211W0 = {0x04, 0x05, 0x02, 0x02, 0x15};
    // +------------+------+---------+-----------+--------------+
    std::vector<uint8_t> MF0UL1101 = {0x03, 0x01, 0x01, 0x00, 0x0B};
    std::vector<uint8_t> MF0ULH1101 = {0x03, 0x02, 0x01, 0x00, 0x0B};
    std::vector<uint8_t> MF0UL2101 = {0x03, 0x01, 0x01, 0x00, 0x0E};
    std::vector<uint8_t> MF0ULH2101 = {0x03, 0x02, 0x01, 0x00, 0x0E};
    // +------------+------+---------+-----------+--------------+
    // {Identification Info, IC Name, User Storage Size (Bytes)}
    std::vector<std::tuple<std::vector<uint8_t>, std::string, uint32_t>>
        IC_list = {
            {NTAG210, "NTAG210", 48},         {NTAG212, "NTAG212", 128},
            {NTAG213, "NTAG213", 144},        {NTAG213F, "NTAG213F", 144},
            {NTAG215, "NTAG215", 504},        {NTAG216, "NTAG216", 888},
            {NTAG216F, "NTAG216F", 888},      {NT3H1101, "NT3H1101", 888},
            {NT3H1101W0, "NT3H1101W0", 888},  {NT3H2111W0, "NT3H2111W0", 1912},
            {NT3H2101, "NT3H2101", 888},      {NT3H1201W0, "NT3H1201W0", 1904},
            {NT3H2211W0, "NT3H2211W0", 1912}, {MF0UL1101, "MF0UL1101", 48},
            {MF0ULH1101, "MF0ULH1101", 48},   {MF0UL2101, "MF0UL2101", 128},
            {MF0ULH2101, "MF0ULH2101", 128}};
};

}  // namespace matrix_hal

#endif  // MATRIX_HAL_NFC_UTILITY_H