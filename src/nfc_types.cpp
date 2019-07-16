
#include "nfc_types.h"

namespace matrix_hal {
// Errors from the standard NXP library, not simplfied NXP library.
std::string DescPhStatus(int status_type) {
    switch (status_type & PH_ERR_MASK) {
        case PH_ERR_IO_TIMEOUT:
            return "IO Timeout, no reply received";
        case PH_ERR_INTEGRITY_ERROR:
            return "Integrity Error, wrong CRC or parity detected";
        case PH_ERR_COLLISION_ERROR:
            return "Collision Error";
        case PH_ERR_BUFFER_OVERFLOW:
            return "Buffer Overflow, attempted to write beyond buffer size";
        case PH_ERR_FRAMING_ERROR:
            return "Framing Error, invalid frame format";
        case PH_ERR_PROTOCOL_ERROR:
            return "Protocol Error, response violated protocol";
        case PH_ERR_AUTH_ERROR:
            return "Authentication Error";
        case PH_ERR_READ_WRITE_ERROR:
            return "Read/Write Error, occured in RAM/ROM or Flash";
        case PH_ERR_TEMPERATURE_ERROR:
            return "Temperature Error, RC sensors detected overheating";
        case PH_ERR_RF_ERROR:
            return "RF Error";
        case PH_ERR_INTERFACE_ERROR:
            return "Interface Error, occured in RC communication";
        case PH_ERR_LENGTH_ERROR:
            return "Length Error";
        case PH_ERR_RESOURCE_ERROR:
            return "Resource Error";
        case PH_ERR_TX_NAK_ERROR:
            return "NAK Error, TX rejected sanely by the counterpart";
        case PH_ERR_RX_NAK_ERROR:
            return "NAK Error, RX request rejected sanely by the counterpart";
        case PH_ERR_EXT_RF_ERROR:
            return "Error due to External RF";
        case PH_ERR_NOISE_ERROR:
            return "EMVCo EMD Noise Error";
        case PH_ERR_ABORTED:
            return "HAL shutdown was called";
        case PH_ERR_INTERNAL_ERROR:
            return "Internal Error";
        case PH_ERR_INVALID_DATA_PARAMS:
            return "Invalid Data Parameters Error, layer ID check failed";
        case PH_ERR_INVALID_PARAMETER:
            return "Parameter Error, invalid parameter supplied";
        case PH_ERR_PARAMETER_OVERFLOW:
            return "Parameter Overflow, reading/writing produced an overflow";
        case PH_ERR_UNSUPPORTED_PARAMETER:
            return "Parameter not supported";
        case PH_ERR_UNSUPPORTED_COMMAND:
            return "Command not supported";
        case PH_ERR_USE_CONDITION:
            return "Condition Error";
        case PH_ERR_KEY:
            return "Key Error";
        case PH_ERR_OSAL_ERROR:
            return "OSAL Error occurred during initialization";
        case PHAC_DISCLOOP_FAILURE:
            return "Failure due to error from lower layer";
        case PHAC_DISCLOOP_COLLISION_PENDING:
            return "Collision pending";
        case PHAC_DISCLOOP_EXTERNAL_RFON:
            return "External RF field on";
        case PHAC_DISCLOOP_EXTERNAL_RFOFF:
            return "External RF field off";
        case PHAC_DISCLOOP_NO_TECH_DETECTED:
            return "No card/device detected";
        case PHAC_DISCLOOP_NO_DEVICE_RESOLVED:
            return "No card/device resolved";
        case PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED:
            return "LPCD succeeded but no card/device detected";
        case PHAC_DISCLOOP_MULTI_TECH_DETECTED:
            return "Multiple cards/devices detected";
        case PHAC_DISCLOOP_MULTI_DEVICES_RESOLVED:
            return "Multiple cards/devices resolved";
        default:
            return "Unknown Error";
            break;
    }
}

/* If status is negative then it is standard NXP library error, not simplified
NXP library error. Errors are handled this way so that end user can do `cout <<
DescStatus(FunctionName())` and get the error logged. */
std::string DescStatus(int status_type) {
    if (status_type < 0)
        return DescPhStatus(-status_type);
    else {
        switch (status_type) {
            case PH_NFCLIB_STATUS_SUCCESS:
                return "Success";
                break;
            case PH_NFCLIB_STATUS_INVALID_STATE:
                return "Invalid State";
                break;
            case PH_NFCLIB_STATUS_INVALID_PARAMETER:
                return "Invalid Parameter";
                break;
            case PH_NFCLIB_STATUS_BUFFER_OVERFLOW:
                return "Buffer Overflow";
                break;
            case PH_NFCLIB_STATUS_ABORTED:
                return "Aborted";
                break;
            case PH_NFCLIB_STATUS_INTERNAL_ERROR:
                return "Internal Error";
                break;
            case PH_NFCLIB_STATUS_PEER_ACTIVATION_DONE:
                return "Activation Done";
                break;
            case PH_NFCLIB_STATUS_MULTIPLE_PEERS_DETECTED:
                return "Multiple Peers Detected";
                break;
            case PH_NFCLIB_STATUS_PEER_ACTIVATION_FAILED:
                return "Peer Activation Failed";
                break;
            case PH_NFCLIB_STATUS_NO_PEER_DISCOVERED:
                return "No Peer Discovered";
                break;
            case PH_NFCLIB_STATUS_PEER_DEACTIVATION_FAILED:
                return "Peer Deactivation Failed";
                break;
            case PH_NFCLIB_STATUS_RF_TRANSMISSION_ERROR:
                return "RF Transmission Error";
                break;
            case PH_NFCLIB_STATUS_RF_TIMEOUT_ERROR:
                return "RF Timeout Error";
                break;
            case PH_NFCLIB_STATUS_RF_PROTOCOL_ERROR:
                return "RF Protocol Error";
                break;
            case PH_NFCLIB_STATUS_RF_PEER_LOST:
                return "RF Peer Lost";
                break;
            case INCORRECT_CARD_FOR_FUNCTION:
                return "Incorrect Card Type For Function";
                break;
            default:
                return "Unknown Error";
                break;
        }
    }
}

std::string DescCardFamily(uint16_t activated_type) {
    switch (activated_type) {
        case E_PH_NFCLIB_MIFARE_CLASSIC_1K:
            return "Mifare Classic 1K";
            break;
        case E_PH_NFCLIB_MIFARE_CLASSIC_4K:
            return "Mifare Classic 4K";
            break;
        case E_PH_NFCLIB_MIFARE_ULTRALIGHT:
            return "Mifare Ultralight | NTAG";
            break;
        case E_PH_NFCLIB_MIFARE_DESFIRE:
            return "Mifare DESfire";
            break;
        case E_PH_NFCLIB_TYPEB_LAYER3:
            return "Type B Layer 3";
            break;
        case E_PH_NFCLIB_TYPEA_LAYER3:
            return "Mifare Classic | Mifare Ultralight | Type A Layer 3";
            break;
        case E_PH_NFCLIB_TYPEA_LAYER4:
            return "Mifare DESFire | Type A Layer 4";
            break;
        case E_PH_NFCLIB_TYPEB_LAYER4:
            return "Type B Layer 4";
            break;
        default:
            return "Unknown Card";
            break;
    }
}

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
std::vector<std::tuple<std::vector<uint8_t>, std::string, uint32_t>> IC_list = {
    {NTAG210, "NTAG210", 48},         {NTAG212, "NTAG212", 128},
    {NTAG213, "NTAG213", 144},        {NTAG213F, "NTAG213F", 144},
    {NTAG215, "NTAG215", 504},        {NTAG216, "NTAG216", 888},
    {NTAG216F, "NTAG216F", 888},      {NT3H1101, "NT3H1101", 888},
    {NT3H1101W0, "NT3H1101W0", 888},  {NT3H2111W0, "NT3H2111W0", 1912},
    {NT3H2101, "NT3H2101", 888},      {NT3H1201W0, "NT3H1201W0", 1904},
    {NT3H2211W0, "NT3H2211W0", 1912}, {MF0UL1101, "MF0UL1101", 48},
    {MF0ULH1101, "MF0ULH1101", 48},   {MF0UL2101, "MF0UL2101", 128},
    {MF0ULH2101, "MF0ULH2101", 128}};

}  // namespace matrix_hal